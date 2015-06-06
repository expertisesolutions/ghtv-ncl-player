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

#include <ghtv/qt/poll_thread.hpp>

#include <poll.h>

namespace ghtv { namespace qt {

void poll_thread::add_pollfd(pollfd pfd)
{
  Q_EMIT add_pollfd_signal(pfd);
}

void poll_thread::pop_front_pollfd()
{
  Q_EMIT pop_front_signal();
}

void poll_thread::pop_front_slot ()
{
  retries.erase(retries.begin ());
  pollfds.erase(pollfds.begin ());
}

void poll_thread::add_pollfd_slot (pollfd pfd)
{
  retries.push_back(0u);
  pollfds.push_back(pfd);
}

} }
