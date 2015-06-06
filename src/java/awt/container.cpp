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

#include <jvb/jvb.hpp>

namespace ghtv { namespace qt { namespace java { namespace awt {

// container::container(JNIEnv* env, object_type oinfo, extends_type::info einfo)
//   : object_type(oinfo), extends_type(einfo)
// {
//   std::cout << "container::container" << std::endl;
// }

// void container::realize_container(JNIEnv* env)
// {
//   if(childrens)
//   {
//     component_window* w = base(env).window;
//     for(std::vector<jobject>::const_iterator first = childrens->begin()
//           , last = childrens->end(); first != last; ++first)
//     {
//       javabind::object other_component(*first, env);
//       component& other_component_peer = javabind::get_peer<component>
//         (other_component, "java/awt/Component");
//       other_component_peer.realize_window(new component_window(w));
//       w->add_window(other_component_peer.window);
//       container& other_container_peer = javabind::get_peer<container>
//         (other_component, "java/awt/Container");
//       other_container_peer.realize_container(env);
//       env->DeleteGlobalRef(*first);
//     }
//     childrens = boost::none;
//   }
// }

// void container::add_component(JNIEnv* env, javabind::object other_component)
// {
//   std::cout << "add_component in container" << std::endl;
//   component& other_component_peer = javabind::get_peer<component>
//     (other_component, "java/awt/Component");
//   component_window* w = base(env).window;
//   if(w && other_component_peer.window)
//   {
//     assert(!childrens);
//     w->add_window(other_component_peer.window);
//   }
//   else if(w)
//   {
//     assert(!childrens);
//     other_component_peer.realize_window(new component_window(w));
//     w->add_window(other_component_peer.window);
//     container& other_container_peer = javabind::get_peer<container>
//       (other_component, "java/awt/Container");
//     other_container_peer.realize_container(env);
//   }
//   else
//   {
//     if(!childrens)
//       childrens = std::vector<jobject>();
//     childrens->push_back(env->NewGlobalRef(other_component.raw()));
//   }
// }

// void container::remove_component(JNIEnv* env, javabind::object other_component)
// {
//   std::cout << "remove_component in container" << std::endl;
//   // component& other_component_peer = javabind::get_peer<component>
//   //   (other_component, "com/sun/dtv/lwuit/Component");
//   // component_window* window = base(env).window;
//   // if(other_component_peer.window->parentWidget() == window)
//   // {
//   //   other_component_peer.window->setVisible(false);
//   //   other_component_peer.window->setParent(0);
//   // }
// }

// void container::set_coordinate_layout(JNIEnv* env, javabind::int_ width, javabind::int_ height)
// {
//   std::cout << "container::set_coordinate_layout " << width << 'x' << height << std::endl;
//   component_window* window = base(env).window;
//   if(window)
//   {
//     assert(window != 0);
//     if(!window->absolute_coordinates)
//     {
//       std::cout << "Making absolute coordinates" << std::endl;
//       window->set_coordinate_layout(0, 0);
//     }
//   }
// }

} } } }
