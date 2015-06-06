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

#include "ghtv/qt/dsmcc_download.hpp"
#include "ghtv/qt/dsmcc.hpp"

#include <gts/constants/pmt/stream_type.hpp>
#include <gts/constants/arib/descriptor_tag.hpp>
#include <gts/constants/etsi/descriptor_tag.hpp>
#include <gts/constants/etsi/protocol_id.hpp>
#include <gts/constants/abnt/application_type.hpp>
#include <gts/constants/abnt/application_control_code.hpp>
#include <gts/sections/program_association_section.hpp>
#include <gts/sections/program_map_section.hpp>
#include <gts/sections/application_information_section.hpp>
#include <gts/sections/extended_section.hpp>
#include <gts/descriptors/application_descriptor.hpp>
#include <gts/descriptors/application_name_descriptor.hpp>
#include <gts/descriptors/transport_protocol_descriptor.hpp>
#include <gts/descriptors/stream_identifier_descriptor.hpp>
#include <gts/descriptors/ginga_application_location_descriptor.hpp>
#include <gts/dsmcc/sections/user_network_message_section.hpp>
#include <gts/dsmcc/descriptors/group_info_indication.hpp>
#include <gts/dsmcc/descriptors/service_gateway_info.hpp>
#include <gts/dsmcc/download_message_id.hpp>
#include <gts/dsmcc/download_server_initiate.hpp>
#include <gts/dsmcc/download_info_indication.hpp>
#include <gts/dsmcc/download_data_block.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/version.hpp>
#include <boost/range.hpp>

#include <sstream>

#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QSizePolicy>
#include <QHeaderView>

#include <linux/dvb/dmx.h>
#include <linux/dvb/frontend.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stropts.h>
#include <errno.h>
#include <poll.h>

#include <iostream>

namespace ghtv { namespace qt {

template <typename Iterator, typename V>
Iterator find_association(Iterator first, Iterator last, V v)
{
  for(;first != last;++first)
  {
    if(first->second == v)
      break;
  }
  return first;
}

namespace dsmcc_download_detail {

download_thread::download_thread(unsigned int pid
                                 , std::vector<std::pair<unsigned int, unsigned int> > associations)
  : pid(pid), dii_pid(0), associations(associations)
  , dsi_resolved(false), dii_resolved(false), transaction_identification(0)
  , service_gateway_module_id(0), download_id(0)
  , blocks_to_download(0)
  , mutex(new QMutex())
  , cancel(false)
{
  moveToThread(this);
}

void download_thread::filter(unsigned int pid, unsigned int table_id)
{
  dmx_sct_filter_params filter;
  std::memset(&filter, 0, sizeof(filter));
  filter.pid = pid;
  filter.timeout = 0;
  filter.flags = DMX_IMMEDIATE_START | DMX_CHECK_CRC;
  filter.filter.filter[0] = table_id;
  filter.filter.mask[0] = 0xFF;
      
  if(ioctl(demuxfd.fd, DMX_SET_FILTER, &filter) == -1)
  {
    std::cout << "couldn't open another filter" << std::endl;
    //Q_EMIT download_failure ();
    return;
  }
}

void download_thread::stop_filter()
{
  if(ioctl(demuxfd.fd, DMX_STOP) == -1)
  {
    std::cout << "couldn't close filter" << std::endl;
    //Q_EMIT download_failure ();
    return;
  }
}

void download_thread::run()
{

  int fd = open("/dev/dvb/adapter0/demux0", O_RDWR|O_NONBLOCK);
  if(fd == -1)
  {
    //Q_EMIT download_failure();
    return;
  }

  demuxfd.fd = fd;

  filter(pid, 0x3b);

  std::cout << "Should download from pid: " << pid << std::endl;
  do
  {
    {
      QMutexLocker lock(mutex.get());
      if(cancel)
        return;
    }

    std::cout << "polling" << std::endl;
    pollfd pfd;
    std::memset(&pfd, 0, sizeof pfd);
    pfd.fd = demuxfd.fd;
    pfd.events = POLLIN;
    int n = poll(&pfd, 1, 50);
    if(n >= 0)
    {
      if(pfd.revents & POLLIN)
      {
        std::cout << "poll succesful" << std::endl;
        int count = 0;
        unsigned char buffer[4096];
        do
        {
          count = ::read(demuxfd.fd, buffer, 4096);
        }
        while (count < 0 && errno == EINTR);
        if(count < 0)
        {
          //Q_EMIT pmt_failure();
          return;
        }

        if(count > 0)
        {
          std::cout << "Read packet of size " << count << std::endl;
          typedef gts::sections::extended_section<unsigned char const*> extended_section_type;
          typedef extended_section_type::table_id_iterator table_id_iterator;
          extended_section_type extended_section(&buffer[0], &buffer[0] + count);
          table_id_iterator table_id = extended_section.begin ();
          std::cout << "UN message" << std::endl;
          typedef gts::dsmcc::sections::user_network_message_section
            <unsigned char const*> section_un_type;
          section_un_type dsmcc_section(extended_section);
          typedef section_un_type::user_network_message_iterator
            user_network_message_iterator;
          user_network_message_iterator user_network_message_it
            = gts::iterators::next<9>(dsmcc_section.begin());
          typedef user_network_message_iterator::deref_type user_network_message;
          user_network_message un = *user_network_message_it;

          if(*table_id == 0x3b)
          {
            typedef user_network_message::message_id_iterator message_id_iterator;
            message_id_iterator message_id_it = gts::iterators::next<2>(un.begin());
            switch(*message_id_it)
            {
            case gts::dsmcc::download_message_id::download_info_indication:
              std::cout << "DII" << std::endl;
              if(dsi_resolved && !dii_resolved)
              {
                typedef gts::dsmcc::download_info_indication
                  <unsigned char const*> download_info_indication;
                download_info_indication dii(un);
                typedef download_info_indication::transaction_id_iterator transaction_id_iterator;
                typedef download_info_indication::download_id_iterator download_id_iterator;
                typedef download_info_indication::block_size_iterator block_size_iterator;
                typedef download_info_indication::module_range_iterator module_range_iterator;
                transaction_id_iterator transaction_id_it
                  = gts::iterators::next<3>(dii.begin());
                std::cout << "transaction_id: " << *transaction_id_it
                          << std::endl;
                std::cout << "Identification: " << ((*transaction_id_it & 0xFFFE) >> 1)
                          << std::endl;
                if(((*transaction_id_it & 0xFFFE) >> 1) == transaction_identification)
                {
                  std::cout << "This is the DII that we're looking for" << std::endl;
                  stop_filter();
                  download_id = *gts::iterators::next<5>(dii.begin());
                  block_size = *gts::iterators::next<6>(dii.begin());
                  module_range_iterator module_range_it
                    = gts::iterators::next<12>(dii.begin());
                  std::cout << "DII has " << boost::distance(*module_range_it)
                            << " modules" << std::endl;
                  typedef module_range_iterator::deref_type module_range;
                  module_range modules = *module_range_it;
                  typedef boost::range_iterator<module_range>::type module_iterator;
                  unsigned int total_size = 0;
                  for(module_iterator first = boost::begin(modules)
                        , last = boost::end(modules); first != last; ++first)
                  {
                    typedef boost::range_value<module_range>::type module_type;
                    typedef module_type::module_id_iterator module_id_iterator;
                    typedef module_type::module_size_iterator module_size_iterator;
                    typedef module_type::module_version_iterator module_version_iterator;
                    module_type module = *first;
                    module_id_iterator module_id_it = module.begin();
                    module_size_iterator module_size_it = ++module_id_it;
                    module_version_iterator module_version_it = ++module_size_it;
                    std::cout << "module_id: " << *module_id_it << std::endl;
                    std::size_t number_of_blocks = *module_size_it/block_size;
                    std::cout << "Module size: " << *module_size_it << std::endl;
                    std::cout << "number_of_blocks: " << number_of_blocks << std::endl;
                    if(number_of_blocks*block_size < *module_size_it)
                      ++number_of_blocks;
                    std::cout << "number_of_blocks: " << number_of_blocks << std::endl;
                    blocks_to_download += number_of_blocks;
                    total_size += *module_size_it;
                    boost::shared_ptr<qt::module> p
                      (new qt::module(*module_id_it, *module_size_it
                                      , *module_version_it
                                      , number_of_blocks));
                    this->modules.insert(std::make_pair(*module_id_it, p));
                  }
                  filter(dii_pid, 0x3c); // DDB
                  dii_resolved = true;
                  Q_EMIT downloading(blocks_to_download, total_size);
                }
              }
              break;
            case gts::dsmcc::download_message_id::download_server_initiate:
              std::cout << "DSI" << std::endl;
              if(!dsi_resolved)
              {
                typedef gts::dsmcc::download_server_initiate<unsigned char const*>
                  download_server_initiate;
                download_server_initiate dsi(un);
                typedef download_server_initiate::private_data_iterator private_data_iterator;
                private_data_iterator private_data_it = gts::iterators::next<7>(dsi.begin());
                private_data_iterator::deref_type private_data = *private_data_it;
                std::cout << "byte: " << (int)*boost::begin(private_data)
                          << std::endl;
                std::cout << "byte: " << (int)*boost::next(boost::begin(private_data), 1)
                          << std::endl;
                std::cout << "byte: " << (int)*boost::next(boost::begin(private_data), 2)
                          << std::endl;
                std::cout << "byte: " << (int)*boost::next(boost::begin(private_data), 3)
                          << std::endl;

                typedef gts::dsmcc::descriptors::service_gateway_info
                  <unsigned char const*> service_gateway_info;
                service_gateway_info sgi(boost::begin(private_data)
                                         , boost::end(private_data));
                typedef service_gateway_info::object_ref_iterator object_ref_iterator;

                object_ref_iterator object_ref_it = sgi.begin();
                object_ref_iterator::deref_type object_ref = *object_ref_it;
                
                std::cout << "biop profiles: " << object_ref.biop_profiles.size() << std::endl;
                if(object_ref.biop_profiles.size())
                {
                  std::cout << "Has biop profile" << std::endl;

                  boost::uint_t<32u>::least transaction_id
                    = object_ref.biop_profiles[0].conn_binder.delivery_para_use_tap.transaction_id;
                  std::cout << "Normal transaction id " << transaction_id << std::endl;
                  std::cout << "Transaction Identification "
                            << ((transaction_id & 0xFFFE) >> 1)
                            << std::endl;
                  std::cout << "assocTag: "
                            << object_ref.biop_profiles[0].conn_binder.delivery_para_use_tap
                    .assoc_tag << std::endl;
                  std::cout << "service gateway to be found in carousel_id "
                            << object_ref.biop_profiles[0].object_location.carousel_id
                            << " in DII with module "
                            << object_ref.biop_profiles[0].object_location.module_id
                            << std::endl;
                  dsi_resolved = true;
                  stop_filter();
                
                  std::vector<std::pair<unsigned int, unsigned int> >::const_iterator
                    current = find_association(associations.begin(), associations.end()
                                               , object_ref.biop_profiles[0].conn_binder
                                               .delivery_para_use_tap.assoc_tag);
                  if(current != associations.end())
                  {
                    std::cout << "PID to be filtered: " << current->first << std::endl;
                    transaction_identification = ((transaction_id & 0xFFFE) >> 1);
                    service_gateway_module_id = object_ref.biop_profiles[0].object_location
                      .module_id;
                    //modules_to_download.push_back(service_gateway_module_id);
                    dii_pid = current->first;
                    filter(current->first, 0x3b); // DII
                    Q_EMIT waiting_dii();
                  }
                  else
                  {
                    std::cout << "Couldn't find any association for given association tag"
                              << std::endl;
                  }
                }
              }
              break;
            }
          }
          else if(dsi_resolved && dii_resolved && *table_id == 0x3c)
          {
            std::cout << "DDB" << std::endl;
            typedef gts::dsmcc::download_data_block<unsigned char const*>
              download_data_block;
            typedef download_data_block::download_id_iterator download_id_iterator;
            typedef download_data_block::module_id_iterator module_id_iterator;
            typedef download_data_block::module_version_iterator module_version_iterator;
            typedef download_data_block::block_number_iterator block_number_iterator;
            typedef download_data_block::block_data_iterator block_data_iterator;
            download_data_block ddb(un);
            download_id_iterator download_id_it = gts::iterators::next<3>(ddb.begin());
            if(*download_id_it == download_id)
            {
              std::cout << "download_id that we want" << std::endl;
              module_id_iterator module_id_it = gts::iterators::next<2>(download_id_it);
              module_version_iterator module_version_it = ++module_id_it;
              block_number_iterator block_number_it = ++module_version_it;
              block_data_iterator block_data_it = ++block_number_it;

              std::map<unsigned int, boost::shared_ptr<module> >::const_iterator
                current = modules.find(*module_id_it);
              if(current != modules.end())
              {
                std::cout << "Module " << *module_id_it << std::endl;
                if(*module_version_it == current->second->module_version)
                {
                  std::cout << "Blocks: " << current->second->blocks_finished.size()
                            << std::endl;
                  std::cout << "Block " << *block_number_it << std::endl;
                  std::size_t block_number = *block_number_it;
                  if(current->second->blocks_finished.size() > block_number
                     && !current->second->blocks_finished[block_number])
                  {
                    std::cout << "Copying block " << block_number << std::endl;
                    current->second->blocks_finished[block_number] = true;
                    std::size_t start = block_size * block_number;
                    if(start < current->second->content.size())
                    {
                      std::size_t block_size_ = (std::min<std::size_t>)
                        (block_size, current->second->content.size()-start);
                      typedef block_data_iterator::deref_type block_data_range;
                      block_data_range block_data = *block_data_it;
                      typedef boost::range_iterator<block_data_range>::type data_iterator;
                      data_iterator first = boost::begin(block_data)
                        , last = boost::end(block_data);

                      std::size_t max_size = std::distance(first, last);
                      if(max_size > block_size_)
                        last = boost::next(first, block_size_);
                      std::cout << "blocksize: " << block_size_ << std::endl;
                      std::copy(first, last, &current->second->content[start]);
                      Q_EMIT block_downloaded(block_size_);
                    }
                    else
                      Q_EMIT block_downloaded(0);
                    
                    if(current->second->finished())
                    {
                      std::cout << "Finished download for module " << *module_id_it
                                << std::endl;
                      //modules_downloaded->insert(current->second->module_id);
                      if(modules_finished())
                      {
                        std::cout << "All modules have finished downloading" << std::endl;
                        Q_EMIT downloaded();
                        stop_filter();
                      }
                    }
                  }
                  else
                  {
                    std::cout << "Apparently already copied " << block_number << std::endl;
                  }
                }
                else
                  std::cout << "Version doesn't match "
                            << *module_version_it
                            << " expected: " << current->second->module_version
                            << std::endl;
              }
            }
            else
              std::cout << "download_id of DDB: " << *download_id_it
                        << " download_id expected: " << download_id << std::endl;
          }
        }
      }
    }
    else
    {
      std::cout << "polling failed with errno: " << errno << std::endl;
    }
  }
  while(true);
}

tune_thread::tune_thread(int frontendfd, unsigned int frequency)
  : frontendfd(frontendfd), frequency(frequency)
{
  moveToThread(this);
}

void tune_thread::run()
{
  std::cout << "dsmcc_download_tune_thread::run " << frequency << std::endl;
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
    Q_EMIT tune_failure ();
    return;
  }

  for(int i = 0; i != 10; ++i)
  {
    std::cout << "Trying lock" << std::endl;
    fe_status_t status;
    if(ioctl(frontendfd, FE_READ_STATUS, &status) == -1)
    {
      std::cout << "Error ioctl read status" << std::endl;
      break;
    }

    if(status & FE_HAS_LOCK)
    {
      std::cout << "reception_thread has locked frequency" << std::endl;
      Q_EMIT tune_success ();
      return;
    }
    Q_EMIT tune_increment();
    QThread::msleep(1000);
  }
  std::cout << "Failed tuning" << std::endl;
  Q_EMIT tune_failure();
}

pmt_thread::pmt_thread(int demuxfd)
  : demuxfd(demuxfd)
{
  moveToThread(this);
}

void pmt_thread::run()
{
  std::cout << "pmt_thread::run" << std::endl;
  pollfd pfd;
  pfd.fd = demuxfd.fd;
  pfd.events = POLLIN;
  pfd.revents = 0;
  int n = poll(&pfd, 1, 500);
  std::cout << "poll returned" << std::endl;
  if(n > 0)
  {
    std::cout << "going to read" << std::endl;
    bool found_ait = false;
    boost::shared_array<unsigned char> buffer(new unsigned char[4096]);
    int count = 0;
    do
    {
      count = ::read(demuxfd.fd, buffer.get (), 4096);
    } while (count < 0 && errno == EINTR);
    if(count < 0)
    {
      Q_EMIT pmt_failure();
      return;
    }

    if(count > 0)
    {
      std::cout << "read PMT" << std::endl;
      Q_EMIT pmt_read();

      typedef gts::sections::program_map_section<unsigned char const*> program_map_section;
      program_map_section pmt(buffer.get(), buffer.get() + count);
      typedef program_map_section::table_id_iterator table_id_iterator;
      typedef program_map_section::section_syntax_indicator_iterator section_syntax_indicator_iterator;
      typedef program_map_section::private_indicator_iterator private_indicator_iterator;
      typedef program_map_section::private_section_length_iterator private_section_length_iterator;
      typedef program_map_section::program_number_iterator program_number_iterator;
      typedef program_map_section::program_map_repetition_iterator program_map_repetition_iterator;

      sections_poll_thread.reset(new poll_thread);
      dsmcc_poll_thread.reset(new poll_thread);

      table_id_iterator table_id_it = pmt.begin();
      std::cout << "table_id " << *table_id_it << std::endl;
      program_number_iterator program_number_it = gts::iterators::next<4>(table_id_it);
      std::cout << "program_number: " << *program_number_it << std::endl;
      
      program_map_repetition_iterator program_maps_it = gts::iterators::next<8>(program_number_it);
      typedef program_map_repetition_iterator::deref_type program_map_range;
      program_map_range program_maps = *program_maps_it;
      bool ait_read = false;
      typedef boost::range_iterator<program_map_range>::type program_map_iterator;
      for(program_map_iterator first = boost::begin(program_maps)
            , last = boost::end(program_maps); first != last; ++first)
      {
        typedef boost::range_value<program_map_range>::type program_map;
        typedef program_map::stream_type_iterator stream_type_iterator;
        typedef program_map::elementary_pid_iterator elementary_pid_iterator;
        typedef program_map::es_info_length_iterator es_info_length_iterator;
        typedef program_map::descriptor_repetition_iterator descriptor_repetition_iterator;
        
        std::cout << "map" << std::endl;
        stream_type_iterator stream_type_it = first->begin();
        elementary_pid_iterator elementary_pid = gts::iterators::next(stream_type_it);
        switch(*stream_type_it)
        {
        case gts::constants::pmt::stream_types::private_sections:
        {
          std::cout << "might contain AIT" << std::endl;
          descriptor_repetition_iterator descriptors_it = gts::iterators::next<3>
            (stream_type_it);
          typedef descriptor_repetition_iterator::deref_type descriptor_range;
          descriptor_range descriptors = *descriptors_it;
          typedef boost::range_iterator<descriptor_range>::type descriptor_iterator;
          for(descriptor_iterator first = boost::begin(descriptors)
                , last = boost::end(descriptors); first != last; ++first)
          {
            typedef boost::range_value<descriptor_range>::type descriptor_type;
            typedef descriptor_type::tag_iterator tag_iterator;
            tag_iterator tag_it = first->begin();
            if(*tag_it == gts::constants::arib::descriptor_tags::data_component_descriptor)
            {
              std::cout << "Found ES with AIT" << std::endl;


              read_ait(*elementary_pid, ait_read);
              found_ait = true;
            }
          }
        }
        break;
        case gts::constants::pmt::stream_types::dsmcc_unspecified:
        case gts::constants::pmt::stream_types::dsmcc_type_a:
        case gts::constants::pmt::stream_types::dsmcc_type_b:
        case gts::constants::pmt::stream_types::dsmcc_type_c:
        case gts::constants::pmt::stream_types::dsmcc_type_d:
        {
          std::cout << "DSMCC sections" << std::endl;
          descriptor_repetition_iterator descriptors_it = gts::iterators::next<3>
            (stream_type_it);
          typedef descriptor_repetition_iterator::deref_type descriptor_range;
          descriptor_range descriptors = *descriptors_it;
          typedef boost::range_iterator<descriptor_range>::type descriptor_iterator;
          for(descriptor_iterator first = boost::begin(descriptors)
                , last = boost::end(descriptors); first != last; ++first)
          {
            typedef boost::range_value<descriptor_range>::type descriptor_type;
            typedef descriptor_type::tag_iterator tag_iterator;
            tag_iterator tag_it = first->begin();
            if(*tag_it == gts::constants::etsi::descriptor_tags::stream_identifier_descriptor)
            {
              std::cout << "Found stream identifier descriptor" << std::endl;
              typedef gts::descriptors::stream_identifier_descriptor
                <descriptor_type::base_iterator>
                stream_id_descriptor_type;
              stream_id_descriptor_type stream_identifier(*first);
              typedef stream_id_descriptor_type::component_tag_iterator component_tag_iterator;
              component_tag_iterator component_tag = gts::iterators::next(stream_identifier.begin());
              std::cout << "Component tag: " << *component_tag << std::endl;
              Q_EMIT association(*elementary_pid, *component_tag);
            }
          }
        }
        break;
        }
      }

      if(!found_ait)
        Q_EMIT ait_not_found();
      else if(ait_read)
        Q_EMIT this->ait_read();
      return;
    }
  }  
  
  Q_EMIT pmt_failure();
}

void pmt_thread::read_ait(int pid, bool& read)
{
  dmx_sct_filter_params filter;
  std::memset(&filter, 0, sizeof(filter));
  filter.pid = pid;
  filter.timeout = 0;
  filter.filter.filter[0] = 0x74;
  filter.filter.mask[0] = 0xFF;
  filter.flags = DMX_IMMEDIATE_START | DMX_ONESHOT | DMX_CHECK_CRC;
    
  if(ioctl(demuxfd.fd, DMX_SET_FILTER, &filter) == -1)
  {
    std::cout << "couldn't open another filter" << std::endl;
    Q_EMIT ait_timeout ();
    return;
  }

  pollfd pfd;
  pfd.fd = demuxfd.fd;
  pfd.events = POLLIN;
  pfd.revents = 0;
  int n = poll(&pfd, 1, 60000);
  if(n > 0)
  {
    std::cout << "going to read" << std::endl;
    boost::shared_array<unsigned char> buffer(new unsigned char[4096]);
    int count = 0;
    do
    {
      count = ::read(demuxfd.fd, buffer.get (), 4096);
    } while (count < 0 && errno == EINTR);
    if(count < 0)
    {
      std::cout << "Failed read " << errno << std::endl;
      Q_EMIT ait_timeout();
      return;
    }

    if(count > 0)
    {
      std::cout << "Found AIT" << std::endl;

      typedef gts::sections::application_information_section<unsigned char const*>
        application_information_section;
      application_information_section ait(buffer.get(), buffer.get() + count);

      typedef application_information_section::table_id_iterator table_id_iterator;
      typedef application_information_section::section_syntax_identifier_iterator 
        section_syntax_identifier_iterator;
      typedef application_information_section::section_length_iterator
        section_length_iterator;
      typedef application_information_section::application_type_iterator
        application_type_iterator;
      typedef application_information_section::version_number_iterator
        version_number_iterator;
      typedef application_information_section::current_next_indicator_iterator
        current_next_indicator_iterator;
      typedef application_information_section::section_number_iterator
        section_number_iterator;
      typedef application_information_section::last_section_number_iterator
        last_section_number_iterator;
      typedef application_information_section::descriptors_iterator descriptors_iterator;
      typedef application_information_section::applications_iterator applications_iterator;

      application_type_iterator application_type_it
        = gts::iterators::next<3>(ait.begin());

      descriptors_iterator descriptors_it = gts::iterators::next<5>(application_type_it);
      typedef descriptors_iterator::deref_type descriptor_range;
      descriptor_range descriptors = *descriptors_it;
      typedef boost::range_iterator<descriptor_range>::type descriptor_iterator;
      for(descriptor_iterator first = boost::begin(descriptors), last = boost::end(descriptors)
            ;first != last; ++first)
      {
        std::cout << "common AIT descriptor tag: " << *first->begin() << std::endl;
      }

      applications_iterator applications_it = gts::iterators::next(descriptors_it);
      typedef applications_iterator::deref_type application_range;
      application_range applications = *applications_it;
      typedef boost::range_iterator<application_range>::type application_iterator;
      for(application_iterator first = boost::begin(applications), last = boost::end(applications)
            ;first != last; ++first)
      {
        std::string name = "UNKNOWN", type, control_code
          , transport_protocol = "UNKNOWN";
        unsigned int component_id = 0;
        //bool service_bound = false;

        switch(*application_type_it)
        {
        case gts::constants::abnt::application_types::ginga_j:
          type = "GINGA-J";
          break;
        case gts::constants::abnt::application_types::ginga:
          type = "GINGA";
          break;
        case gts::constants::abnt::application_types::ginga_ncl:
          type = "GINGA-NCL";
          break;
        }

        typedef boost::range_value<application_range>::type application_type;
        std::cout << "A application" << std::endl;
        typedef application_type::organization_identifier_iterator organization_identifier_iterator;
        typedef application_type::application_identifier_iterator application_identifier_iterator;
        typedef application_type::application_control_code_iterator application_control_code_iterator;
        typedef application_type::application_descriptors_iterator application_descriptors_iterator;

        application_identifier_iterator application_identifier_it = gts::iterators::next(first->begin());
        std::cout << "application identifier: " << *application_identifier_it << std::endl;

        application_control_code_iterator application_control_code_it
          = gts::iterators::next(application_identifier_it);
        std::cout << "application control code: " << *application_control_code_it << std::endl;
        switch(*application_control_code_it)
        {
        case gts::constants::abnt::application_control_codes::autostart:
          control_code = "AUTOSTART";
          break;
        case gts::constants::abnt::application_control_codes::present:
          control_code = "PRESENT";
          break;
        case gts::constants::abnt::application_control_codes::destroy:
          control_code = "DESTROY";
          break;
        case gts::constants::abnt::application_control_codes::kill:
          control_code = "KILL";
          break;
        case gts::constants::abnt::application_control_codes::remote:
          control_code = "REMOTE";
          break;
        case gts::constants::abnt::application_control_codes::unbound:
          control_code = "UNBOUND";
          break;
        default:
        {
          std::stringstream s;
          s << "UNKNOWN 0x" << std::hex << *application_control_code_it;
          control_code = s.str();
          break;
        }
        }

        application_descriptors_iterator application_descriptors_it
          = gts::iterators::next(application_control_code_it);
        typedef application_descriptors_iterator::deref_type descriptor_range;
        descriptor_range descriptors = *application_descriptors_it;
        typedef boost::range_iterator<descriptor_range>::type descriptor_iterator;
        for(descriptor_iterator first = boost::begin(descriptors), last = boost::end(descriptors)
              ;first != last; ++ first)
        {
          typedef boost::range_value<descriptor_range>::type descriptor_type;
          switch(*first->begin())
          {
          case gts::constants::arib::descriptor_tags::application_descriptor:
            std::cout << "application descriptor" << std::endl;
            {
              typedef gts::descriptors::application_descriptor
                <descriptor_type::base_iterator> application_descriptor;
              application_descriptor descriptor(*first);

              typedef application_descriptor::application_profiles_length_iterator
                application_profiles_length_iterator;
              typedef application_descriptor::application_profiles_iterator
                application_profiles_iterator;
              typedef application_descriptor::service_bound_flag_iterator
                service_bound_flag_iterator;
              typedef application_descriptor::visibility_iterator
                visibility_iterator;
              typedef application_descriptor::application_priority_iterator
                application_priority_iterator;
              typedef application_descriptor::transport_protocol_label_iterator
                transport_protocol_label_iterator;

              std::cout << "application_profiles_length "
                        << *gts::iterators::next(descriptor.begin())
                        << std::endl;
              application_profiles_iterator application_profiles_it
                = gts::iterators::next<2>(descriptor.begin());
              typedef application_profiles_iterator::deref_type
                application_profile_range;
              application_profile_range application_profiles
                = *application_profiles_it;
              typedef boost::range_iterator<application_profile_range>::type
                application_profile_iterator;
              for(application_profile_iterator first = boost::begin(application_profiles)
                    , last = boost::end(application_profiles); first != last; ++first)
              {
                std::cout << "application profile" << std::endl;
              }
              service_bound_flag_iterator service_bound_flag_it
                = gts::iterators::next(application_profiles_it);
              // std::cout << "service bound flag: " << *service_bound_flag_it
              //           << std::endl;
              // service_bound = *service_bound_flag_it;
              visibility_iterator visibility_it
                = gts::iterators::next(service_bound_flag_it);
              std::cout << "visibility: " << *visibility_it << std::endl;
              application_priority_iterator application_priority_it
                = gts::iterators::next(visibility_it);
              std::cout << "app priority: " << *application_priority_it << std::endl;
            }
            break;
          case gts::constants::arib::descriptor_tags::application_name_descriptor:
            {
              std::cout << "application name descriptor" << std::endl;
              typedef gts::descriptors::application_name_descriptor
                <descriptor_type::base_iterator> application_name_descriptor;
              application_name_descriptor descriptor (*first);
              typedef application_name_descriptor::names_iterator names_iterator;
              names_iterator names_it = gts::iterators::next(descriptor.begin());
              typedef names_iterator::deref_type name_range;
              name_range names = *names_it;
              typedef boost::range_iterator<name_range>::type name_iterator;
              for(name_iterator first = boost::begin(names), last = boost::end(names)
                    ;first != last;++first)
              {
                typedef boost::range_value<name_range>::type name_type;
                name_type name_ = *first;
                typedef name_type::application_name_iterator app_name_iterator;
                app_name_iterator::deref_type app_name = *gts::iterators::next<2>(name_.begin());
                name.clear();
                name.insert(name.end(), boost::begin(app_name), boost::end(app_name));
              }
            }
            break;
          case gts::constants::arib::descriptor_tags::transport_protocol_descriptor:
            std::cout << "transport_protocol descriptor" << std::endl;
            {
              typedef gts::descriptors::transport_protocol_descriptor
                <descriptor_type::base_iterator> transport_protocol_descriptor;
              typedef transport_protocol_descriptor::protocol_id_iterator
                protocol_id_iterator;
              typedef transport_protocol_descriptor::transport_protocol_label_iterator
                transport_protocol_label_iterator;
              typedef transport_protocol_descriptor::selector_byte_iterator selector_byte_iterator;
              transport_protocol_descriptor descriptor(*first);
              protocol_id_iterator protocol_id_it = gts::iterators::next(descriptor.begin());
              std::cout << "Protocol id: ";
              switch(*protocol_id_it)
              {
              case gts::constants::etsi::protocol_ids::object_carousel:
                std::cout << "Object Carousel" << std::endl;
                transport_protocol = "Object Carousel";

                {
                  typedef transport_protocol_descriptor::object_carousel_transport
                    object_carousel_transport;
                  selector_byte_iterator selector_byte_it = gts::iterators::next<2>(protocol_id_it);
                  object_carousel_transport transport(*selector_byte_it);
                  typedef object_carousel_transport::remote_connection_iterator
                    remote_connection_iterator;
                  typedef object_carousel_transport::component_tag_iterator
                    component_tag_iterator;
                  component_tag_iterator component_it
                    = gts::iterators::next<4>(transport.begin());
                  std::cout << "has remote? " << *transport.begin() << std::endl;
                  component_id = *component_it;
                }
                break;
              case gts::constants::etsi::protocol_ids::http_return_channel:
                std::cout << "HTTP Return Channel" << std::endl;
                transport_protocol = "HTTP";
                break;
              default:
                std::cout << "Unknown " << *protocol_id_it << std::endl;
              }
            }
            break;
          case gts::constants::arib::descriptor_tags::ginga_j_application_descriptor:
            std::cout << "ginga j application descriptor" << std::endl;
            break;
          case gts::constants::arib::descriptor_tags::ginga_j_application_location_descriptor:
            std::cout << "ginga j application location descriptor" << std::endl;
          case gts::constants::arib::descriptor_tags::ginga_ncl_application_location_descriptor:
            if(*first->begin() == gts::constants::arib::descriptor_tags::ginga_ncl_application_location_descriptor)
              std::cout << "ginga ncl application location descriptor" << std::endl;

            {
              typedef gts::descriptors::ginga_application_location_descriptor
                <descriptor_type::base_iterator>
                ginga_application_location_descriptor;
              ginga_application_location_descriptor location(*first);
              typedef ginga_application_location_descriptor::tag_iterator tag_iterator;
              typedef ginga_application_location_descriptor::base_directory_iterator
                base_directory_iterator;
              typedef ginga_application_location_descriptor::classpath_extension_iterator
                classpath_extension_iterator;
              typedef ginga_application_location_descriptor::initial_class_iterator
                initial_class_iterator;
              if(base_directory.empty() && classpath.empty() && initial_class.empty())
              {
                base_directory_iterator::deref_type base_directory_ = *gts::iterators::next<1>(location.begin());
                base_directory.insert(base_directory.end()
                                      , boost::begin(base_directory_)
                                      , boost::end(base_directory_));
                std::cout << "base_directory: " << base_directory << std::endl;;
                classpath_extension_iterator::deref_type classpath_ = *gts::iterators::next<2>(location.begin());
                classpath.insert(classpath.end(), boost::begin(classpath_)
                                 , boost::end(classpath_));
                std::cout << "classpath extension: " << classpath << std::endl;;
                initial_class_iterator::deref_type initial_class_ = *gts::iterators::next<3>(location.begin());
                initial_class.insert(initial_class.end(), boost::begin(initial_class_)
                                     , boost::end(initial_class_));
                std::cout << "initial class: " << initial_class << std::endl;
              }
            }
            break;
          default:
            std::cout << "descriptor with tag: " << *first->begin() << std::endl;
          }
        }
        Q_EMIT signal_application(name, type, control_code, transport_protocol
                                  , component_id, base_directory, classpath
                                  , initial_class);
        
      }

      read = true;
      return;
    }
  }

  std::cout << "poll timeout" << std::endl;
  Q_EMIT ait_timeout();
}

}

dsmcc_download::dsmcc_download(std::string const& name, unsigned int frequency
                               , unsigned int service_identifier)
  : name(name), frequency(frequency), service_identifier(service_identifier)
  , downloaded_size(0), download_in_sec(0)
  , progress_bar(0), status_label(0), frontendfd(0)
  , download_thread_(0), action_thread(0)
{
  setAttribute(Qt::WA_DeleteOnClose);
  QGridLayout* vertical_layout = new QGridLayout;

  QHBoxLayout* service_layout = new QHBoxLayout;
  QLabel* service_text_label = new QLabel("Service: ");
  service_layout->addWidget(service_text_label);
  QLabel* service_label = new QLabel(QString::fromStdString(name));
  service_layout->addWidget(service_label);

  vertical_layout->addLayout(service_layout, 0, 0);
  
  QHBoxLayout* status_layout = new QHBoxLayout;
  QLabel* status_text_label = new QLabel("Status: ");
  status_layout->addWidget(status_text_label);
  status_label = new QLabel("Tuning");
  status_layout->addWidget(status_label);

  vertical_layout->addLayout(status_layout, 1, 0);

  QHBoxLayout* frequency_layout = new QHBoxLayout;
  QLabel* frequency_text_label = new QLabel("Frequency: ");
  frequency_layout->addWidget(frequency_text_label);
  QLabel* frequency_label = new QLabel(QString::number(frequency/1000000) + QString(" MHz"));
  frequency_layout->addWidget(frequency_label);

  vertical_layout->addLayout(frequency_layout, 2, 0);

  QHBoxLayout* service_id_layout = new QHBoxLayout;
  QLabel* service_id_text_label = new QLabel("Service Id: ");
  service_id_layout->addWidget(service_id_text_label);
  QLabel* service_id_label = new QLabel(QString("0x") + QString::number(service_identifier, 16));
  service_id_layout->addWidget(service_id_label);

  vertical_layout->addLayout(service_id_layout, 3, 0);

  QHBoxLayout* size_layout = new QHBoxLayout;
  QLabel* size_text_label = new QLabel("Carousel size: ");
  size_layout->addWidget(size_text_label);
  size_label = new QLabel("Unknown");
  size_layout->addWidget(size_label);

  vertical_layout->addLayout(size_layout, 4, 0);

  QHBoxLayout* downloaded_size_layout = new QHBoxLayout;
  QLabel* downloaded_size_text_label = new QLabel("Downloaded: ");
  downloaded_size_layout->addWidget(downloaded_size_text_label);
  downloaded_size_label = new QLabel("0 KB");
  downloaded_size_layout->addWidget(downloaded_size_label);

  vertical_layout->addLayout(downloaded_size_layout, 5, 0);

  QHBoxLayout* speed_layout = new QHBoxLayout;
  QLabel* speed_text_label = new QLabel("Speed: ");
  speed_layout->addWidget(speed_text_label);
  speed_label = new QLabel("0 KB/s");
  speed_layout->addWidget(speed_label);

  vertical_layout->addLayout(speed_layout, 6, 0);

  applications_widget = new QTableWidget(0, 4);
  applications_widget->setMinimumSize(QSize(550, 100));
  QStringList properties_header;
  properties_header.push_back("Application Name");
  properties_header.push_back("Application Type");
  properties_header.push_back("Control Code");
  properties_header.push_back("Transport Protocol");
  applications_widget->setHorizontalHeaderLabels(properties_header);
  applications_widget->resizeColumnsToContents();
  applications_widget->verticalHeader()->hide();
  applications_widget->horizontalHeader()->setStretchLastSection(true);

  vertical_layout->addWidget(applications_widget, 7, 0);

  QHBoxLayout* buttons_layout = new QHBoxLayout;
  download_button = new QPushButton("&Download");
  download_button->setEnabled(false);
  buttons_layout->addWidget(download_button);
  cancel_button = new QPushButton("&Cancel");
  buttons_layout->addWidget(cancel_button);

  vertical_layout->addLayout(buttons_layout, 8, 0);

  progress_bar = new QProgressBar;
  progress_bar->setMaximum(1);
  progress_bar->setMinimum(0u);
  progress_bar->setEnabled(false);
  vertical_layout->addWidget(progress_bar, 9, 0, 1, 1);
  
  setLayout(vertical_layout);

  QObject::connect(download_button, SIGNAL(clicked()), this, SLOT(download()));
  QObject::connect(cancel_button, SIGNAL(clicked()), this, SLOT(cancel()));
}

void dsmcc_download::exit_download_thread()
{
  assert(download_thread_ != 0);
  {
    QMutexLocker lock(download_thread_->mutex.get());
    download_thread_->cancel = true;
  }
  download_thread_->exit ();
  download_thread_->wait ();
}

dsmcc_download::~dsmcc_download()
{
  Q_EMIT destroying_window_signal();
  std::cout << "dsmcc_download::~dsmcc_download()" << std::endl;
  if(action_thread)
  {
    action_thread->exit ();
    action_thread->wait ();
    delete action_thread;
  }
  if(download_thread_)
  {
    exit_download_thread();
    delete download_thread_;
  }
  ::close(frontendfd);
}

void dsmcc_download::thread_destroy::operator ()(QThread* thread) const
{
  thread->exit ();
  thread->wait ();
  delete thread;
}

void dsmcc_download::start()
{
  frontendfd = open("/dev/dvb/adapter0/frontend0", O_RDONLY|O_RDWR);
  if(frontendfd == -1)
  {
    status_label->setText("Failed opening device frontend");
    Q_EMIT download_failure();
    return;
  }

  progress_bar->setMaximum(10u);
  progress_bar->setMinimum(0u);
  progress_bar->setValue(0);
  progress_bar->setEnabled(true);
  
  action_thread = new dsmcc_download_detail::tune_thread(frontendfd, frequency);
  QObject::connect(action_thread, SIGNAL(tune_success()), this, SLOT(tune_success())
                   , Qt::QueuedConnection);
  QObject::connect(action_thread, SIGNAL(tune_failure()), this, SLOT(tune_failure())
                   , Qt::QueuedConnection);
  QObject::connect(action_thread, SIGNAL(tune_increment()), this, SLOT(tune_increment())
                   , Qt::QueuedConnection);
  action_thread->start();
}

void dsmcc_download::tune_success()
{
  std::cout << "tune_success" << std::endl;
  progress_bar->setValue(10u);

  action_thread->exit ();
  action_thread->wait ();
  delete action_thread;
  action_thread = 0;

  status_label->setText("Reading PAT");

  int demuxfd = open("/dev/dvb/adapter0/demux0", O_RDWR|O_NONBLOCK);
  if(demuxfd == -1)
  {
    status_label->setText("Failed opening device demuxer");
    Q_EMIT download_failure();
    return;
  }

  dsmcc_download_detail::close_fd close_demux_fd(demuxfd);

  dmx_sct_filter_params filter;
  std::memset(&filter, 0, sizeof(filter));
  filter.pid = 0; // PAT
  filter.timeout = 0;
  filter.flags = DMX_IMMEDIATE_START | DMX_ONESHOT | DMX_CHECK_CRC;
    
  if(ioctl(demuxfd, DMX_SET_FILTER, &filter) == -1)
  {
    std::cout << "couldn't open another filter" << std::endl;
    Q_EMIT download_failure ();
    return;
  }

  pollfd pfd;
  pfd.fd = demuxfd;
  pfd.events = POLLIN;
  pfd.revents = 0;
  int n = poll(&pfd, 1, 10000);
  progress_bar->setValue(0u);
  progress_bar->setEnabled(false);
  if(n > 0)
  {
    boost::shared_array<unsigned char> buffer(new unsigned char[4096]);
    int count = ::read(demuxfd, buffer.get (), 4096);
    if(count > 0)
    {
      QObject::connect(this, SIGNAL(read_pat_signal (boost::shared_array<unsigned char>, unsigned int))
                       , this, SLOT(read_pat (boost::shared_array<unsigned char>, unsigned int))
                       , Qt::QueuedConnection);
      Q_EMIT read_pat_signal(buffer, count);
      return;
    }
  }

  status_label->setText("Failed reading PAT");
  Q_EMIT download_failure();
}

void dsmcc_download::read_pat (boost::shared_array<unsigned char> buf, unsigned int size)
{
  std::cout << "Read PAT" << std::endl;
  status_label->setText("Parsing PAT");

  typedef gts::sections::program_association_section<unsigned char const*> program_association_section_type;
  program_association_section_type program_association_section(&buf[0], &buf[0] + size);

  typedef program_association_section_type::program_associations_iterator program_associations_iterator;
  program_associations_iterator program_associations_it = gts::iterators::next<8>
    (program_association_section.begin());
  typedef program_associations_iterator::deref_type program_association_range;
  program_association_range program_associations = *program_associations_it;
  typedef boost::range_iterator<program_association_range>::type program_association_iterator;
  for(program_association_iterator first = boost::begin(program_associations)
        , last = boost::end(program_associations); first != last; ++first)
  {
    typedef boost::range_value<program_association_range>::type program_association_type;
    if(first->program_number == service_identifier)
    {
      status_label->setText("Searching PMT");
      
      std::cout << "PMT PID: " << first->pid << std::endl;

      int demuxfd = open("/dev/dvb/adapter0/demux0", O_RDWR|O_NONBLOCK);
      if(demuxfd == -1)
      {
        status_label->setText("Failed opening device demuxer");
        Q_EMIT download_failure();
        return;
      }

      dmx_sct_filter_params filter;
      std::memset(&filter, 0, sizeof(filter));
      filter.pid = first->pid;
      filter.timeout = 0;
      filter.flags = DMX_IMMEDIATE_START | DMX_ONESHOT | DMX_CHECK_CRC;
      filter.filter.filter[0] = 2;
      filter.filter.mask[0] = 0xFF;
      
      if(ioctl(demuxfd, DMX_SET_FILTER, &filter) == -1)
      {
        std::cout << "couldn't open another filter" << std::endl;
        Q_EMIT download_failure ();
        return;
      }

      assert(action_thread == 0);
      action_thread = new dsmcc_download_detail::pmt_thread(demuxfd);
      QObject::connect(action_thread, SIGNAL(pmt_read())
                       , this, SLOT(pmt_read())
                       , Qt::QueuedConnection);
      QObject::connect(action_thread, SIGNAL(ait_not_found())
                       , this, SLOT(ait_not_found())
                       , Qt::QueuedConnection);
      QObject::connect(action_thread, SIGNAL(pmt_failure())
                       , this, SLOT(pmt_failure())
                       , Qt::QueuedConnection);
      QObject::connect(action_thread, SIGNAL(ait_timeout())
                       , this, SLOT(ait_timeout())
                       , Qt::QueuedConnection);
      QObject::connect(action_thread, SIGNAL(ait_read())
                       , this, SLOT(ait_read())
                       , Qt::QueuedConnection);
      QObject::connect(action_thread, SIGNAL(association(unsigned int, unsigned int))
                       , this, SLOT(association(unsigned int, unsigned int))
                       , Qt::QueuedConnection);
      QObject::connect(action_thread, SIGNAL(signal_application(std::string, std::string, std::string, std::string, unsigned int, std::string, std::string, std::string))
                       , this, SLOT(application(std::string, std::string, std::string, std::string, unsigned int, std::string, std::string, std::string))
                       , Qt::QueuedConnection);

      action_thread->start();

      return;
    }
  }

  status_label->setText("Couldn't find service in program");
  Q_EMIT download_failure();
}

void dsmcc_download::poll_timeout ()
{
  std::cout << "timeout reading packet" << std::endl;
}

void dsmcc_download::tune_failure()
{
  std::cout << "tune_failure" << std::endl;
  status_label->setText("Failed opening device frontend");
  Q_EMIT download_failure();
}

void dsmcc_download::tune_increment()
{
  std::cout << "tune_increment" << std::endl;
  progress_bar->setValue(progress_bar->value() + 1);
}

void dsmcc_download::pmt_read()
{
  std::cout << "PMT read" << std::endl;
  status_label->setText("Searching AIT");
}

void dsmcc_download::ait_not_found()
{
  std::cout << "AIT not found" << std::endl;
  status_label->setText("Not found AIT. Probably no application is being transmitted");
}

void dsmcc_download::pmt_failure()
{
  std::cout << "PMT not found" << std::endl;
  status_label->setText("PMT not found");
}

void dsmcc_download::ait_read()
{
  std::cout << "AIT read" << std::endl;
  status_label->setText("AIT read");
  progress_bar->setEnabled(false);
  if(applications_widget->selectedItems().empty())
  {
    applications_widget->setCurrentCell(0, 0);
    download_button->setEnabled(true);
  }
  action_thread->exit();
  delete action_thread;
  action_thread = 0;
}

void dsmcc_download::cancel()
{
  close();
}

void dsmcc_download::download()
{
  std::cout << "Download" << std::endl;
  download_button->setEnabled(false);
  application_info info = applications[applications_widget->currentRow()];
  std::cout << "Download component_id: " << info.component_id << std::endl;
  std::vector<std::pair<unsigned int, unsigned int> >::const_iterator
    current = find_association(associations.begin(), associations.end()
                               , info.component_id);
  if(current == associations.end())
  {
    status_label->setText("Not found association for component tag");
  }
  else
  {
    std::cout << "Should search for DSI in pid " << current->first << std::endl;
    status_label->setText("Searching DSI");
    download_thread_ = new dsmcc_download_detail::download_thread(current->first, associations);
    base_directory = info.base_directory;
    classpath = info.classpath;
    initial_class = info.initial_class;
    QObject::connect(download_thread_, SIGNAL(waiting_dii()), this, SLOT(waiting_dii())
                     , Qt::QueuedConnection);
    QObject::connect(download_thread_, SIGNAL(downloading(unsigned int, unsigned int)), this
                     , SLOT(ddb_downloading(unsigned int, unsigned int))
                     , Qt::QueuedConnection);
    QObject::connect(download_thread_, SIGNAL(block_downloaded(unsigned int)), this
                     , SLOT(block_downloaded(unsigned int))
                     , Qt::QueuedConnection);
    QObject::connect(download_thread_, SIGNAL(downloaded()), this, SLOT(ddb_downloaded())
                     , Qt::QueuedConnection);

    applications_widget->setEnabled(false);
    download_thread_->start();
  }
}

void dsmcc_download::waiting_dii()
{
  std::cout << "Downloading dii" << std::endl;
  status_label->setText("Searching DII");
}

void dsmcc_download::ddb_downloading(unsigned int blocks_to_download
                                     , unsigned int size)
{
  std::cout << "Downloading DDB " << blocks_to_download
            << " with total size: " << size << std::endl;
  status_label->setText("Downloading Carousel");
  progress_bar->setMaximum(blocks_to_download);
  progress_bar->setValue(0);
  size_label->setText(QString::number(size/1024) + " KB");
  QTimer::singleShot(1000, this, SLOT(speed_measure()));
}

void dsmcc_download::speed_measure()
{
  std::cout << "dsmcc_download::speed_measure" << std::endl;
  if(download_thread_)
  {
    std::cout << "download_in_sec: " << download_in_sec << std::endl;
    speed_label->setText(QString::number(download_in_sec/1000) + " KB/s");
    download_in_sec = 0;
    QTimer::singleShot(1000, this, SLOT(speed_measure()));
  }
}

void dsmcc_download::block_downloaded(unsigned int size)
{
  progress_bar->setValue(progress_bar->value() + 1);
  downloaded_size += size;
  downloaded_size_label->setText(QString::number(downloaded_size/1024) + " KB");
  download_in_sec += size;
}

void dsmcc_download::ddb_downloaded()
{
  std::cout << "Downloaded" << std::endl;
  status_label->setText("Downloaded Carousel");
  download_button->setText("&OK");
  QObject::disconnect(download_button, SIGNAL(clicked()), this, SLOT(download()));
  QObject::connect(download_button, SIGNAL(clicked()), this, SLOT(start_app()));
  download_button->setEnabled(true);
  exit_download_thread();
}

void dsmcc_download::start_app()
{
  std::cout << "Should start app" << std::endl;
  Q_EMIT download_success();
}

void dsmcc_download::ait_timeout()
{
  std::cout << "AIT not found" << std::endl;
  status_label->setText("Timeout waiting for AIT");
}

void dsmcc_download::association(unsigned int pid, unsigned int component_tag)
{
  std::cout << "Association between PID: " << pid << " and component_tag: "
            << component_tag << std::endl;
  associations.push_back(std::make_pair(pid, component_tag));
}

void dsmcc_download::application(std::string name, std::string type
                                 , std::string control_code
                                 , std::string transport_protocol
                                 , unsigned int component_id
                                 , std::string base_directory
                                 , std::string classpath
                                 , std::string initial_class)
{
  std::cout << "Application " << name << " type " << type
            << " control code " << control_code << " transport protocol "
            << transport_protocol << " component id " << component_id
            << std::endl;
  unsigned int row = applications_widget->rowCount();
  applications_widget->setRowCount(row+1);
  applications_widget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(name)));
  applications_widget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(type)));
  applications_widget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(control_code)));
  applications_widget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(transport_protocol)));
  application_info a = {component_id, base_directory, classpath, initial_class};
  applications.push_back(a);
}

} }
