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

#ifndef GHTV_QT_PLAYER_VIDEO_PLAYER_HPP
#define GHTV_QT_PLAYER_VIDEO_PLAYER_HPP

#include <QFrame>
#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/executor.hpp>
#include <ghtv/qt/ncl/player/common_player.hpp>
#include <ghtv/qt/ncl/player/create_url.hpp>

#ifdef GHTV_PHONON_LOWER_CASE_DIR
#include <phonon/videoplayer.h>
#include <phonon/mediaobject.h>
#else
#include <Phonon/VideoPlayer>
#include <Phonon/MediaObject>
#endif

namespace ghtv { namespace qt { namespace ncl { namespace player {

class video_player : public common_player
{
  Q_OBJECT
public:
  video_player(std::string const& source, std::string const& root_path, QWidget* player_container
               , ncl_widget* root_window, gntl::structure::composed::component_location
               <std::string, std::string> location
               , ncl::executor* e
               , boost::optional<std::string> const& interface_);

  ~video_player();

  void pause();
  void resume();

  void key_process(std::string const& key, bool pressed);
  bool wants_keys() const;

  bool start_set_property(std::string const& name, std::string const& value);
  bool commit_set_property(std::string const& name);
  void start_area(std::string const& name) {}

  ncl_widget* ncl_window;
  Phonon::VideoPlayer* video_player_; // no direct ownership. Frame owns it
  gntl::structure::composed::component_location<std::string, std::string> location;
  boost::posix_time::time_duration seek;

public Q_SLOTS:
  void has_video_changed(bool);
  void video_state_changed(Phonon::State new_state, Phonon::State old_state);
};

} } } } 

#endif
