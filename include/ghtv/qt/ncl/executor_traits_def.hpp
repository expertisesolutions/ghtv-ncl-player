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

#ifndef GHTV_QT_EXECUTOR_TRAITS_DEF_HPP
#define GHTV_QT_EXECUTOR_TRAITS_DEF_HPP

#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/lua/lua_player.hpp>
#include <ghtv/qt/ncl/executor.hpp>
#include <ghtv/qt/ncl/player/player_factory.hpp>
#include <ghtv/qt/ncl/executor_widget.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/home/phoenix.hpp>

#include <QImage>

namespace ghtv { namespace qt { namespace ncl {

inline void executor::key_process(std::string key, bool pressed)
{
  if(player)
    player->key_process(key, pressed);
}

} } }

namespace gntl { namespace concept { namespace structure {

inline void executor_traits<ghtv::qt::ncl::executor>::add_border
 (executor& e, int border_width, color_type color)
{
  if(border_width < 0)
    e.outside_border = false;
  else
    e.outside_border = true;
  e.border_size = border_width;
  e.border_color = color;
  if(e.frame)
  {
    assert(e.player_container != 0);
    e.frame->setLineWidth(border_width);
    if(!e.outside_border)
    {
      e.frame->setGeometry(e.dimensions.x, e.dimensions.y, e.dimensions.width, e.dimensions.height);
      e.player_container->setGeometry(e.border_size, e.border_size, e.dimensions.width-2*e.border_size
                                      , e.dimensions.height-2*e.border_size);
    }
    else
    {
      std::size_t x = e.dimensions.x, y = e.dimensions.y;
      if(x > e.border_size)
        x -= e.border_size;
      if(y > e.border_size)
        y -= e.border_size;
      e.frame->setGeometry(x, y, e.dimensions.width + 2*e.border_size, e.dimensions.height + 2*e.border_size);
      e.player_container->setGeometry(e.border_size, e.border_size, e.dimensions.width-2*e.border_size
                                      , e.dimensions.height-2*e.border_size);
    }
  }
}

inline void executor_traits<ghtv::qt::ncl::executor>::remove_border (executor& e)
{
  if(e.frame)
    e.frame->setLineWidth(0);
  e.border_size = 0;
}

inline void executor_traits<ghtv::qt::ncl::executor>::start
(executor& e, gntl::parser::libxml2::dom::xml_string<> source
 , boost::optional<std::string> interface_
 , gntl::algorithm::structure::media::dimensions dim
 , gntl::structure::composed::component_location<std::string, std::string> location)
{
  e.dimensions = dim;
  if(!e.player)
  {
    e.frame.reset(new QFrame(e.root_window));

    if(e.top)
    {
      if(unsigned int* p = boost::get<unsigned int>(&*e.top))
      {
        if(dim.height < *p)
        {
          dim.x += dim.height;
          dim.height = 0;
        }
        else
        {
          dim.x += *p;
          dim.height -= *p;
        }
      }
      else if(double* p = boost::get<double>(&*e.top))
      {
        if(*p >= 100.0)
        {
          dim.x += dim.height;
          dim.height = 0;
        }
        else
        {
          dim.x += dim.height*(*p/100);
          dim.height *= 1 - (*p/100);
        }
      }
    }

    if(e.left)
    {
      if(unsigned int* p = boost::get<unsigned int>(&*e.left))
      {
        if(dim.width < *p)
        {
          dim.x += dim.width;
          dim.width = 0;
        }
        else
        {
          dim.x += *p;
          dim.width -= *p;
        }
      }
      else if(double* p = boost::get<double>(&*e.left))
      {
        if(*p >= 100.0)
        {
          dim.x += dim.width;
          dim.width = 0;
        }
        else
        {
          dim.x += dim.width*(*p/100);
          dim.width *= 1 - (*p/100);
        }
      }
    }

    if(e.width)
    {
      if(unsigned int* p = boost::get<unsigned int>(&*e.width))
      {
        if(*p < dim.width)
          dim.width = *p;
      }
      else if(double* p = boost::get<double>(&*e.width))
      {
        if(*p < 100.0)
          dim.width *= *p/100;
      }
    }
    else if(e.right)
    {
      if(unsigned int* p = boost::get<unsigned int>(&*e.right))
      {
        if(*p < dim.width)
          dim.width -= *p;
        else
          dim.width = 0;
      }
      else if(double* p = boost::get<double>(&*e.right))
      {
        if(*p <= 100.0)
          dim.width *= 1-(*p/100);
      }
    }

    if(e.height)
    {
      if(unsigned int* p = boost::get<unsigned int>(&*e.height))
      {
        if(*p < dim.height)
          dim.height = *p;
      }
      else if(double* p = boost::get<double>(&*e.height))
      {
        if(*p < 100.0)
          dim.height *= *p/100;
      }
    }
    else if(e.bottom)
    {
      if(unsigned int* p = boost::get<unsigned int>(&*e.bottom))
      {
        if(*p < dim.height)
          dim.height -= *p;
        else
          dim.height = 0;
      }
      else if(double* p = boost::get<double>(&*e.bottom))
      {
        if(*p <= 100.0)
          dim.height *= 1-(*p/100);
      }
    }

    if(!e.outside_border)
    {
      e.frame->setGeometry(dim.x, dim.y, dim.width, dim.height);
      e.player_container = new ghtv::qt::ncl::executor_widget(&*e.frame, e);
      e.player_container->setGeometry(e.border_size, e.border_size, dim.width-2*e.border_size
                                      , dim.height-2*e.border_size);
    }
    else
    {
      std::size_t x = dim.x, y = dim.y;
      if(x > e.border_size)
        x -= e.border_size;
      if(y > e.border_size)
        y -= e.border_size;
      e.frame->setGeometry(x, y, dim.width + 2*e.border_size, dim.height + 2*e.border_size);
      e.player_container = new ghtv::qt::ncl::executor_widget(&*e.frame, e);
      e.player_container->setGeometry(e.border_size, e.border_size, dim.width-2*e.border_size
                                      , dim.height-2*e.border_size);
    }

    e.frame->show();
    e.player_container->show();

    std::string new_source = source;
    e.player = ghtv::qt::ncl::player::player_factory
      (new_source, e.player_container, e.root_path
       , location, e.root_window, &e, interface_);

    QRect frame_rect(0, 0, dim.width - dim.x, dim.height - dim.y);
    e.frame->setLineWidth(e.border_size);
    e.frame->setFrameRect(frame_rect);
    e.frame->setFrameShape(QFrame::Box);
    e.root_window->idle_register();

    if(e.frame.get())
    {
      std::vector<QWidget*>& zindex_vec = (*e.zindex_map)[dim.zindex];
      {
        std::map<int, std::vector<QWidget*> >::const_iterator
          iterator = e.zindex_map->upper_bound(dim.zindex);
        if(iterator != e.zindex_map->end())
        {
          std::vector<QWidget*>const& next_zindex_vec = iterator->second;
          assert(!next_zindex_vec.empty());
          e.frame->stackUnder(next_zindex_vec.front());
        }
        else
        {
          e.frame->raise();
        }
      }
      zindex_vec.push_back(e.frame.get());
      std::vector<QWidget*>::reverse_iterator widget_iterator = zindex_vec.rbegin()
        , referential_iterator = widget_iterator
        , last_widget_iterator = zindex_vec.rend();
      ++widget_iterator;
      while(widget_iterator != last_widget_iterator)
      {
        (*widget_iterator)->stackUnder(*referential_iterator);
        ++widget_iterator;
        ++referential_iterator;
      }
    }
    e.source = source;
  }
}

inline void executor_traits<ghtv::qt::ncl::executor>::abort(executor& e)
{
  e.stop();
}

inline void executor_traits<ghtv::qt::ncl::executor>::stop(executor& e)
{
  e.stop();
}

inline void executor_traits<ghtv::qt::ncl::executor>::pause(executor& e)
{
  if(e.player)
    e.player->pause();
}

inline void executor_traits<ghtv::qt::ncl::executor>::resume(executor& e)
{
  if(e.player)
    e.player->resume();
}

inline void executor_traits<ghtv::qt::ncl::executor>::area_time_begin
 (executor& e, component_identifier i, time_duration begin_time)
{
  executor::time_area a = {i, begin_time, boost::posix_time::not_a_date_time};
  e.time_areas.push_back(a);
}

inline void executor_traits<ghtv::qt::ncl::executor>::area_time_end
 (executor& e, component_identifier i, time_duration end_time)
{
  executor::time_area a = {i, boost::posix_time::not_a_date_time, end_time};
  e.time_areas.push_back(a);
}

inline void executor_traits<ghtv::qt::ncl::executor>::area_time_begin_end
 (executor& e, component_identifier i, time_duration begin_time
  , time_duration end_time)
{
  executor::time_area a = {i, begin_time, end_time};
  e.time_areas.push_back(a);
}

inline void executor_traits<ghtv::qt::ncl::executor>::area_frame_begin
  (executor& e, component_identifier i, unsigned int frame)
{
}

inline void executor_traits<ghtv::qt::ncl::executor>::area_frame_end
  (executor& e, component_identifier i, unsigned int frame)
{
}

inline void executor_traits<ghtv::qt::ncl::executor>::area_frame_begin_end
  (executor& e, component_identifier i, unsigned int begin_frame
   , unsigned int end_frame)
{
}

inline void executor_traits<ghtv::qt::ncl::executor>::area_npt_begin
  (executor& e, component_identifier i, int npt)
{
}

inline void executor_traits<ghtv::qt::ncl::executor>::area_npt_end
  (executor& e, component_identifier i, int npt)
{
}

inline void executor_traits<ghtv::qt::ncl::executor>::area_npt_begin_end
  (executor& e, component_identifier i, int begin_npt, int end_npt)
{
}

inline void executor_traits<ghtv::qt::ncl::executor>::start_area
  (executor& e, component_identifier area)
{
  if(e.player)
    e.player->start_area(area);
}

namespace parser_detail {

namespace spirit {
#if BOOST_VERSION >= 104200
    namespace arg_names = boost::spirit::qi;
#else
    namespace arg_names = boost::spirit::arg_names;
#endif
}

struct coordinates
{
  boost::variant<unsigned int, double> top, left, width, height;
};

typedef std::pair<boost::variant<unsigned int, double>, boost::variant<unsigned int, double> > coordinate_pair;

inline boost::optional<boost::variant<unsigned int, double> > parse_coordinate(std::string const& value)
{
  namespace qi = boost::spirit::qi;
  namespace spirit = boost::spirit;
  namespace arg_names = parser_detail::spirit::arg_names;
  namespace phoenix = boost::phoenix;
  boost::variant<unsigned int, double> c;
  std::string::const_iterator iterator = value.begin();
  if(qi::phrase_parse (iterator, value.end ()
                , ((spirit::lexeme[(spirit::double_[phoenix::ref (c) = arg_names::_1] >> '%')])
                   | spirit::uint_[phoenix::ref (c) = arg_names::_1])
                , spirit::ascii::space)
     && iterator == value.end())
    return c;
  else
    return boost::none;  
}

inline boost::optional<coordinate_pair> parse_coordinate_pair(std::string const& value)
{
  namespace qi = boost::spirit::qi;
  namespace spirit = boost::spirit;
  namespace arg_names = parser_detail::spirit::arg_names;
  namespace phoenix = boost::phoenix;
  coordinate_pair c (0u, 0u);
  std::string::const_iterator iterator = value.begin();
  if(qi::phrase_parse (iterator, value.end ()
                ,
                ((spirit::lexeme[(spirit::double_[phoenix::ref (c.first) = arg_names::_1] >> '%')])
                 | spirit::uint_[phoenix::ref (c.first) = arg_names::_1])
                >> ","
                >> 
                ((spirit::lexeme[spirit::double_[phoenix::ref (c.second) = arg_names::_1] >> '%'])
                 | spirit::uint_[phoenix::ref (c.second) = arg_names::_1])
                , spirit::ascii::space)
     && iterator == value.end())
    return c;
  else
    return boost::none;
}

inline boost::optional<coordinates> parse_bounds(std::string const& value)
{
  namespace qi = boost::spirit::qi;
  namespace spirit = boost::spirit;
  namespace arg_names = parser_detail::spirit::arg_names;
  namespace phoenix = boost::phoenix;
  coordinates c = {0u, 0u, 0u, 0u};
  std::string::const_iterator iterator = value.begin();
  if(qi::phrase_parse (iterator, value.end ()
                ,
                ((spirit::lexeme[(spirit::double_[phoenix::ref (c.top) = arg_names::_1] >> '%')])
                 | spirit::uint_[phoenix::ref (c.top) = arg_names::_1])
                >> ","
                >> 
                ((spirit::lexeme[spirit::double_[phoenix::ref (c.left) = arg_names::_1] >> '%'])
                 | spirit::uint_[phoenix::ref (c.left) = arg_names::_1])
                >> ","
                >> 
                ((spirit::lexeme[spirit::double_[phoenix::ref (c.width) = arg_names::_1] >> '%'])
                 | spirit::uint_[phoenix::ref (c.width) = arg_names::_1])
                >> ","
                >> 
                ((spirit::lexeme[spirit::double_[phoenix::ref (c.height) = arg_names::_1] >> '%'])
                 | spirit::uint_[phoenix::ref (c.height) = arg_names::_1])
                , spirit::ascii::space)
     && iterator == value.end())
    return c;
  else
    return boost::none;
}

}

inline bool executor_traits<ghtv::qt::ncl::executor>::start_set_property
  (executor& e, std::string property_name, std::string property_value)
{
  if(!e.player)
  {
    if(property_name == "bounds")
    {
      std::cout << "parsing bounds" << std::endl;
      if(boost::optional<parser_detail::coordinates> c = parser_detail::parse_bounds(property_value))
      {
        std::cout << "parsed bounds" << std::endl;
        e.top = c->top;
        e.left = c->left;
        e.width = c->width;
        e.height = c->height;
        return true;
      }
    }
    if(property_name == "location")
    {
      if(boost::optional<parser_detail::coordinate_pair> c = parser_detail::parse_coordinate_pair(property_value))
      {
        e.top = c->first;
        e.left = c->second;
        return true;
      }
    }
    if(property_name == "size")
    {
      if(boost::optional<parser_detail::coordinate_pair> c = parser_detail::parse_coordinate_pair(property_value))
      {
        e.width = c->first;
        e.height = c->second;
        return true;
      }
    }
    else if(property_name == "top")
    {
      if(boost::optional<boost::variant<unsigned int, double> > 
         c = parser_detail::parse_coordinate(property_value))
      {
        e.top = *c;
        return true;
      }
    }
    else if(property_name == "left")
    {
      if(boost::optional<boost::variant<unsigned int, double> > 
         c = parser_detail::parse_coordinate(property_value))
      {
        e.left = *c;
        return true;
      }
    }
    else if(property_name == "width")
    {
      if(boost::optional<boost::variant<unsigned int, double> > 
         c = parser_detail::parse_coordinate(property_value))
      {
        e.width = *c;
        return true;
      }
    }
    else if(property_name == "height")
    {
      if(boost::optional<boost::variant<unsigned int, double> > 
         c = parser_detail::parse_coordinate(property_value))
      {
        e.height = *c;
        return true;
      }
    }
    else if(property_name == "right")
    {
      if(boost::optional<boost::variant<unsigned int, double> > 
         c = parser_detail::parse_coordinate(property_value))
      {
        e.right = *c;
        return true;
      }
    }
    else if(property_name == "bottom")
    {
      if(boost::optional<boost::variant<unsigned int, double> > 
         c = parser_detail::parse_coordinate(property_value))
      {
        e.bottom = *c;
        return true;
      }
    }
    return false;
  }
  else
    return e.player->start_set_property(property_name, property_value);
}

inline bool executor_traits<ghtv::qt::ncl::executor>::start_set_property
  (executor& e, std::string property_name, int property_value)
{
  // TODO implement
  return false;
}

inline bool executor_traits<ghtv::qt::ncl::executor>::commit_set_property
  (executor& e, std::string property_name)
{
  if(!e.player)
  {
    if(property_name == "bounds" || property_name == "width" || property_name == "height"
       || property_name == "top" || property_name == "left" || property_name == "bottom"
       || property_name == "right" || property_name == "size" || property_name == "location")
      return true;
    return false;
  }
  else
    return e.player->commit_set_property(property_name);
}

inline bool executor_traits<ghtv::qt::ncl::executor>::wants_keys(executor const& e)
{
  return e.player && e.player->wants_keys();
}

inline void executor_traits<ghtv::qt::ncl::executor>::explicit_duration
  (executor& e, time_duration explicit_duration)
{
  e.explicit_duration = explicit_duration;
}

} } }

#endif
