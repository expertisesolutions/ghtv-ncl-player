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

#ifndef GHTV_QT_JAVA_COMPONENT_HPP
#define GHTV_QT_JAVA_COMPONENT_HPP

#include <ghtv/qt/java/component_window.hpp>

#include <jvb/adapt_class.hpp>

#include <iostream>
#include <cassert>

#include <jni.h>

#include <QWidget>
#include <QLabel>

namespace ghtv { namespace qt { namespace java { namespace awt {

struct component
{
  component(jvb::environment, JavaVM* jvm);
  jvb::int_ width() const;
  jvb::int_ height() const;
  jvb::int_ get_x() const;
  jvb::int_ get_y() const;
  void set_x(jvb::int_ x);
  void set_y(jvb::int_ y);
  void set_width(jvb::int_ w);
  void set_height(jvb::int_ h);
  void set_visible(bool v);
  bool is_visible();
  void set_preferred_size(jvb::double_ w, jvb::double_ h);
  void set_delay(jvb::int_ n);
  jvb::object get_preferred_size(JNIEnv*);
  void set_focus(bool v);
  void realize_window(component_window* window);
  void paint();
  void repaint();

  component_window* window;
  mutable int x_, y_, width_, height_;
  mutable bool visible;
  JavaVM* jvm;
  jvb::method<void(jvb::object)> paint_method;
  jobject awt_graphics2d;
  graphics2d* graphics_peer;
};

} } } }

#endif
