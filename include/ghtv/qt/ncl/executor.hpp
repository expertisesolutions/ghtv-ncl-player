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

#ifndef GHTV_QT_EXECUTOR_HPP
#define GHTV_QT_EXECUTOR_HPP

#include <ghtv/qt/ncl/player/player_base.hpp>

#include <gntl/concept/structure/executor.hpp>
#include <gntl/structure/composed/component_location.hpp>
#include <gntl/algorithm/structure/media/dimensions.hpp>
#include <gntl/parser/libxml2/dom/xml_string.hpp>
#include <gntl/parser/libxml2/dom/color.hpp>

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/version.hpp>
#include <boost/mpl/vector.hpp>
#if BOOST_VERSION >= 103800
#include <boost/algorithm/string.hpp>
#endif
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/noncopyable.hpp>

#include <QWidget>
#include <QDir>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QFrame>
#include <QString>

namespace ghtv { namespace qt { namespace ncl {

struct ncl_widget;

struct executor
{
  typedef gntl::parser::libxml2::dom::xml_string<> component_identifier;

  executor(ncl_widget* root_window
           , std::string root_path
           , std::map<int, std::vector<QWidget*> >& zindex_map)
    : root_window(root_window)
    , root_path(root_path)
    , zindex_map(&zindex_map)
    , border_size(0)
    , outside_border(false)
    , explicit_duration(boost::posix_time::not_a_date_time)
  {
  }

  ~executor()
  {
    stop();
  }

  void stop()
  {
    for(std::map<int, std::vector<QWidget*> >::iterator
          first = zindex_map->begin()
          , last = zindex_map->end()
          ;first != last;)
    {
      std::vector<QWidget*>::iterator i
        = std::find(first->second.begin()
                    , first->second.end()
                    , frame.get());
      if(i != first->second.end())
        first->second.erase(i);
      
      if(first->second.empty())
        zindex_map->erase(first++);
      else
        ++first;
    }

    if(player)
      player.reset();

    player_container = 0;
    frame.reset();
  }

  void key_process(std::string key, bool);

  ncl_widget* root_window;
  std::string root_path;
  boost::shared_ptr<qt::ncl::player::player_base> player;
  QWidget* player_container;
  boost::shared_ptr<QFrame> frame; // owns child-windows
  std::map<int, std::vector<QWidget*> >* zindex_map;
  gntl::algorithm::structure::media::dimensions dimensions;
  std::string source;
  std::size_t border_size;
  bool outside_border;
  gntl::parser::libxml2::dom::color border_color;
  boost::optional<boost::variant<unsigned int, double> > top, left, width, height, right, bottom;
  struct time_area
  {
    component_identifier interface_;
    boost::posix_time::time_duration begin, end;
  };
  std::vector<time_area> time_areas;
  boost::posix_time::time_duration explicit_duration;
};

} } }

namespace gntl { namespace concept { namespace structure {

template <>
struct executor_traits<ghtv::qt::ncl::executor>
{
  typedef boost::mpl::true_ is_executor;
  typedef ghtv::qt::ncl::executor executor;
  typedef boost::posix_time::time_duration time_duration;
  typedef gntl::parser::libxml2::dom::color color_type;
  typedef gntl::parser::libxml2::dom::xml_string<> component_identifier;

  typedef boost::mpl::vector
  <void(executor, gntl::parser::libxml2::dom::xml_string<>
        , boost::optional<std::string>
        , gntl::algorithm::structure::media::dimensions
        , gntl::structure::composed::component_location<std::string, std::string>)> start_function_overloads;
  typedef boost::mpl::vector
  <void(executor)> stop_function_overloads;
  typedef boost::mpl::vector
  <void(executor)> pause_function_overloads;
  typedef boost::mpl::vector
  <void(executor)> resume_function_overloads;
  typedef boost::mpl::vector
  <void(executor)> abort_function_overloads;

  static void add_border (executor& e, int border_width, color_type color);
  static void remove_border (executor& e);

  static void start(executor& e, gntl::parser::libxml2::dom::xml_string<> source
                    , boost::optional<std::string> interface_
                    , gntl::algorithm::structure::media::dimensions dim
                    , gntl::structure::composed::component_location<std::string, std::string> location);
  static void abort(executor& e);
  static void stop(executor& e);
  static void pause(executor& e);
  static void resume(executor& e);

  static void area_time_begin(executor& e, component_identifier i, time_duration begin_time);
  static void area_time_end(executor& e, component_identifier i, time_duration end_time);
  static void area_time_begin_end(executor& e, component_identifier i, time_duration begin_time
                                  , time_duration end_time);
  static void area_frame_begin(executor& e, component_identifier i, unsigned int frame);
  static void area_frame_end(executor& e, component_identifier i, unsigned int frame);
  static void area_frame_begin_end(executor& e, component_identifier i, unsigned int begin_frame
                                   , unsigned int end_frame);
  static void area_npt_begin(executor& e, component_identifier i, int npt);
  static void area_npt_end(executor& e, component_identifier i, int npt);
  static void area_npt_begin_end(executor& e, component_identifier i, int begin_npt, int end_npt);
  static void start_area(executor& e, component_identifier area);
  static bool start_set_property(executor& e, std::string property_name, std::string property_value);
  static bool start_set_property(executor& e, std::string property_name, int property_value);
  static bool commit_set_property(executor& e, std::string property_name);
  static bool wants_keys(executor const& e);
  static void explicit_duration(executor& e, time_duration explicit_duration);
};

} } }

#endif
