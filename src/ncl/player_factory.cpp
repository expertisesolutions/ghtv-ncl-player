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

#include <ghtv/qt/ncl/player/player_base.hpp>
#include <ghtv/qt/ncl/player/image_player.hpp>
#include <ghtv/qt/ncl/player/video_player.hpp>
#include <ghtv/qt/ncl/player/audio_player.hpp>
#include <ghtv/qt/ncl/player/html_player.hpp>
#include <ghtv/qt/ncl/player/text_player.hpp>
#include <ghtv/qt/ncl/player/ncl_player.hpp>
#include <ghtv/qt/ncl/player/main_video_player.hpp>
#include <ghtv/qt/ncl/lua/lua_player.hpp>

#include <boost/exception/all.hpp>

namespace ghtv { namespace qt { namespace ncl { namespace player {

struct source_tag {};

boost::shared_ptr<player_base> player_factory(std::string source, QWidget* player_container
                                              , std::string const& root_path
                                              , gntl::structure::composed::component_location<std::string, std::string> location
                                              , ncl_widget* ncl_window
                                              , executor* executor_, boost::optional<std::string> interface_)
{
  std::replace(source.begin(), source.end(), '\\', '/');
  if(boost::algorithm::iends_with(source, ".png")
     || boost::algorithm::iends_with(source, ".jpg")
     || boost::algorithm::iends_with(source, ".gif")
     || boost::algorithm::iends_with(source, ".bmp"))
  {
    boost::shared_ptr<player_base> player(new image_player(source, root_path, player_container
                                                           , ncl_window, location
                                                           , executor_));
    return player;
  }
  else if(boost::algorithm::iends_with (source, ".mp4")
          || boost::algorithm::iends_with (source, ".avi")
          || boost::algorithm::iends_with (source, ".mpeg")
          || boost::algorithm::iends_with (source, ".mpg")
          || boost::algorithm::iends_with (source, ".3gp"))
  {
    boost::shared_ptr<player_base> player(new video_player(source, root_path, player_container
                                                           , ncl_window, location, executor_
                                                           , interface_));
    return player;
  }
  else if(boost::algorithm::iends_with (source, ".mp3")
          || boost::algorithm::iends_with (source, ".wav"))
  {
    boost::shared_ptr<player_base> player(new audio_player(source, root_path, player_container, ncl_window
                                                           , location, executor_));
    return player;
  }
  else if(boost::algorithm::iends_with (source, ".lua"))
  {
    boost::shared_ptr<player_base> player(new lua::lua_player(source, root_path, *player_container
                                                              , ncl_window, location));
    return player;
  }
  else if(boost::algorithm::iends_with(source, ".html")
          || boost::algorithm::iends_with(source, ".htm"))
  {
    boost::shared_ptr<player_base> player(new html_player(source, root_path, player_container
                                                          , ncl_window, location, executor_));
    return player;
  }
  else if(boost::algorithm::iends_with(source, ".txt"))
  {
    boost::shared_ptr<player_base> player(new text_player(source, root_path, player_container
                                                          , ncl_window, location
                                                          , executor_));
    return player;
  }
  else if(boost::algorithm::iends_with(source, ".ncl"))
  {
    boost::shared_ptr<player_base> player(new ncl_player(source, root_path, player_container, location, executor_));
    return player;
  }
  else if(source == "sbtvd-ts://0")
  {
    boost::shared_ptr<player_base> player(new main_video_player(player_container));
    return player;
  }
  else
  {
    BOOST_THROW_EXCEPTION((::boost::enable_error_info(gntl::gntl_error())
                           << boost::error_info<source_tag, std::string>(source)));
  }
}

} } } }
