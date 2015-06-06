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

#ifndef GHTV_QT_PLAYER_IMAGE_PLAYER_HPP
#define GHTV_QT_PLAYER_IMAGE_PLAYER_HPP

#include <gntl/structure/composed/component_location.hpp>

#include <QLabel>
#include <QFrame>

#include <ghtv/qt/ncl/player/player_base.hpp>
#include <ghtv/qt/ncl/player/url_file.hpp>
#include <ghtv/qt/ncl/player/create_url.hpp>
#include <ghtv/qt/ncl/player/common_player.hpp>
#include <ghtv/qt/ncl/ncl_widget.hpp>

#include <memory>
#include <stdexcept>

namespace ghtv { namespace qt { namespace ncl { namespace player {

class image_player : public common_player
{
  Q_OBJECT
public:
  image_player(std::string const& source, std::string const& root_path, QWidget* player_container
               , ncl_widget* ncl, gntl::structure::composed::component_location
               <std::string, std::string> location
               , ncl::executor* executor_);

  void key_process(std::string const& key, bool pressed);
  bool wants_keys() const;

  bool start_set_property(std::string const& name, std::string const& value);
  bool commit_set_property(std::string const& name);

  void pause();
  void resume();
  void start_area(std::string const& name) {}

  QUrl url;
  ncl_widget* ncl;
  QLabel* label;
  QSize size;
  url_file* file;
public Q_SLOTS:
  void image_download();
  void download_error(std::string);
};

} } } }

#endif 

