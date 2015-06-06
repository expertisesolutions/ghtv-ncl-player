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

#ifndef GHTV_QT_JAVA_CONTAINER_HPP
#define GHTV_QT_JAVA_CONTAINER_HPP

#include <ghtv/qt/java/awt/component.hpp>

#include <jvb/adapt_class.hpp>

#include <iostream>

#include <jni.h>

namespace ghtv { namespace qt { namespace java { namespace awt {

struct container
{
  container(JNIEnv* env);
  void add_component(JNIEnv* env, jvb::object other_component_obj);
  void remove_component(JNIEnv* env, jvb::object other_component_obj);
  void set_coordinate_layout(JNIEnv* env, jvb::int_ width, jvb::int_ height);
  void realize_container(JNIEnv* env);

  boost::optional<std::vector<jobject> > childrens;
};

} } } }

#endif
