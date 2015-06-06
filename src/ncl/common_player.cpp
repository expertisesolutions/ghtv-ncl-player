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

#include <ghtv/qt/ncl/player/common_player.hpp>
#include <ghtv/qt/ncl/ncl_widget.hpp>

#include <QTimer>

namespace ghtv { namespace qt { namespace ncl { namespace player {

common_player::common_player(ncl::executor* e
                             , gntl::structure::composed::component_location
                             <std::string, std::string> location)
  : e(e), total_paused_time (boost::posix_time::seconds (0))
  , location(location)
{
}

void common_player::setup_timers(boost::posix_time::time_duration seek_)
{
  seek = seek_;
  start_time = boost::posix_time::second_clock::universal_time ();
  if(e->explicit_duration != boost::posix_time::not_a_date_time)
  {
    if(e->explicit_duration > seek)
      QTimer::singleShot(e->explicit_duration.total_milliseconds() - seek.total_milliseconds()
                         , this, SLOT(lifetime_timeout()));
  }

  for(std::vector<executor::time_area>::iterator first = e->time_areas.begin()
        , last = e->time_areas.end(); first != last; ++first)
  {
    time_area_event* event = new time_area_event(this, first->interface_
                                                 , first->begin, first->end);
    time_areas.push_back(event);
    if(event->begin != boost::posix_time::not_a_date_time
       && event->begin >= seek)
    {
      QTimer::singleShot(event->begin.total_milliseconds() - seek.total_milliseconds ()
                         , event, SLOT(timer_begin_expired()));
    }
    if(event->end != boost::posix_time::not_a_date_time
       && event->end >= seek)
    {
      QTimer::singleShot(event->end.total_milliseconds() - seek.total_milliseconds ()
                         , event, SLOT(timer_end_expired()));
    }
  }
}

void common_player::pause_timers ()
{
  pause_start_time = boost::posix_time::second_clock::universal_time ();
  for(std::list<time_area_event*>::iterator first = time_areas.begin()
        , last = time_areas.end(); first != last; ++first)
  {
    if(!(*first)->invalid && ((*first)->beginning_expired
                              || (*first)->begin == boost::posix_time::not_a_date_time)
       && (*first)->end != boost::posix_time::not_a_date_time)
    {
      e->root_window->area_event
        (location, (*first)->interface_, gntl::transition_enum::stops);
    }
    (*first)->invalid = true;
  }
  time_areas.clear();
}

void common_player::resume_timers ()
{
  total_paused_time += boost::posix_time::second_clock::universal_time () - pause_start_time;
  std::cout << "total_paused_time: " << total_paused_time << std::endl;
  std::cout << "now: " << boost::posix_time::second_clock::universal_time ()
            << std::endl;
  std::cout << "start_time: " << start_time << std::endl;
  boost::posix_time::time_duration offset_time = boost::posix_time::second_clock::universal_time ()
    - start_time - total_paused_time;

  std::cout << "offset time: " << offset_time << std::endl;
  
  for(std::vector<executor::time_area>::iterator first = e->time_areas.begin()
        , last = e->time_areas.end(); first != last; ++first)
  {
    time_area_event* event = new time_area_event(this, first->interface_
                                                 , first->begin, first->end);
    time_areas.push_back(event);
    if(event->begin != boost::posix_time::not_a_date_time
       && event->begin - offset_time >= seek)
    {
      assert(event->begin.total_milliseconds() - seek.total_milliseconds ()
             - offset_time.total_milliseconds () >= 0);
      QTimer::singleShot(event->begin.total_milliseconds() - seek.total_milliseconds ()
                         - offset_time.total_milliseconds ()
                         , event, SLOT(timer_begin_expired()));
    }
    if(event->end != boost::posix_time::not_a_date_time
       && event->end - offset_time >= seek)
    {
      assert(event->end.total_milliseconds() - seek.total_milliseconds ()
             - offset_time.total_milliseconds () >= 0);
      QTimer::singleShot(event->end.total_milliseconds() - seek.total_milliseconds ()
                         - offset_time.total_milliseconds ()
                         , event, SLOT(timer_end_expired()));
    }
  }
}

void common_player::lifetime_timeout()
{
  ncl_widget* window = e->root_window;
  stop();
  window->natural_end(location);
}

void common_player::stop()
{
  for(std::list<time_area_event*>::iterator first = time_areas.begin()
        , last = time_areas.end(); first != last; ++first)
  {
    if(!(*first)->invalid && ((*first)->beginning_expired
                              || (*first)->begin == boost::posix_time::not_a_date_time)
       && (*first)->end != boost::posix_time::not_a_date_time)
    {
      e->root_window->area_event
        (location, (*first)->interface_
         , gntl::transition_enum::stops);
    }
    (*first)->invalid = true;
  }
  time_areas.clear();

  e->stop();
}

void time_area_event::timer_begin_expired()
{
  if(!invalid)
  {
    assert(begin != boost::posix_time::not_a_date_time);
    beginning_expired = true;

    player->e->root_window->area_event
      (player->location, interface_, gntl::transition_enum::starts);

    if(end == boost::posix_time::not_a_date_time)
    {
      assert(std::find(player->time_areas.begin(), player->time_areas.end(), this)
             != player->time_areas.end());
      player->time_areas.erase
        (std::find(player->time_areas.begin(), player->time_areas.end(), this));
    }
  }
}

void time_area_event::timer_end_expired()
{
  if(!invalid)
  {
    assert(end != boost::posix_time::not_a_date_time);
    assert(beginning_expired || begin == boost::posix_time::not_a_date_time);

    player->e->root_window->area_event
      (player->location, interface_, gntl::transition_enum::stops);
  }
}

} } } }
