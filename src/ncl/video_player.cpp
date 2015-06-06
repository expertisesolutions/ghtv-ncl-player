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

#include <ghtv/qt/ncl/player/video_player.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace ghtv { namespace qt { namespace ncl { namespace player {

video_player::video_player(std::string const& source, std::string const& root_path, QWidget* player_container
                           , ncl_widget* ncl_window
                           , gntl::structure::composed::component_location
                           <std::string, std::string> location
                           , ncl::executor* e
                           , boost::optional<std::string> const& interface_)
  : common_player(e, location), ncl_window(ncl_window)
  , location(location)
  , seek(boost::posix_time::seconds(0))
{
  if(interface_)
    for(std::vector<executor::time_area>::iterator first = e->time_areas.begin()
          , last = e->time_areas.end(); first != last; ++first)
      if(*interface_ == first->interface_)
      {
        if(first->begin != boost::posix_time::not_a_date_time)
          seek = first->begin;
        break;
      }

  video_player_ = new Phonon::VideoPlayer(Phonon::VideoCategory, player_container);
  video_player_->move(0, 0);
  video_player_->resize(player_container->size());
  video_player_->show();

  Phonon::MediaSource source_(create_url(source, root_path));
  connect(video_player_->mediaObject(), SIGNAL(hasVideoChanged(bool))
          , this, SLOT(has_video_changed(bool)));
  connect(video_player_->mediaObject(), SIGNAL(stateChanged(Phonon::State, Phonon::State))
          , this, SLOT(video_state_changed(Phonon::State, Phonon::State)));
  video_player_->load(source_);
}

void video_player::has_video_changed(bool)
{
  video_player_->play ();
}

video_player::~video_player()
{
  if(video_player_)
    video_player_->stop();
}

void video_player::pause()
{
  if(video_player_)
    video_player_->pause();
  common_player::pause_timers ();
}

void video_player::resume()
{
  if(video_player_)
    video_player_->play();
  common_player::resume_timers ();
}

void video_player::key_process(std::string const& key, bool pressed)
{
}

bool video_player::wants_keys() const
{
  return false; 
}

bool video_player::start_set_property(std::string const& name, std::string const& value)
{
  return false;
}

bool video_player::commit_set_property(std::string const& name)
{
  return false;
}

void video_player::video_state_changed(Phonon::State new_state, Phonon::State old_state)
{
  if(new_state == Phonon::ErrorState)
  {
    std::string error = "Error playing video:\n";
    error += video_player_->mediaObject()->errorString().toStdString();
    ncl_window->error_occurred(error);
  }
  else if(old_state != Phonon::PausedState
          && new_state == Phonon::PlayingState)
  {
    if(video_player_->mediaObject ()->isSeekable () && seek.total_milliseconds ())
      video_player_->seek(seek.total_milliseconds ());
    
    setup_timers(seek);
    // ncl_window->video_signal_finish(e, video_player_, media_identifier, context_identifier);
  }
}

} } } }
