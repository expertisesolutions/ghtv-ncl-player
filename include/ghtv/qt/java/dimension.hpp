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

#ifndef GHTV_QT_JAVA_DIMENSION_HPP
#define GHTV_QT_JAVA_DIMENSION_HPP

#include <jvb/adapt_class.hpp>

namespace ghtv { namespace qt { namespace java { namespace awt {

JVB_ADAPT_CLASS((java)(awt)(Dimension)
                , (public)
                , (methods
                   (getWidth, jvb::double_(),  nil)
                   (getHeight, jvb::double_(),  nil)
                   (getSize, Dimension(),  nil)
                   (equals, bool(jvb::Object),  nil)
                   (hashCode, jvb::int_(),  nil)
                   (toString, jvb::String(),  nil)
                  ) // methods
                  (overloads
                   (setSize,
                    (void(jvb::double_, jvb::double_))
                    (void(Dimension))
                    (void(jvb::int_, jvb::int_))
                   ) // setSize
                  ) // overloads
                )

}

struct dimension
{
  
};

} } }


#endif
