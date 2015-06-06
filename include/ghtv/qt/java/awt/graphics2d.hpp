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

#ifndef GHTV_QT_JAVA_GRAPHICS2D_HPP
#define GHTV_QT_JAVA_GRAPHICS2D_HPP

#include <jvb/adapt_class.hpp>

#include <QRect>
#include <QWidget>

namespace ghtv { namespace qt { namespace java { namespace awt {

struct graphics2d
{
  graphics2d();

  void init_paint(QPainter& painter);
  void end_paint();

  void reset_clip();
  void set_clip(jvb::int_ x, jvb::int_ y, jvb::int_ width
                , jvb::int_ height);
  jvb::object get_clip(JNIEnv* env);
  bool draw_image(JNIEnv* env, jvb::object img
                  , jvb::int_ x, jvb::int_ y
                  , jvb::object observer);
  void draw_string(JNIEnv* env, jvb::string str, jvb::int_ x, jvb::int_ y);
  void dispose();
  void translate(int x, int y);

  int translate_x, translate_y;
  QPainter* painter;
  QRect clip;
};

} } } }

#endif

