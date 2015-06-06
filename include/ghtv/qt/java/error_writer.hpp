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

#ifndef GHTV_QT_JAVA_ERROR_WRITER_HPP
#define GHTV_QT_JAVA_ERROR_WRITER_HPP

#include <ghtv/qt/java/java_widget.hpp>
#include <jvb/jcl/java/io/Writer.hpp>

#include <jvb/adapt_class.hpp>

namespace ghtv { namespace qt { namespace java {

JVB_ADAPT_CLASS((ghtv)(debug)(ErrorWriter)
                , (public)
                , (methods
                   (write, void(jvb::array<jvb::char_>, jvb::int_, jvb::int_), nil)
                   (close, void(), nil)(flush, void(), nil)
                  )
                  (extends jvb::jcl::java::io::Writer)
                  (constructors (ErrorWriter()))
                )

struct error_writer
{
  error_writer(java_widget* widget) : widget(widget) {}

  void write(jvb::environment e, jvb::array<jvb::char_> chars, jvb::int_ off
             , jvb::int_ len)
  {
    std::cout << "should write characters off: " << off << " len: " << len << std::endl;
    jvb::array<jvb::char_>::region_type r = chars.all(e);
    widget->reading_exception(std::string(r.raw_begin()+off, r.raw_begin()+off+len));
  }

  void close(jvb::environment) {}
  void flush(jvb::environment)
  {
    std::cout << "Flush was called" << std::endl;
  }

  java_widget* widget;
};

} } }

#endif
