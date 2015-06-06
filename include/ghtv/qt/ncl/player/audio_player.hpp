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

#ifndef GHTV_QT_PLAYER_AUDIO_PLAYER_HPP
#define GHTV_QT_PLAYER_AUDIO_PLAYER_HPP

#include <ghtv/qt/ncl/player/common_player.hpp>
#include <ghtv/qt/ncl/player/create_url.hpp>
#include <ghtv/qt/ncl/ncl_widget.hpp>

#include <boost/lexical_cast.hpp>

#ifdef GHTV_PHONON_LOWER_CASE_DIR
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#else
#include <Phonon/AudioOutput>
#include <Phonon/MediaObject>
#endif

#include <QFrame>

#include <memory>
#include <stdexcept>

namespace ghtv { namespace qt { namespace ncl { namespace player {

class audio_player;

namespace audio_detail {

class MediaObject : public Phonon::MediaObject
{
  Q_OBJECT
public:
  MediaObject(QWidget* widget, ncl_widget* ncl, audio_player* player)
    : Phonon::MediaObject(widget), ncl(ncl)
    , player(player)
  {}

  ncl_widget* ncl;
  audio_player* player;
public Q_SLOTS:
  void stateChanged_impl(Phonon::State state, Phonon::State old);
};

}

class audio_player : public common_player
{
public:
  audio_player(std::string const& source, std::string const& root_path, QWidget* player_container
               , ncl_widget* ncl, gntl::structure::composed::component_location
               <std::string, std::string> location
               , ncl::executor* executor_);
  void pause();
  void resume();

  bool start_set_property(std::string const& name, std::string const& value);
  bool commit_set_property(std::string const& name);
  void start_area(std::string const& name) {}

  void key_process(std::string const& key, bool pressed);
  bool wants_keys() const;

  double sound_level_temporary;
  audio_detail::MediaObject* media_object;
  Phonon::AudioOutput* audio_output;
};

} } } }

#endif
