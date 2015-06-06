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

#ifndef GHTV_QT_POLL_THREAD_HPP
#define GHTV_QT_POLL_THREAD_HPP

#include <QThread>
#include <QMutex>
#include <QTimer>

#include <boost/shared_array.hpp>

#include <vector>

#include <poll.h>

namespace ghtv { namespace qt {

class poll_thread : public QThread
{
  Q_OBJECT
public:
  poll_thread();
  ~poll_thread();
  void run ();

  void add_pollfd(pollfd);
  void pop_front_pollfd ();
  void start_poll ();

  QTimer* idle_timer;
  // assert(retries.size () == pollfds.size ());
  std::vector<unsigned int> retries;
  std::vector<pollfd> pollfds;
Q_SIGNALS:
  void add_pollfd_signal (pollfd);
  void pop_front_signal ();
  void timeout_signal ();
  void read_packet_signal (boost::shared_array<unsigned char>, unsigned int size);
  void start_timer_signal ();
private Q_SLOTS:
  void add_pollfd_slot (pollfd);
  void pop_front_slot ();
  void run_poll ();
  void start_timer ();
};

} }

#endif
