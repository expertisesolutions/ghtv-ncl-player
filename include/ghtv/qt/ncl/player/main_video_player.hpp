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

#ifndef GHTV_QT_PLAYER_MAIN_VIDEO_PLAYER_HPP
#define GHTV_QT_PLAYER_MAIN_VIDEO_PLAYER_HPP

#include <ghtv/qt/ncl/player/player_base.hpp>

namespace ghtv { namespace qt { namespace ncl { namespace player {

struct main_video_player : player_base
{
  main_video_player(QWidget* player_container)
  {
  }

  void pause () {}
  void resume() {}
  void key_process(std::string const& key, bool pressed) {}
  bool wants_keys() const { return false; }
  bool start_set_property(std::string const& name, std::string const& value)
  {
    return false;
  }
  bool commit_set_property(std::string const& name) { return false; }
  void start_area(std::string const& name) {}
};

} } } }

#endif
