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

#ifndef GHTV_QT_RECEPTION_HPP
#define GHTV_QT_RECEPTION_HPP

#include <ghtv/qt/poll_thread.hpp>

#include <linux/dvb/frontend.h>

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>

#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

namespace ghtv { namespace qt {

const unsigned int frequencies []
= {
  473142857, 479142857, 485142857, 491142857, 497142857, 503142857, 509142857, 515142857, 521142857, 527142857, 
  533142857, 539142857, 545142857, 551142857, 557142857, 563142857, 569142857, 575142857, 581142857, 587142857, 
  593142857, 599142857, 605142857, 617142857, 623142857, 629142857, 635142857, 641142857, 647142857, 653142857, 
  659142857, 665142857, 671142857, 677142857, 683142857, 689142857, 695142857, 701142857, 707142857, 713142857, 
  719142857, 725142857, 731142857, 737142857, 743142857, 749142857, 755142857, 761142857, 767142857, 773142857, 
  779142857, 785142857, 791142857, 797142857, 803142857
};

class reception;

class reception_thread : public QThread
{
  Q_OBJECT
public:
  reception_thread(int frontendfd
                   , std::string adapter_index, std::string frontend_index);
  ~reception_thread ();

  void run ();
  bool parse_sdt (unsigned char const* buf, int size);

  int frontendfd;

  std::string adapter_index, frontend_index;

  struct ready_state {};

  struct frequency_locking_state
  {
    unsigned int frequency;
    std::size_t retries;
    frequency_locking_state(unsigned int frequency)
      : frequency(frequency), retries(0u) {}
  };

  struct frequency_lock_state
  {
    unsigned int frequency;
    
    struct thread_destroy
    {
      typedef void result_type;
      void operator () (QThread* thread) const;
    };

    boost::shared_ptr<qt::poll_thread> poll_thread;

    frequency_lock_state (unsigned int frequency);
    ~frequency_lock_state ();
  };

  typedef boost::variant<ready_state, frequency_locking_state, frequency_lock_state> variant_state;
  variant_state state;
Q_SIGNALS:
  void frequency_locked_signal ();
  void lock_error_signal ();
  void network_found (std::string name, unsigned int service_identifier);
  void lock_end_signal ();
public Q_SLOTS:
  void lock_frequency(unsigned int frequency);
  void check_lock_frequency();
  void filter_sdt ();
  void pollfd_timeout ();
  void read_packet (boost::shared_array<unsigned char>, unsigned int size);
};

class reception : public QWidget
{
  Q_OBJECT

public:
  reception ();
  ~reception();

  std::size_t current_frequency_index;

  QListWidget* channels_list;
  QLineEdit* adapter_edit, *frontend_edit;
  QPushButton *scan_button, *stop_scan_button, *select_button;
  QProgressBar* progress_bar;

  struct channel
  {
    std::string name;
    unsigned int frequency;
    unsigned int service_identifier;
  };

  std::vector<channel> channels;
  
  void start_scan ();
  void scan_finished();

  bool scan_in_progress, should_stop_scan;
  int frontendfd;
  QThread* scan_thread;
Q_SIGNALS:
  void selected_channel(std::string, unsigned int frequency, unsigned int service_identifier);
  void lock_frequency(unsigned int frequency);
  void destroying_window_signal();
public Q_SLOTS:
  void stop_scan ();
  void scan ();
  void select ();
  void lock_error ();
  void lock_end ();
  void network_found(std::string name, unsigned int service_identifier);
  void channels_list_selection_changed(QListWidgetItem*, QListWidgetItem*);
};

} }

#endif
