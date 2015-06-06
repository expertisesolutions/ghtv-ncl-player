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

#include <ghtv/qt/java/awt/component.hpp>
#include <ghtv/qt/java/component_window.hpp>

namespace ghtv { namespace qt { namespace java { namespace awt {

component::component(jvb::environment env, JavaVM* jvm)
  : window(0)
  , x_(0), y_(0), width_(0), height_(0), jvm(jvm)
  // , paint_method (self(env).class_()
  //                 .method<void(javabind::object)>
  //                 ("paint", "java/awt/Graphics"))
  , awt_graphics2d(0), graphics_peer(0)
{
  std::cout << "component::component " << this << std::endl;
  {
    jclass ex = env.raw()->FindClass("java/lang/Exception");
    env.raw()->ThrowNew(ex, "Just to debug backtrace");
    env.raw()->ExceptionDescribe();
    env.raw()->ExceptionClear();
  }
  // javabind::class_ graphics2d_impl = env.find_class("ghtv/awt/QGraphics2D");
  // javabind::object local_awt_graphics2d = graphics2d_impl
  //   .constructor<javabind::object()>()(graphics2d_impl);
  // awt_graphics2d = env.raw()->NewGlobalRef(local_awt_graphics2d.raw());  
  // graphics_peer = &javabind::get_peer<graphics2d>(local_awt_graphics2d);
}

void component::repaint()
{
  if(window)
    window->repaint();
}

void component::realize_window(component_window* w)
{
  window = w;
  w->set_awt_component(this);
  w->setVisible(visible);
  std::cout << "Realizing window " << w
            << " x, y: " << x_ << 'x' << y_
            << " w, h: " << width_ << 'x' << height_
            << " this:" << this << std::endl;
  w->move(x_, y_);
  w->resize(width_, height_);
  
}

jvb::int_ component::width() const
{
  if (window)
    width_ = window->width();
  return width_;
}
jvb::int_ component::height() const
{
  if(window != 0)
    height_ = window->height();
  return height_;
}
jvb::int_ component::get_x() const
{
  if (window != 0)
    x_ = window->x();
  return x_;
}
jvb::int_ component::get_y() const
{
  if (window != 0)
    y_ = window->y();
  return y_;
}
void component::set_x(jvb::int_ x)
{
  std::cout << "set_x " << x << std::endl;
  x_ = x;
  if (window != 0)
    window->set_x(x_);
}
void component::set_y(jvb::int_ y)
{
  std::cout << "set_y " << y << std::endl;
  y_ = y;
  if (window != 0)
    window->set_y(y_);
}
void component::set_width(jvb::int_ w)
{
  width_ = w;
  if (window != 0)
    window->set_width(width_);
}
void component::set_height(jvb::int_ h)
{
  height_ = h;
  if (window != 0)
    window->set_height(height_);
}
void component::set_visible(bool v)
{
  std::cout << "Setting visibility of " << window
            << "to " << v << std::endl;
  visible = v;
  if(window)
    window->setVisible(visible);
}
bool component::is_visible()
{
  return visible;
}
// void component::set_preferred_size(jvb::double_ w, jvb::double_ h)
// {
//   width_ = w;
//   height_ = h;
//   if(window)
//     window->set_preferred_size(w.raw(), h.raw());
// }

void component::set_delay(jvb::int_ n)
{
}
// jvb::object component::get_preferred_size(JNIEnv* e)
// {
//   jvb::environment env(e);
//   jvb::class_ dimension_class = env.find_class("java/awt/Dimension");
//   return dimension_class.constructor
//     <jvb::object(jvb::int_, jvb::int_)>
//     ()(dimension_class, width_, height_);
// }
void component::set_focus(bool v)
{
  std::cout << "set_focus " << v << std::endl;
}

void component::paint()
{
  // JNIEnv* env = 0;
  // jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), 0);
  // if(env->IsSameObject(self(env).raw(), 0))
  // {
  //   std::cout << "Object was garbage collected!" << std::endl;
  //   return;
  // }
  // jobject self_ref = env->NewLocalRef(self(env).raw());
  // assert(self_ref != 0);
  // jvb::object self_(self_ref, env);
  
  // try
  // {
  //   assert(!env->ExceptionCheck());

  //   std::cout << "Should paint " << window << std::endl;
  //   QPainter painter(window);
  //   assert(painter.isActive());
  //   QSize s = window->size();
  //   painter.eraseRect(0, 0, s.width(), s.height());

  //   jvb::object graphics(env->NewLocalRef(awt_graphics2d), env);
  //   graphics_peer->init_paint(painter);
  //   graphics_peer->translate(window->translate_x, window->translate_y);
  //   paint_method(self_, graphics);
  //   std::cout << "paint was called " << window << std::endl;
  //   graphics_peer->end_paint();
  // }
  // catch(std::exception&)
  // {
  //   std::cout << "A exception was thrown while calling paint" << std::endl;
  //   if(env->ExceptionCheck())
  //     env->ExceptionDescribe();
  // }
}


} } } }
