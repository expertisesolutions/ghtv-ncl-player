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

#include <ghtv/qt/ncl/player/audio_player.hpp>

namespace ghtv { namespace qt { namespace ncl { namespace player {

namespace audio_detail {

void MediaObject::stateChanged_impl(Phonon::State state, Phonon::State old)
{
  if(state == Phonon::ErrorState)
  {
    std::string error = "Error playing audio: ";
    error += errorString().toStdString();
    ncl->error_occurred(error);
  }
  else if(state == Phonon::PlayingState)
  {
    if(player)
      player->setup_timers();
  }
}

}

audio_player::audio_player(std::string const& source, std::string const& root_path, QWidget* player_container
                           , ncl_widget* ncl, gntl::structure::composed::component_location
                           <std::string, std::string> location
                           , ncl::executor* executor_)
  : common_player(executor_, location), sound_level_temporary(0)
  , media_object(0), audio_output(0)
{
  std::auto_ptr<audio_detail::MediaObject> media_object
    (new audio_detail::MediaObject(player_container, ncl, this));
  std::auto_ptr<Phonon::AudioOutput> audio_output(new Phonon::AudioOutput(player_container));

  QObject::connect(media_object.get(), SIGNAL(stateChanged(Phonon::State, Phonon::State))
                   , media_object.get(), SLOT(stateChanged_impl(Phonon::State, Phonon::State)));

  Phonon::Path phonon_path = Phonon::createPath(media_object.get(), audio_output.get());
  if(phonon_path.isValid())
  {
    QUrl url = create_url(source, root_path);
    Phonon::MediaSource media_source(url);
    media_object->setCurrentSource(media_source);
    media_object->play();
    this->media_object = media_object.release();
    this->audio_output = audio_output.release();
  }
  else
  {
    ncl->error_occurred("Error creating audio Path");
    throw std::runtime_error("Error creating audio Path");
  }
}

void audio_player::pause()
{
  media_object->pause();
}

void audio_player::resume()
{
  media_object->play();
}

bool audio_player::start_set_property(std::string const& name, std::string const& value)
{
  if(name == "soundLevel")
  {
    sound_level_temporary = boost::lexical_cast<double>(value);
    return true;
  }
  return false;
}

bool audio_player::commit_set_property(std::string const& name)
{
  if(name == "soundLevel")
  {
    audio_output->setVolume(sound_level_temporary);
    return true;
  }
  return false;
}

void audio_player::key_process(std::string const& key, bool pressed)
{
}

bool audio_player::wants_keys() const
{
  return false; 
}

} } } }

