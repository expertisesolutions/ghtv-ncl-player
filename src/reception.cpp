/* (c) Copyright 2011-2014 Felipe Magno de Almeida
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ghtv/qt/reception.hpp>

#include <gts/sdt_private.hpp>
#include <gts/sections/extended_section.hpp>
#include <gts/descriptors/service_descriptor.hpp>

#include <boost/range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/shared_array.hpp>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QMetaType>

#include <linux/dvb/dmx.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stropts.h>
#include <errno.h>
#include <poll.h>

#include <iostream>

#include <cstring>

namespace ghtv { namespace qt {

reception::reception ()
  : channels_list(0), adapter_edit(0), frontend_edit(0)
  , scan_button(0), stop_scan_button(0), select_button(0), progress_bar(0), frontendfd(0)
  , scan_thread(0), scan_in_progress(false), should_stop_scan(false)
{
  setAttribute(Qt::WA_DeleteOnClose);
  QGridLayout* vertical_layout = new QGridLayout;
  channels_list = new QListWidget;
  vertical_layout->addWidget(channels_list, 1, 0, 10, 1);

  QHBoxLayout* text_layout = new QHBoxLayout;
  text_layout->addWidget(new QLabel("/dev/dvb/adapter"));
  adapter_edit = new QLineEdit("0");
  text_layout->addWidget(adapter_edit);
  text_layout->addWidget(new QLabel("/frontend"));
  frontend_edit = new QLineEdit("0");
  text_layout->addWidget(frontend_edit);
  scan_button = new QPushButton("&Scan");
  text_layout->addWidget(scan_button);
  stop_scan_button = new QPushButton("St&op");
  stop_scan_button->setEnabled(false);
  text_layout->addWidget(stop_scan_button);

  vertical_layout->addLayout(text_layout, 0, 0);

  select_button = new QPushButton("S&elect");
  select_button->setEnabled(false);
  vertical_layout->addWidget(select_button, 1, 1);

  progress_bar = new QProgressBar;
  progress_bar->setMaximum(sizeof(frequencies)/sizeof (frequencies[0])-1);
  progress_bar->setMinimum(0u);
  progress_bar->setVisible(false);
  vertical_layout->addWidget(progress_bar, 11, 0, 1, 2);

  setLayout(vertical_layout);

  QObject::connect(stop_scan_button, SIGNAL(clicked ()), this, SLOT(stop_scan ()));
  QObject::connect(scan_button, SIGNAL(clicked ()), this, SLOT(scan ()));
  QObject::connect(select_button, SIGNAL(clicked ()), this, SLOT(select ()));
  QObject::connect(channels_list, SIGNAL(currentItemChanged (QListWidgetItem*, QListWidgetItem*)), this, SLOT(channels_list_selection_changed (QListWidgetItem*, QListWidgetItem*)));
}

reception::~reception()
{
  Q_EMIT destroying_window_signal();
  if(scan_thread)
  {
    scan_thread->exit();
    scan_thread->wait();
    delete scan_thread;
  }
  if(frontendfd)
    ::close(frontendfd);
}

reception_thread::reception_thread(int frontendfd
                                   , std::string adapter_index, std::string frontend_index)
    : frontendfd(frontendfd)
    , adapter_index(adapter_index), frontend_index(frontend_index)
{
  moveToThread(this);
  QObject::connect(this, SIGNAL (frequency_locked_signal ())
                   , this, SLOT (filter_sdt ())
                   , Qt::QueuedConnection);
}

void reception_thread::lock_frequency (unsigned int frequency)
{
  std::cout << "reception_thread::lock_frequency" << std::endl;
  dvb_frontend_parameters parameters;
  parameters.frequency = frequency;
  parameters.inversion = INVERSION_AUTO;
  parameters.u.ofdm = dvb_ofdm_parameters ();
  parameters.u.ofdm.bandwidth = BANDWIDTH_6_MHZ;
  parameters.u.ofdm.code_rate_HP = parameters.u.ofdm.code_rate_LP = FEC_AUTO;
  parameters.u.ofdm.constellation = QAM_AUTO;
  parameters.u.ofdm.transmission_mode = TRANSMISSION_MODE_AUTO;
  parameters.u.ofdm.guard_interval = GUARD_INTERVAL_AUTO;
  parameters.u.ofdm.hierarchy_information = HIERARCHY_AUTO;

  if(ioctl(frontendfd, FE_SET_FRONTEND, &parameters) == -1)
  {
    std::cout << "couldn't set parameter" << std::endl;
    Q_EMIT lock_error_signal ();
    return;
  }

  state = frequency_locking_state (frequency);

  QTimer::singleShot(200, this, SLOT(check_lock_frequency ()));
}

void reception_thread::check_lock_frequency ()
{
  std::cout << "check_lock_frequency" << std::endl;
  // must be frequency locking
  assert(!!boost::get<frequency_locking_state>(&state));
  frequency_locking_state& s = boost::get<frequency_locking_state>(state);

  fe_status_t status;
  if(ioctl(frontendfd, FE_READ_STATUS, &status) == -1)
  {
    std::cout << "Error ioctl read status" << std::endl;
    Q_EMIT lock_error_signal ();
    return;
  }

  if(status & FE_HAS_LOCK)
  {
    std::cout << "reception_thread has locked frequency" << std::endl;
    state = frequency_lock_state(s.frequency);
    Q_EMIT frequency_locked_signal ();
  }
  else
  {
    if(++s.retries == 10)
      Q_EMIT lock_error_signal ();
    else
      QTimer::singleShot(200, this, SLOT(check_lock_frequency ()));
  }
}

void reception_thread::filter_sdt ()
{
  std::cout << "filter_sdt" << std::endl;
  std::string demux_path = "/dev/dvb/adapter" + adapter_index
    + "/demux" + frontend_index;
    
  int demuxfd = open(demux_path.c_str (), O_RDWR|O_NONBLOCK);
  if(!demuxfd)
  {
    std::cout << "couldn't open demux" << std::endl;
    Q_EMIT lock_error_signal ();
    return;
  }

  assert(!!boost::get<frequency_lock_state>(&state));
  frequency_lock_state& s = boost::get<frequency_lock_state>(state);

  QObject::connect(s.poll_thread.get (), SIGNAL(timeout_signal ())
                   , this, SLOT(pollfd_timeout ())
                   , Qt::QueuedConnection);
  QObject::connect(s.poll_thread.get ()
                   , SIGNAL(read_packet_signal (boost::shared_array<unsigned char>, unsigned int))
                   , this, SLOT(read_packet (boost::shared_array<unsigned char>, unsigned int))
                   , Qt::QueuedConnection);

  dmx_sct_filter_params filter;
  std::memset(&filter, 0, sizeof(filter));
  filter.pid = 0x11; // SDT/BAT
  filter.timeout = 0;
  filter.flags = DMX_IMMEDIATE_START | DMX_CHECK_CRC;
    
  if(ioctl(demuxfd, DMX_SET_FILTER, &filter) == -1)
  {
    std::cout << "couldn't open another filter" << std::endl;
    Q_EMIT lock_error_signal ();
    return;
  }

  s.poll_thread->start ();

  pollfd pfd;
  pfd.fd = demuxfd;
  pfd.events = POLLIN;
  pfd.revents = 0;
  s.poll_thread->add_pollfd(pfd);
  s.poll_thread->start_poll ();
}

void reception_thread::pollfd_timeout ()
{
  std::cout << "pollfd timeout" << std::endl;
  Q_EMIT lock_error_signal ();
}

reception_thread::frequency_lock_state::frequency_lock_state (unsigned int frequency)
  : frequency(frequency)
{
  poll_thread.reset(new qt::poll_thread, thread_destroy ());
}

void reception_thread::frequency_lock_state::thread_destroy::operator ()(QThread* thread) const
{
  thread->exit ();
  thread->wait ();
  delete thread;
}

reception_thread::frequency_lock_state::~frequency_lock_state()
{
}

poll_thread::poll_thread ()
  : idle_timer(0)
{
  moveToThread(this);

  QObject::connect(this, SIGNAL(add_pollfd_signal(pollfd)), this, SLOT(add_pollfd_slot(pollfd))
                   , Qt::BlockingQueuedConnection);
  QObject::connect(this, SIGNAL(pop_front_signal()), this, SLOT(pop_front_slot())
                   , Qt::BlockingQueuedConnection);
  QObject::connect(this, SIGNAL(start_timer_signal()), this, SLOT(start_timer())
                   , Qt::QueuedConnection);
}

poll_thread::~poll_thread()
{
  for(std::vector<pollfd>::iterator first = pollfds.begin()
        , last = pollfds.end(); first != last; ++first)
  {
    ::close(first->fd);
  }
}

void poll_thread::start_poll ()
{
  assert(idle_timer != 0);
  Q_EMIT start_timer_signal ();
}

void poll_thread::start_timer ()
{
  assert(idle_timer != 0);
  idle_timer->start(0);
}

void poll_thread::run ()
{
  assert(idle_timer == 0);
  idle_timer = new QTimer(this);
  connect(idle_timer, SIGNAL (timeout ()), this, SLOT(run_poll ())
          , Qt::QueuedConnection);

  QThread::exec ();
}

void poll_thread::run_poll ()
{
  std::cout << "run_poll " << pollfds.size () << std::endl;
  assert(!pollfds.empty ());
  int n = poll(&pollfds[0], pollfds.size (), 100);
  if(n == -1)
  {
    std::cout << "poll failed" << std::endl;
    Q_EMIT timeout_signal ();
    return;
  }

  if(n == 0) 
  {
    std::cout << "poll timeout" << std::endl;
    for(std::vector<unsigned int>::iterator first = retries.begin (), last = retries.end ()
          ;first != last; ++first)
      if(++*first == 100)
      {
        QThread::exit ();
        std::cout << "should emit that one FD timed out" << std::endl;
        Q_EMIT timeout_signal ();
        return;
      }
  }  
  else
  {
    std::cout << "poll executed fine " << n << std::endl;
    for(int first_index = 0, last_index = pollfds.size ()
          ; first_index != last_index;++first_index)
    {
      boost::shared_array<unsigned char> buffer(new unsigned char[1024]);
      int count = read(pollfds[first_index].fd, buffer.get (), 1024);
      if(count < 0 && errno == EOVERFLOW)
      {
        count = read(pollfds[first_index].fd, buffer.get (), 1024);
        if(count < 0)
        {
          std::cout << "Failed reading section, errno: " << errno << std::endl;
          Q_EMIT timeout_signal ();
          return;
        }
      }
      else if(count < 0 && (errno == EAGAIN))
        continue;
      else if(count < 0)
      {
        std::cout << "some unknown error, errno: " << errno << std::endl;
        Q_EMIT timeout_signal ();
        return;
      }

      retries[first_index] = 0;
      std::cout << "read something" << std::endl;
      Q_EMIT read_packet_signal(buffer, count);
    }
  }
}

reception_thread::~reception_thread()
{
  std::cout << "reception_thread::~reception_thread" << std::endl;
}

void reception_thread::read_packet(boost::shared_array<unsigned char> buffer, unsigned int size)
{
  std::cout << "read_packet " << size << std::endl;
  assert(!!boost::get<frequency_lock_state>(&state)
         || !!boost::get<ready_state>(&state));
  if(boost::get<frequency_lock_state>(&state))
  {
  frequency_lock_state& s = boost::get<frequency_lock_state>(state);

  if(size >= 4)
  {
    typedef gts::sections::extended_section<unsigned char*> section_type;
          
    section_type section(buffer.get (), buffer.get () + size);

    typedef section_type::table_id_iterator table_id_iterator;
    table_id_iterator table_id = section.begin ();
    std::cout << "table_id: 0x" << std::hex << *table_id << std::dec << std::endl;
    if(*table_id == 0x42) // SDT
    {
      std::cout << "SDT" << std::endl;
      if(parse_sdt(buffer.get(), size))
      {
        Q_EMIT lock_end_signal ();
      }
      else
        Q_EMIT lock_error_signal ();
    }
  }    
  }
  else
  {
    std::cout << "Already in ready state" << std::endl;
  }
}

void reception_thread::run ()
{
  QThread::exec ();
}

void reception::scan ()
{
  channels.clear();
  channels_list->clear();
  progress_bar->setValue(0);
  progress_bar->setVisible(true);
  scan_button->setEnabled (false);
  current_frequency_index = 0;
  scan_in_progress = true;
  select_button->setEnabled(false);
  stop_scan_button->setEnabled(true);

  std::cout << "reception_thread::run" << std::endl;
  std::string frontend_path = "/dev/dvb/adapter" + adapter_edit->text ().toStdString ()
    + "/frontend" + frontend_edit->text ().toStdString ();
  std::cout << "opening " << frontend_path << std::endl;
  frontendfd = open(frontend_path.c_str (), O_RDONLY|O_RDWR);

  start_scan ();
}

void reception::stop_scan()
{
  if(scan_in_progress)
  {
    should_stop_scan = true;
    stop_scan_button->setEnabled(false);
  }
}

void reception::start_scan ()
{
  scan_thread = new reception_thread(frontendfd
                                     , adapter_edit->text ().toStdString ()
                                     , frontend_edit->text ().toStdString ());
  scan_thread->start ();
  QObject::connect(this, SIGNAL(lock_frequency (unsigned int)), scan_thread
                  , SLOT(lock_frequency(unsigned int)), Qt::QueuedConnection);
  QObject::connect(scan_thread, SIGNAL(lock_error_signal()), this
                  , SLOT(lock_error()), Qt::QueuedConnection);
  QObject::connect(scan_thread, SIGNAL(network_found(std::string, unsigned int)), this
                   , SLOT(network_found(std::string, unsigned int)), Qt::QueuedConnection);
  QObject::connect(scan_thread, SIGNAL(lock_end_signal()), this
                   , SLOT(lock_end()), Qt::QueuedConnection);
  Q_EMIT lock_frequency(frequencies[current_frequency_index]);
}

void reception::network_found(std::string name, unsigned int service_identifier)
{
  channels_list->insertItem(channels_list->count (), QString::fromStdString(name));
  std::cout << "lock found network " << name << " on frequency "
            << frequencies[current_frequency_index]
            << ", trying next " << current_frequency_index << std::endl;

  channel c = {name, frequencies[current_frequency_index], service_identifier};
  channels.push_back(c);
  
}

void reception::lock_end ()
{
  progress_bar->setValue(progress_bar->value () + 1);

  assert(scan_thread != 0);
  scan_thread->exit ();
  scan_thread->wait ();
  delete scan_thread;
  scan_thread = 0;

  if(current_frequency_index+1 != sizeof(frequencies)/sizeof(frequencies[0])
     && !should_stop_scan)
  {
    ++current_frequency_index;
    start_scan ();
  }
  else
  {
    scan_finished();
  }
}

void reception::scan_finished()
{
  std::cout << "scan finished" << std::endl;
  scan_in_progress = false;
  should_stop_scan = false;
  stop_scan_button->setEnabled(false);
  scan_button->setEnabled(true);
  if(channels_list->selectedItems().size())
  {
    select_button->setEnabled(true);
  }
  ::close(frontendfd);
  frontendfd = 0;
}

void reception::channels_list_selection_changed(QListWidgetItem* current, QListWidgetItem*)
{
  if(!scan_in_progress)
  {
    select_button->setEnabled(true);
  }
}

void reception::lock_error ()
{
  std::cout << "lock_error, trying next " << current_frequency_index << std::endl;
  progress_bar->setValue(progress_bar->value () + 1);

  assert(scan_thread != 0);
  scan_thread->exit ();
  scan_thread->wait ();
  delete scan_thread;
  scan_thread = 0;

  if(current_frequency_index != sizeof(frequencies)/sizeof(frequencies[0])
     && !should_stop_scan)
  {
    ++current_frequency_index;
    start_scan ();
  }
  else
  {
    scan_finished();
  }
}

void reception::select ()
{
  assert(channels_list->count() == channels.size());
  QList<QListWidgetItem*> selected_items = channels_list->selectedItems();
  if(selected_items.size() == 1)
  {
    std::size_t row = channels_list->row(*selected_items.begin());
    Q_EMIT selected_channel(channels[row].name, channels[row].frequency
                            , channels[row].service_identifier);
  }  
}

bool reception_thread::parse_sdt(unsigned char const* buf, int size)
{
  typedef gts::sections::extended_section<unsigned char const*> extended_section_type;

  typedef extended_section_type::section_syntax_indicator_iterator section_syntax_indicator_iterator;
  typedef extended_section_type::private_indicator_iterator private_indicator_iterator;
  typedef extended_section_type::private_section_length_iterator private_section_length_iterator;
  typedef extended_section_type::table_id_extension_iterator table_id_extension_iterator;
  typedef extended_section_type::version_number_iterator version_number_iterator;
  typedef extended_section_type::current_next_indicator_iterator current_next_indicator_iterator;
  typedef extended_section_type::section_number_iterator section_number_iterator;
  typedef extended_section_type::last_section_number_iterator last_section_number_iterator;
  typedef extended_section_type::private_data_iterator private_data_iterator;
  typedef extended_section_type::crc32_iterator crc32_iterator;
  typedef extended_section_type::end_iterator end_iterator;
  
  extended_section_type extended_section(buf, buf + size);
  std::cout << "parsing SDT" << std::endl;

  section_syntax_indicator_iterator
    section_syntax_indicator = gts::iterators::next(extended_section.begin ());
  bool r = false;

  if(*section_syntax_indicator == 1) // section syntax indicator
  {
    std::cout << "section syntax indicator == 1" << std::endl;
    private_indicator_iterator private_indicator = gts::iterators::next(section_syntax_indicator);
    //if(*private_indicator == 1)
    {
      std::cout << "private indicator == 1" << std::endl;
      private_section_length_iterator private_section_length = gts::iterators::next(private_indicator);
      table_id_extension_iterator table_id_extension = gts::iterators::next(private_section_length);
      version_number_iterator version_number = gts::iterators::next(table_id_extension);
      current_next_indicator_iterator current_next_indicator
        = gts::iterators::next(version_number);
      section_number_iterator section_number = gts::iterators::next(current_next_indicator);
      last_section_number_iterator last_section_number
        = gts::iterators::next(section_number);
      private_data_iterator private_data_it = gts::iterators::next(last_section_number);
      /**/
      boost::iterator_range<unsigned char const*> private_data = *private_data_it;
      std::cout << "size of private_data: " << boost::distance(private_data) << std::endl;

      typedef gts::sdt_private<unsigned char const*> sdt_private_type;
      sdt_private_type sdt_private(boost::begin(private_data), boost::end(private_data));

      typedef sdt_private_type::original_network_identifier_iterator original_network_identifier_iterator;
      typedef sdt_private_type::services_iterator services_iterator;

      original_network_identifier_iterator original_network_id = sdt_private.begin ();

      services_iterator services_it = gts::iterators::next(original_network_id);
      typedef services_iterator::deref_type service_range;
      typedef boost::range_iterator<service_range>::type service_iterator;
      typedef boost::range_value<service_range>::type service_type;

      service_range services = *services_it;
      for(service_iterator first = boost::begin(services), last = boost::end(services)
            ;first != last; ++first)
      {
        std::cout << "service" << std::endl;
        typedef service_type::service_identifier_iterator service_identifier_iterator;
        typedef service_type::eit_schedule_flag_iterator eit_schedule_flag_iterator;
        typedef service_type::eit_present_following_flag_iterator eit_present_following_flag_iterator;
        typedef service_type::running_status_iterator running_status_iterator;
        typedef service_type::free_ca_mode_iterator free_ca_mode_iterator;
        typedef service_type::descriptors_loop_length_iterator descriptors_loop_length_iterator;
        typedef service_type::descriptors_iterator descriptors_iterator;

        service_identifier_iterator service_identifier_it = first->begin();
        
        descriptors_iterator descriptors_it = gts::iterators::next<6>(first->begin());
        typedef descriptors_iterator::deref_type descriptor_range;
        descriptor_range descriptors = *descriptors_it;
        typedef boost::range_iterator<descriptor_range>::type descriptor_iterator;
        for(descriptor_iterator first = boost::begin(descriptors)
              , last = boost::end(descriptors); first != last; ++first)
        {
          typedef boost::range_value<descriptor_range>::type descriptor_type;
          typedef descriptor_type::tag_iterator descriptor_tag_iterator;
          
          descriptor_tag_iterator tag_it = first->begin();
          if(*tag_it == 0x48)
          {
            std::cout << "Service descriptor" << std::endl;
            typedef gts::descriptors::service_descriptor<unsigned char const*> service_descriptor;
            service_descriptor service(*first);
            typedef service_descriptor::service_type_iterator service_type_iterator;
            typedef service_descriptor::service_provider_name_length_iterator
              service_provider_name_length_iterator;
            typedef service_descriptor::service_provider_name_iterator
              service_provider_name_iterator;
            typedef service_descriptor::service_name_length_iterator service_name_length_iterator;
            typedef service_descriptor::service_name_iterator service_name_iterator;

            service_provider_name_iterator service_provider_name_it
              = gts::iterators::next<3>(service.begin());
            std::cout << "provider name: " << *service_provider_name_it << std::endl;
            service_name_iterator service_name_it
              = gts::iterators::next<2>(service_provider_name_it);
            std::cout << "service name: " << *service_name_it << std::endl;
            std::string service_name;
            typedef service_name_iterator::deref_type service_name_range;
            service_name_range service_name_ = *service_name_it;
            for(boost::range_iterator<service_name_range>::type first = boost::begin(service_name_)
                  , last = boost::end(service_name_); first != last; ++first)
            {
              char c = *first;
              if(boost::algorithm::is_print ()(c))
                service_name.push_back(c);
            }
            Q_EMIT network_found(service_name, *service_identifier_it);
            break;
          }
        }
      }
    }
  }
  return r;
}

} }
