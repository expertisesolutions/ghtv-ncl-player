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

#ifndef GHTV_QT_PLAYER_COMMON_PLAYER_HPP
#define GHTV_QT_PLAYER_COMMON_PLAYER_HPP

#include <ghtv/qt/ncl/executor.hpp>
#include <ghtv/qt/ncl/player/player_base.hpp>

#include <gntl/structure/composed/component_location.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <QObject>

namespace ghtv { namespace qt { namespace ncl { namespace player {

class time_area_event;

class common_player : public QObject, public player_base
{
  Q_OBJECT
public:
  common_player(ncl::executor* e
                , gntl::structure::composed::component_location
                <std::string, std::string> location);

  ncl::executor* e;
  ncl::executor* executor() const { return e; }

  std::list<time_area_event*> time_areas;
  boost::posix_time::time_duration total_paused_time
    , seek;
  gntl::structure::composed::component_location<std::string, std::string> location;
  boost::posix_time::ptime start_time;
  boost::posix_time::ptime pause_start_time;

  void setup_timers(boost::posix_time::time_duration seek = boost::posix_time::seconds(0));
  void stop();
  void pause_timers ();
  void resume_timers ();
public Q_SLOTS:
  void lifetime_timeout();
};

class time_area_event : public QObject
{
  Q_OBJECT
public:
  time_area_event(common_player* player
                  , executor::component_identifier interface_
                  , boost::posix_time::time_duration begin
                  , boost::posix_time::time_duration end)
    : QObject(player), player(player), invalid(false), beginning_expired(false)
    , interface_(interface_), begin(begin), end(end)
  {}

  common_player* player;
  bool invalid, beginning_expired;
  executor::component_identifier interface_;
  boost::posix_time::time_duration begin, end;

public Q_SLOTS:
  void timer_begin_expired();
  void timer_end_expired();
};

} } } }

#endif
