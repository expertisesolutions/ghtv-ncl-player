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

#ifndef GHTV_QT_JAVA_FORM_HPP
#define GHTV_QT_JAVA_FORM_HPP

#include <ghtv/qt/java/awt/container.hpp>

#include <jvb/adapt_class.hpp>

#include <iostream>

#include <jni.h>

namespace ghtv { namespace qt { namespace java { namespace awt {

struct form
{
  form(JNIEnv* env, component_window* window, JavaVM* jvm);
  void show(JNIEnv* env);
  void register_animation(JNIEnv* env, jvb::object animation);
  void deregister_animation(JNIEnv* env, jvb::object animation);

  void animation_handler();

  JavaVM* jvm;
  jvb::method<bool()> animate_method;
  std::vector<jobject> animations;
};

} } } }

#endif
