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

#include <ghtv/qt/main_window.hpp>
#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/resolution_action.hpp>
#include <ghtv/qt/ncl/debug/debug_media_window.hpp>
#ifndef GHTV_DISABLE_JAVA
#include <ghtv/qt/java/java_widget.hpp>
#endif

#include <gts/dsmcc/biop/message_header.hpp>
#include <gts/dsmcc/biop/message_subheader.hpp>
#include <gts/dsmcc/biop/directory_message.hpp>
#include <gts/dsmcc/biop/file_message.hpp>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/shared_array.hpp>
#include <boost/bind.hpp>
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <QMenuBar>
#include <QLabel>
#include <QFileDialog>
#include <QDir>
#include <QPushButton>

#include <string>

#ifdef __linux
#include <poll.h>
#endif

namespace ghtv { namespace qt {

main_window::main_window(unsigned int width, unsigned int height, QWidget *parent)
  : QMainWindow(parent), width_(width), height_(height), file_menu(0), debug_menu(0)
  , display_menu(0), ncl(0)
#ifndef GHTV_DISABLE_JAVA
  , java(0)
#endif
  , debug_media_window_(0)
  , live_media_info_(0), errors_window(0), initialized(false)
  , error_notification(0), testsuite_window(0)
#ifdef __linux
  , reception_window(0)
#endif
{
#ifdef __linux
  qRegisterMetaType< ::pollfd>("pollfd");
#endif
  qRegisterMetaType< ::boost::shared_array<unsigned char> >("boost::shared_array<unsigned char>");
  qRegisterMetaType<std::string>("std::string");

  file_menu = new QMenu(tr("&File"));
  file_menu->addAction(tr("Open &NCL"), this, SLOT(menu_open_ncl()));
  file_menu->addAction(tr("Open &Java"), this, SLOT(menu_open_java()));
#ifdef __linux
  file_menu->addAction(tr("Open &Reception"), this, SLOT(open_reception()));
#endif
  file_menu->addAction(tr("&Close"), this, SLOT(close_file()));
  menuBar()->addMenu(file_menu);

  debug_menu = new QMenu(tr("&Debug"));
  debug_menu->addAction(tr("&Medias"), this, SLOT(debug_medias()));
  debug_menu->addAction(tr("&Errors"), this, SLOT(debug_errors()));
  debug_menu->setEnabled(false);

  menuBar()->addMenu(debug_menu);

  display_menu = new QMenu(tr("D&isplay"));
  resolutions.push_back(new ghtv::qt::resolution_action(640, 480, display_menu));
  resolutions.back()->setCheckable(true);
  QObject::connect(resolutions.back(), SIGNAL(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*))
                   , this, SLOT(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*)));
  display_menu->addAction(resolutions.back());
  resolutions.push_back(new ghtv::qt::resolution_action(854, 480, display_menu));
  resolutions.back()->setCheckable(true);
  QObject::connect(resolutions.back(), SIGNAL(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*))
                   , this, SLOT(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*)));
  display_menu->addAction(resolutions.back());
  resolutions.push_back(new ghtv::qt::resolution_action(1024, 600, display_menu));
  resolutions.back()->setCheckable(true);
  QObject::connect(resolutions.back(), SIGNAL(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*))
                   , this, SLOT(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*)));
  display_menu->addAction(resolutions.back());
  resolutions.push_back(new ghtv::qt::resolution_action(1024, 768, display_menu));
  resolutions.back()->setCheckable(true);
  QObject::connect(resolutions.back(), SIGNAL(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*))
                   , this, SLOT(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*)));
  display_menu->addAction(resolutions.back());
  resolutions.push_back(new ghtv::qt::resolution_action(1280, 720, display_menu));
  resolutions.back()->setCheckable(true);
  QObject::connect(resolutions.back(), SIGNAL(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*))
                   , this, SLOT(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*)));
  display_menu->addAction(resolutions.back());
  resolutions.push_back(new ghtv::qt::resolution_action(1366, 768, display_menu));
  resolutions.back()->setCheckable(true);
  QObject::connect(resolutions.back(), SIGNAL(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*))
                   , this, SLOT(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*)));
  display_menu->addAction(resolutions.back());
  resolutions.push_back(new ghtv::qt::resolution_action(1920, 1080, display_menu));
  resolutions.back()->setCheckable(true);
  QObject::connect(resolutions.back(), SIGNAL(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*))
                   , this, SLOT(change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*)));
  display_menu->addAction(resolutions.back());

  menuBar()->addMenu(display_menu);

  QMenu* tests = new QMenu(tr("&Tests"));
  
  tests->addAction(tr("Test Suite mode"), this, SLOT(start_testsuite_mode()));
  menuBar()->addMenu(tests);

}

void main_window::change_resolution(std::size_t width, std::size_t height, ghtv::qt::resolution_action* action)
{
  for(std::vector<ghtv::qt::resolution_action*>::iterator first = resolutions.begin()
        , last = resolutions.end(); first != last; ++first)
    if(*first != action)
      (*first)->setChecked(false);

  showNormal ();
  resize(width, height + menuBar()->height());
}

#ifndef GHTV_DISABLE_JAVA
void main_window::open_early_ncl(std::string ncl_filename)
{
  close_file();

  filename = ncl_filename.c_str();
  if(initialized)
    open_queued_ncl();
  else
  {
    QObject::connect(this, SIGNAL(initialized_signal()), this, SLOT(open_queued_ncl()));
  }
}

void main_window::menu_open_java()
{
  
}

void main_window::open_early_java(std::string java_class, std::string classpath, std::string java_root)
{
  close_file();

  this->java_class = QString::fromStdString(java_class);
  this->classpath = QString::fromStdString(classpath);
  this->java_root = QString::fromStdString(java_root);
  if(initialized)
    open_queued_java();
  else
  {
    QObject::connect(this, SIGNAL(initialized_signal()), this, SLOT(open_queued_java()));
  }
}

void main_window::open_queued_java()
{
  assert(initialized);

  boost::optional<QDir> ghtv_jar_dir;
  if(ghtv_jar)
    ghtv_jar_dir = QDir(QString::fromStdString(*ghtv_jar));

  debug_menu->setEnabled(true);
  java = new qt::java::java_widget(this, QString::fromStdString(jre_path)
                                   , java_class, classpath, java_root
                                   , ghtv_jar_dir);
  setCentralWidget(java);

  QObject::connect(java, SIGNAL(error_occurred_signal(std::string))
                   , this, SLOT(error_occurred(std::string)));

  java->show();
  java->raise();
  java->start();
}
#endif
void main_window::open_queued_ncl()
{
  try
  {
    assert(initialized);
    QFileInfo fileinfo(filename);
    QUrl url;
    if(fileinfo.exists())
    {
      url = QUrl::fromLocalFile(fileinfo.canonicalFilePath());
    }
    else
    {
      url = QUrl(filename);
      if(url.isRelative())
      {
        QString path = QDir::currentPath();
        path += '/';
        path += filename;
        url = QUrl::fromLocalFile(path);
        assert(url.scheme() != "" && !url.isRelative());
      }
    }
    debug_menu->setEnabled(true);
    ncl = new ghtv::qt::ncl::ncl_widget;
    setCentralWidget(ncl);

    QObject::connect(ncl, SIGNAL(media_clicked_signal(std::string, std::string))
                     , this, SLOT(media_clicked(std::string, std::string)));
    QObject::connect(ncl, SIGNAL(error_occurred_signal(std::string))
                     , this, SLOT(error_occurred(std::string)));
    ncl->start_ncl(url);
  }
  catch(std::exception& e)
  {
    error_occurred(boost::diagnostic_information(e));
  }
}

void main_window::debug_errors()
{
  delete errors_window;
  delete error_notification;
  error_notification = 0;
  errors_window = new ncl::debug::errors(this);
  QObject::connect(errors_window, SIGNAL(destroying_window_signal())
                   , this, SLOT(destroying_errors_window()));
  errors_window->show();
}

void main_window::error_occurred(std::string error)
{
  if(error_messages.empty())
  {
#ifndef GHTV_DISABLE_JAVA
    assert(ncl || java);
    QWidget* error_parent = ncl?static_cast<QWidget*>(ncl)
      :static_cast<QWidget*>(java);
#else
    assert(ncl);
    QWidget* error_parent = static_cast<QWidget*>(ncl);
#endif
    assert(error_notification == 0);
    error_notification = new QLabel("This document has errors", error_parent);
    error_notification->move(QPoint(0, 0));
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor("red"));
    error_notification->setPalette(palette);
    error_notification->show();
  }
  error_messages.push_back(error);
}

void main_window::debug_medias()
{
  debug_media_window_ = new ncl::debug::debug_media_window(ncl, this);
  QObject::connect(debug_media_window_, SIGNAL(destroying_window_signal())
                   , this, SLOT(destroying_debug_media_window()));
  debug_media_window_->show();
}

void main_window::destroying_reception_window()
{
  reception_window = 0;
}

void main_window::destroying_dsmcc_download_window()
{
  dsmcc_download_window = 0;
}

void main_window::destroying_debug_media_window()
{
  debug_media_window_ = 0;
}

void main_window::destroying_errors_window()
{
  errors_window = 0;
}

void main_window::close_file()
{
  delete error_notification;
  error_notification = 0;
  delete debug_media_window_;
  debug_media_window_ = 0;
  delete errors_window;
  errors_window = 0;
  debug_menu->setEnabled(false);
  if(ncl)
  {
    ncl->stop_ncl();
    delete ncl;
    ncl = 0;
  }
  error_messages.clear();
#ifndef GHTV_DISABLE_JAVA
  delete java;
  java = 0;
#endif
}

#ifdef __linux
void main_window::open_reception ()
{
  assert(reception_window == 0);
  reception_window = new ghtv::qt::reception;
  QObject::connect(reception_window, SIGNAL(destroying_window_signal())
                   , this, SLOT(destroying_reception_window()));
  QObject::connect(reception_window, SIGNAL(selected_channel(std::string, unsigned int, unsigned int))
                   , this, SLOT(open_channel(std::string, unsigned int, unsigned int))
                   , Qt::QueuedConnection);
  reception_window->show ();
}

void main_window::open_channel(std::string name, unsigned int frequency, unsigned int service_identifier)
{
  delete reception_window;
  reception_window = 0;

  std::cout << "Should download DSMCC from " << name << std::endl;
  dsmcc_download_window = new ghtv::qt::dsmcc_download(name, frequency, service_identifier);
  QObject::connect(dsmcc_download_window, SIGNAL(destroying_window_signal())
                   , this, SLOT(destroying_dsmcc_download_window()));
  QObject::connect(dsmcc_download_window, SIGNAL(download_success())
                   , this, SLOT(open_dsmcc_app()));

  dsmcc_download_window->show();
  dsmcc_download_window->start();
}

typedef gts::dsmcc::biop::directory_message<std::vector<unsigned char>::const_iterator> 
  directory_message;
typedef gts::dsmcc::biop::file_message<std::vector<unsigned char>::const_iterator>
  file_message;
typedef gts::dsmcc::biop::bind<std::vector<unsigned char>::const_iterator> bind;

struct bind_info
{
  unsigned int module_id;
  boost::iterator_range<std::vector<unsigned char>::const_iterator> object_key;
};

struct directory_info
{
  directory_message directory;
  qt::bind bind;
  bind_info info;
};

inline bool operator<(bind_info const& lhs, bind_info const& rhs)
{
  return (lhs.module_id == rhs.module_id && lhs.object_key < rhs.object_key)
    || lhs.module_id < rhs.module_id;
}

void main_window::open_dsmcc_app()
{
  std::cout << "open_dsmcc_app" << std::endl;
  assert(dsmcc_download_window != 0);
  assert(dsmcc_download_window->download_thread_ != 0);

  // unsigned int service_gateway_module_id = dsmcc_download_window->download_thread_
  //   ->service_gateway_module_id;
  std::map<unsigned int, boost::shared_ptr<ghtv::qt::module> > modules
    = dsmcc_download_window->download_thread_->modules;
  std::string base_directory = dsmcc_download_window->base_directory
    , classpath = dsmcc_download_window->classpath, initial_class
    = dsmcc_download_window->initial_class;

  delete dsmcc_download_window;
  dsmcc_download_window = 0;

  //typedef gts::dsmcc::biop::iop_ior<std::vector<unsigned char>::const_iterator> iop_ior;
  typedef boost::variant<directory_message, file_message> message_variant;
  std::multimap<bind_info, directory_info> binding_iors;
  std::map<bind_info, message_variant> indexed_messages;
  directory_message service_gateway;

  for(std::map<unsigned int, boost::shared_ptr<ghtv::qt::module> >::const_iterator
        module_it = modules.begin(), last = modules.end(); module_it != last; ++module_it)
  {
    {
    std::vector<unsigned char>::const_iterator first = module_it->second->content.begin()
      , last = module_it->second->content.end();
    while(first != last)
    {
      typedef gts::dsmcc::biop::message_header<std::vector<unsigned char>::const_iterator>
        message_header_type;
      typedef message_header_type::magic_iterator magic_iterator;
      typedef message_header_type::biop_version_iterator biop_version_iterator;
      typedef message_header_type::byte_order_iterator byte_order_iterator;
      typedef message_header_type::message_type_iterator message_type_iterator;
      typedef message_header_type::message_size_iterator message_size_iterator;

      message_header_type message_header(first, last);
      magic_iterator::deref_type magic = *message_header.begin();
      if(magic == 0x42494F50)
      {
        typedef gts::dsmcc::biop::message_subheader<std::vector<unsigned char>::const_iterator>
          message_subheader_type;
        typedef message_subheader_type::object_key_iterator object_key_iterator;
        typedef message_subheader_type::object_kind_iterator object_kind_iterator;
        typedef message_subheader_type::object_info_iterator object_info_iterator;
        typedef message_subheader_type::service_context_list_iterator service_context_list_iterator;
        typedef message_subheader_type::end_iterator subheader_end_iterator;
      
        message_size_iterator message_size_it = gts::iterators::next<4>(message_header.begin());
        std::size_t message_size = *message_size_it;
        std::cout << "Message size: " << message_size << std::endl;
        first = boost::next(first, (std::min<std::size_t>)(message_size
                                                           + std::distance(first, (++message_size_it).base())
                                                           , std::distance(first, last)));
        std::vector<unsigned char>::const_iterator subheader_iterator
          = gts::iterators::next<5>(message_header.begin()).base();
        message_subheader_type message_subheader(subheader_iterator
                                                 , first);//module_it->second->content.end());
        object_key_iterator object_key_it = message_subheader.begin();
        object_kind_iterator object_kind_it = gts::iterators::next(object_key_it);
        object_info_iterator object_info_it = gts::iterators::next(object_kind_it);

        bind_info message_info = {module_it->second->module_id, *object_key_it};

        object_kind_iterator::deref_type kind = *object_kind_it;
        if((boost::distance(kind) == 4 && std::equal(kind.begin(), kind.end(), "srg"))
           || (boost::distance(kind) == 4 && std::equal(kind.begin(), kind.end(), "dir")))
        {
          std::cout << "Service gateway kind" << std::endl;
          subheader_end_iterator message_subheader_end 
            = gts::iterators::next<2>(object_info_it);

          typedef directory_message::message_body_length_iterator message_body_length_iterator;
          typedef directory_message::binds_iterator binds_iterator;
          directory_message directory(message_subheader_end.base(), first);

          if(boost::distance(kind) == 4 && std::equal(kind.begin(), kind.end(), "srg"))
            service_gateway = directory;

          typedef binds_iterator::deref_type bind_range;
          bind_range binds = *++directory.begin();
          typedef boost::range_iterator<bind_range>::type bind_iterator;
          std::cout << "binds " << boost::distance(binds) << std::endl;
          for(bind_iterator bfirst = boost::begin(binds), blast = boost::end(binds)
                ;bfirst != blast; ++bfirst)
          {
            std::cout << "Bind" << std::endl;
            assert(!bfirst->ior.biop_profiles.empty());
            bind_info info = 
              {bfirst->ior.biop_profiles[0].object_location.module_id
               , bfirst->ior.biop_profiles[0].object_location.object_key};
            directory_info di = {directory, *bfirst, message_info};
            binding_iors.insert(std::make_pair(info, di));

            std::cout << bfirst->ior.biop_profiles[0].object_location.module_id
                      << std::endl;
          }
          indexed_messages.insert(std::make_pair(message_info, directory));
        }      
        else if(boost::distance(kind) == 4 && std::equal(kind.begin(), kind.end(), "fil"))
        {
          file_message file(object_info_it.base(), first);

          indexed_messages.insert(std::make_pair(message_info, file));
        }
        else
        {
          std::cout << "Unknown kind" << std::endl;
        }
      }
      else
      {
        std::cout << "Wrong magic number" << std::endl;
        break;
      }
    }
    }
  }
  // else
  // {
  //   std::cout << "Not found service_gateway module" << std::endl;
  // }

  boost::filesystem::path temporary_directory;
#if BOOST_VERSION >= 104600
  do
  {
    temporary_directory = boost::filesystem::temp_directory_path()
      / boost::filesystem::unique_path();
  } while(!boost::filesystem::create_directory(temporary_directory));
#else
  {
    char template_[] = "temporaryXXXXXX";
    char* temp = mkdtemp(template_);
    temporary_directory = temp;
  }
#endif

  for(std::map<bind_info, message_variant>::const_iterator
        first = indexed_messages.begin(), last = indexed_messages.end()
        ;first != last; ++first)
  {
    if(file_message const* p = boost::get<file_message>(&first->second))
    {
      std::vector<std::string> path;
      std::multimap<bind_info, directory_info>::const_iterator
        parent = binding_iors.find(first->first);
      while(parent != binding_iors.end())
      {
        std::string name(parent->second.bind.names[0].id.begin()
                         , parent->second.bind.names[0].id.end());
        if(!name.empty() && name[name.size()-1] == 0)
          name.erase(boost::prior(name.end()));
        path.push_back(name);
        parent = binding_iors.find(parent->second.info);
      }
    
      boost::filesystem::path file_path = temporary_directory;
      for(std::vector<std::string>::const_reverse_iterator first = path.rbegin()
            , last = path.rend(); first != last; ++first)
      {
        file_path /= *first;
        if(boost::next(first) != last
           && !boost::filesystem::exists(file_path))
        {
          std::cout << "Creating directory " << file_path << std::endl;
          boost::filesystem::create_directory(file_path);
        }
      }
      typedef file_message::file_message_body_iterator file_message_body_iterator;
      typedef file_message_body_iterator::deref_type body_type;
      body_type body = *++ ++p->begin();
      typedef body_type::content_iterator content_iterator;
      typedef content_iterator::deref_type content_type;
      content_type content = *++body.begin();
      std::cout << "Saving " << file_path << std::endl;
      boost::filesystem::ofstream file(file_path);
      std::copy(boost::begin(content), boost::end(content)
                , std::ostream_iterator<unsigned char>(file));
    }
  }

  std::cout << "Temp directory: " << temporary_directory << std::endl;

  std::cout << "base_directory: " << base_directory
            << " Should start: " << initial_class << std::endl;

  filename = QString::fromStdString((temporary_directory / initial_class).string());
  std::cout << "Starting " << filename.toStdString() << std::endl;
  assert(initialized);
  ncl->stop_ncl();
  error_messages.clear();
  open_queued_ncl();
}
#endif

void main_window::menu_open_ncl()
{
  setFocus(Qt::OtherFocusReason);
  filename = QFileDialog::getOpenFileName( this, tr("Open NCL Document")
                                           , QDir::currentPath()

                                           , tr("NCL files (*.ncl);;All Files (*.*)")
                                           , 0, 0);
  if(filename.size())
  {
    assert(initialized);
    if(ncl)
      ncl->stop_ncl();
    error_messages.clear();
    open_queued_ncl();
  }
}

void main_window::media_clicked(std::string media_identifier, std::string context_identifier)
{
  delete live_media_info_;
  live_media_info_ = 0;
  live_media_info_ = new ncl::debug::live_media_info(media_identifier, context_identifier, ncl);
  live_media_info_->show();
}

main_window::~main_window()
{
}

void main_window::keyPressEvent(QKeyEvent* event)
{
  if((!ncl || !ncl->process_key(event, true))
#ifndef GHTV_DISABLE_JAVA
     && (!java || !java->process_key(event, true))
#endif
     )
    QMainWindow::keyPressEvent(event);
}

void main_window::keyReleaseEvent(QKeyEvent* event)
{
  if((!ncl || !ncl->process_key(event, false))
#ifndef GHTV_DISABLE_JAVA
     && (!java || !java->process_key(event, false))
#endif
     )
    QMainWindow::keyReleaseEvent(event);
}

void main_window::paintEvent(QPaintEvent* event)
{
  if(!initialized)
  {
    if(width_ && height_)
      resize(QSize(width_, height_));
    else if(width_)
      resize(QSize(width_, size().height()));
    else if(height_)
      resize(QSize(size().width(), height_));

    width_ = height_ = 0;

    initialized = true;
    Q_EMIT initialized_signal();
  }
  QWidget::paintEvent(event);
}

void main_window::start_testsuite_mode()
{
  if(!testsuite_window)
  {
    close_file();

    testsuite_window = new ncl::testsuite::testsuite_window;

    file_menu->setEnabled(false);
    display_menu->setEnabled(false);

    QObject::connect(testsuite_window, SIGNAL(destroy_window()), this, SLOT(stop_testsuite_mode()));
    QObject::connect(testsuite_window, SIGNAL(run_test(QString)), this, SLOT(run_test(QString)));
    QObject::connect(testsuite_window, SIGNAL(stop_test()), this, SLOT(stop_test()));
    testsuite_window->show();
    testsuite_window->updateGeometry();
  }
}

void main_window::run_test(QString test)
{
  close_file();
  filename = test;
  error_messages.clear();
  open_queued_ncl();
}

void main_window::stop_test()
{
  close_file();
  error_messages.clear();
}

void main_window::stop_testsuite_mode()
{
  testsuite_window = 0;
  file_menu->setEnabled(true);
  display_menu->setEnabled(true);
  close_file();
}

} }
