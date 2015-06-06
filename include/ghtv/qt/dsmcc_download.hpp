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

#ifndef GHTV_QT_DSMCC_DOWNLOAD_HPP
#define GHTV_QT_DSMCC_DOWNLOAD_HPP

#include <ghtv/qt/poll_thread.hpp>
#include <ghtv/qt/dsmcc.hpp>

#include <QWidget>
#include <QTableWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>

#include <boost/shared_ptr.hpp>

#include <string>
#include <set>
#include <vector>

namespace ghtv { namespace qt {

namespace dsmcc_download_detail {

struct close_fd
{
  close_fd() : fd(-1) {}
  close_fd(int fd)
  : fd(fd) {}
  ~close_fd()
  {
    if(fd != -1)
      ::close(fd);
  }
  int fd;
};

class tune_thread : public QThread
{
  Q_OBJECT
public:
  tune_thread(int frontendfd, unsigned int frequency);

  void run();

  int frontendfd;
  unsigned int frequency;
Q_SIGNALS:
  void tune_success();
  void tune_failure();
  void tune_increment();
};

class pmt_thread : public QThread
{
  Q_OBJECT
public:
  pmt_thread(int demuxfd);

  void run();
  void read_ait(int pid, bool&);

  std::string base_directory, classpath, initial_class;
  close_fd demuxfd;
  boost::shared_ptr<qt::poll_thread> sections_poll_thread;
  boost::shared_ptr<qt::poll_thread> dsmcc_poll_thread;
Q_SIGNALS:
  void tune_success();
  void ait_timeout();
  void pmt_failure();
  void pmt_read();
  void ait_read();
  void ait_not_found();
  void association(unsigned int pid, unsigned int component_tag);
  void signal_application(std::string name, std::string type
                          , std::string control_code
                          , std::string transport_protocol
                          , unsigned int component_id
                          , std::string base_directory
                          , std::string classpath
                          , std::string initial_class);
};

class download_thread : public QThread
{
  Q_OBJECT
public:
  download_thread(unsigned int pid
                  , std::vector<std::pair<unsigned int, unsigned int> > associations);

  void run();
  void filter(unsigned int pid, unsigned int table_id);
  void stop_filter();

  bool modules_finished()
  {
    for(std::map<unsigned int, boost::shared_ptr<module> >::const_iterator
          first = modules.begin(), last = modules.end()
          ;first != last; ++first)
    {
      if(!first->second->finished())
        return false;
    }
    return true;
  }

  unsigned int pid;
  unsigned int dii_pid;
  std::vector<std::pair<unsigned int, unsigned int> > associations;
  close_fd demuxfd;
  bool dsi_resolved, dii_resolved;
  unsigned int transaction_identification;
  unsigned int service_gateway_module_id, download_id;
  unsigned int block_size;
  unsigned int blocks_to_download;

  std::auto_ptr<QMutex> mutex;
  bool cancel;

  std::map<unsigned int, boost::shared_ptr<module> > modules;
  std::set<unsigned int> modules_downloaded;
Q_SIGNALS:
  void waiting_dii();
  void downloading(unsigned int blocks_to_download, unsigned int size);
  void block_downloaded(unsigned int);
  void downloaded();
};

}

class dsmcc_download : public QWidget
{
  Q_OBJECT
public:
  dsmcc_download(std::string const& name, unsigned int frequency
                 , unsigned int service_identifier);
  ~dsmcc_download();

  void start();

  std::string name;
  unsigned int frequency;
  unsigned int service_identifier;
  unsigned int downloaded_size;
  unsigned int download_in_sec;
  QProgressBar* progress_bar;
  QLabel* status_label, *size_label
    , *downloaded_size_label, *speed_label;
  QTableWidget* applications_widget;
  int frontendfd;
  dsmcc_download_detail::download_thread* download_thread_;
  QThread* action_thread;
  QPushButton *download_button, *cancel_button;

  struct thread_destroy
  {
    typedef void result_type;
    void operator () (QThread* thread) const;
  };

  struct application_info
  {
    unsigned int component_id;
    std::string base_directory, classpath, initial_class;
  };

  std::vector<application_info> applications;
  std::vector<std::pair<unsigned int, unsigned int> > associations;
  std::string base_directory, classpath, initial_class;

  void exit_download_thread();

Q_SIGNALS:
  void download_success();
  void download_failure();
  void read_pat_signal (boost::shared_array<unsigned char> buf, unsigned int size);
  void destroying_window_signal();
public Q_SLOTS:
  void tune_success();
  void tune_failure();
  void tune_increment();
  void read_pat (boost::shared_array<unsigned char> buf, unsigned int size);
  void poll_timeout ();
  void pmt_read();
  void pmt_failure();
  void ait_not_found();
  void ait_read();
  void ait_timeout();
  void cancel();
  void download();
  void start_app();
  void association(unsigned int pid, unsigned int component_tag);
  void application(std::string name, std::string type
                   , std::string control_code
                   , std::string transport_protocol
                   , unsigned int component_id
                   , std::string base_directory
                   , std::string classpath
                   , std::string initial_class);
  void waiting_dii();
  void ddb_downloading(unsigned int blocks_to_download, unsigned int size);
  void block_downloaded(unsigned int);
  void ddb_downloaded();
  void speed_measure();
};

} }

#endif

