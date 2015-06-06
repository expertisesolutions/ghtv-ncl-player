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

#include <ghtv/qt/java/awt/form.hpp>
#include <ghtv/qt/java/component_window.hpp>

#include <boost/bind.hpp>
#include <boost/utility.hpp>

namespace ghtv { namespace qt { namespace java { namespace awt {

form::form(JNIEnv* env, component_window* window
           , JavaVM* jvm)
  : jvm(jvm)
  // , animate_method (javabind::env(env)
  //                   .find_class("com/sun/dtv/lwuit/animations/Animation")
  //                   .method<bool()>("animate"))
{
  // std::cout << "form::form" << std::endl;
  // base(env).base(env).set_width(window->width());
  // base(env).base(env).set_height(window->height());
  // base(env).base(env).realize_window(window);
  // assert(window->parent());
  // assert(window->size() == static_cast<QWidget*>(window->parent())->size());
  // assert(window->width() != 0);
  // assert(window->height() != 0);
}

void form::show(JNIEnv* env)
{
  std::cout << "form::show" << std::endl;
  // base(env).base(env).set_visible(true);
}

void form::register_animation(JNIEnv* env, jvb::object animation)
{
  std::cout << "form::register_animation" << std::endl;
  // {
  //   jclass ex = env->FindClass("java/lang/Exception");
  //   env->ThrowNew(ex, "Just to debug backtrace");
  //   env->ExceptionDescribe();
  //   env->ExceptionClear();
  // }
  // component_window* w = base(env).base(env).window;
  // assert(w != 0);
  // if(animations.empty())
  //   w->set_animation_timer(boost::bind(&form::animation_handler, this));

  // animations.push_back(env->NewGlobalRef(animation.raw()));
}

void form::deregister_animation(JNIEnv* env, jvb::object animation)
{
  std::cout << "form::deregister_animation" << std::endl;
  // component_window* w = base(env).base(env).window;
  // assert(w != 0);
  // std::size_t i = 0;
  // while(i != animations.size())
  // {
  //   if(env->IsSameObject(animations[i], animation.raw()))
  //   {
  //     env->DeleteGlobalRef(animations[i]);
  //     animations.erase(boost::next(animations.begin(), i));
  //   }
  //   else
  //     ++i;
  // }
  // if(animations.empty())
  //   w->reset_animation_timer();
}

void form::animation_handler()
{
  std::cout << "form::animation_handler" << std::endl;
  // JNIEnv* env = 0;
  // jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), 0);
  // assert(env != 0);
  // try
  // {
  //   component_window* w = base(env).base(env).window;
  //   assert(w != 0);
  //   assert(!env->ExceptionCheck());
  //   for(std::vector<jobject>::const_iterator first = animations.begin()
  //         , last = animations.end(); first != last; ++first)
  //   {
  //     jvb::object animation(*first, env);
  //     std::cout << "Calling animate" << std::endl;
  //     if(animate_method(animation))
  //     {
  //       std::cout << "Should call paint for animation" << std::endl;
  //     }
  //   }
  //   w->update();
  // }
  // catch(std::exception const& e)
  // {
  //   std::cout << "Exception caught while calling animate" << std::endl;
  //   if(env->ExceptionCheck())
  //     env->ExceptionDescribe();
  // }
}

} } } }
