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

#ifndef GHTV_QT_PLAYER_NCL_PLAYER_HPP
#define GHTV_QT_PLAYER_NCL_PLAYER_HPP

#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/player/common_player.hpp>

#include <QString>

namespace ghtv { namespace qt { namespace ncl { namespace player {

class ncl_player : public common_player
{
  Q_OBJECT;
public:
  ncl_player(std::string const& source, std::string const& root_path, QWidget* player_container
             , gntl::structure::composed::component_location
               <std::string, std::string> location
             , ncl::executor* e)
    : common_player(e, location), ncl(new ncl_widget)
  {
    ncl->setParent(player_container);
    ncl->move(QPoint(0, 0));
    ncl->resize(player_container->size());
    ncl->show();
    QString path (root_path.c_str());
    path += "/";
    path += source.c_str();
    ncl->start_ncl(path);
    setup_timers();
  }

  void pause() {}
  void resume() {}

  void key_process(std::string const& key, bool pressed) {}
  
  bool wants_keys() const { return false;  }
  bool start_set_property(std::string const& name, std::string const& value)
  {
    return false;
  }
  bool commit_set_property(std::string const& name)
  {
    return false;
  }
  void start_area(std::string const& name) {}

  ncl_widget* ncl;
};

} } } }

#endif
