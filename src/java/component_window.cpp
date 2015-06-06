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

#include <ghtv/qt/java/component_window.hpp>
#include <ghtv/qt/java/awt/component.hpp>

#include <boost/utility.hpp>

#include <QPaintEvent>

namespace ghtv { namespace qt { namespace java {

component_window::component_window(QWidget* parent)
  : QWidget(parent), component(0)
  , animation_timer(0), translate_x(0), translate_y(0), absolute_coordinates(false)
{
  std::cout << "component_window::component_window " << parent << std::endl;
}

component_window::component_window(awt::Component parent)
  // : QWidget(parent), component(0)
  // , animation_timer(0), translate_x(0), translate_y(0), absolute_coordinates(false)
{
  std::cout << "component_window::component_window " << this << std::endl;
  // setAutoFillBackground(false);
  // border_label = new QLabel(this);
  // border_label->resize(size());
  // border_label->setStyleSheet("border: 2px solid black");
  // border_label->show();
  // show();
  // // // if(parent)
  // // // {
  // // //   std::cout << "Parent size: " << parent->width() << "x" << parent->height()
  // // //             << " "  << this << std::endl;
  // // // }
  // // javabind::env e(env);
  // // // if(parent)
  // // //   resize(parent->size());
  // // paint_id = self(env).class_().method<void(javabind::object)>
  // //   ("paint", "java/awt/Graphics").raw();
  // // assert(paint_id != 0);
  // // {
  // //   javabind::class_ animation = e.find_class("com/sun/dtv/lwuit/animations/Animation");
  // //   animate_id = animation.method<bool()>("animate").raw();
  // //   assert(animate_id != 0);
  // // }

  // // javabind::class_ graphics2d_impl = e.find_class("ghtv/awt/QGraphics2D");
  // // javabind::object local_awt_graphics2d = graphics2d_impl
  // //   .constructor<javabind::object()>()(graphics2d_impl);
  // // awt_graphics2d = env->NewGlobalRef(local_awt_graphics2d.raw());

  // // {
  // //   graphics_peer = &javabind::get_peer<graphics2d>(local_awt_graphics2d);
  // //   graphics_peer->init(this);
  // // }
}

void component_window::set_animation_timer(boost::function<void()> h)
{
  assert(!animation_handler);
  assert(animation_timer == 0);
  animation_handler = h;
  animation_timer = new QTimer(this);
  animation_timer->setInterval(33);
  QObject::connect(animation_timer, SIGNAL(timeout()), this, SLOT(animation_timeout()));
  animation_timer->start();
}

void component_window::reset_animation_timer()
{
  animation_timer->stop();
  delete animation_timer;
  animation_timer = 0;
  animation_handler = boost::function<void()>();
}

void component_window::set_awt_component(awt::component* c)
{
  component = c;
}

void component_window::resizeEvent(QResizeEvent* event)
{
  std::cout << "resized " << this << " to " << width() << 'x' << height() << std::endl;
  border_label->resize(size());
}

void component_window::paintEvent(QPaintEvent* event)
{
  std::cout << "component_window::paintEvent" << std::endl;
  if(component)
  {
    event->accept();
    std::cout << "has component, calling paint" << std::endl;
    component->paint();
  }
}

// void component_window::register_animation(JNIEnv* env, javabind::object animation)
// {
//   std::cout << "Registering animate for some animation " << this << std::endl;
//   animations.push_back(env->NewGlobalRef(animation.raw()));
//   if(!animation_timer)
//   {
//     animation_timer = new QTimer(this);
//     animation_timer->setInterval(33);
//     QObject::connect(animation_timer, SIGNAL(timeout()), this, SLOT(animation_timeout()));
//     animation_timer->start();
//     std::cout << "register_animation Setting visibility to true" << std::endl;
//   }
// }

// void component_window::deregister_animation(JNIEnv* env, javabind::object animation)
// {
//   std::cout << "De-registering animate for some animation" << std::endl;
//   std::size_t i = 0;
//   while(i != animations.size())
//   {
//     if(env->IsSameObject(animations[i], animation.raw()))
//     {
//       env->DeleteGlobalRef(animations[i]);
//       animations.erase(boost::next(animations.begin(), i));
//     }
//     else
//       ++i;
//   }
// }

// void component_window::draw_image(image const& img, int x, int y, QRect clip)
// {
//   // std::cout << "component_window::draw_image at " << x << 'x' << y
//   //           << " " << this << std::endl;
//   // QPainter painter(&paint_image);
//   // // std::cout << "clip " << clip.x() << 'x' << clip.y() << ' '
//   // //           << clip.width() << 'x' << clip.height() << std::endl;
//   // QSize s = img.qt_image.size();
//   // std::cout << "coordinates " << x << 'x' << y
//   //           << ' ' << s.width() << 'x' << s.height()
//   //           << std::endl;
//   // if(s.width() + x > clip.x() + clip.width()
//   //    || x < clip.x() || s.height() + y > clip.y() + clip.height()
//   //    || y < clip.y())
//   //   std::cout << "WARNING outside clip area" << std::endl;

//   // painter.setClipRect(clip);
//   // painter.drawImage(QPoint(x, y), img.qt_image);
// }

void component_window::draw_string(std::string const& str, int x, int y, QRect clip)
{
  // QString string = QString::fromStdString(str);
  // QPainter painter(&paint_image);
  // // painter.setClipRect(clip);
  // std::cout << "clip " << clip.x() << 'x' << clip.y() << ' '
  //           << clip.width() << 'x' << clip.height() << std::endl;
  // std::cout << "coordinates " << x << 'x' << y << std::endl;

  // QFont const& font = painter.font();
  // QFontMetrics metrics(font);
  // QPoint point(x, y + metrics.height());

  // //QPen pen(QColor(0, 0, 0));
  // // painter.setPen(pen);
  // //QFont font("Helvetica", 30);
  // // painter.setFont(font);
  // painter.drawText(point, string);
}

void component_window::set_preferred_size(awt::Dimension d)
{
  // std::cout << "Set preferred size " << this << ' ' << w << 'x' << h << std::endl;
  // resize(w, h);
}

// void component_window::add_window(component_window* other_window)
// {
//   std::cout << "component_window::add_window " << other_window
//             << " to " << this << std::endl;
//   setVisible(true);
//   std::cout << "is visible? " << isVisible() << std::endl;
//   other_window->setParent(this);
//   if(absolute_coordinates)
//   {
//     std::cout << "absolute_coordinates" << std::endl;
//     QPoint pos = other_window->pos();
//     other_window->set_coordinate_layout(translate_x - pos.x()
//                                         , translate_y - pos.y());
//     std::cout << "Coordinate " << (translate_x - pos.x())
//               << 'x' << (translate_y - pos.y()) << std::endl;
//     std::cout << "size: " << other_window->width() << 'x' << other_window->height()
//               << std::endl;
//   }
//   else
//     std::cout << "NOT absolute_coordinates" << std::endl;
//   other_window->setVisible(isVisible());
// }

void component_window::set_x(jvb::int_ x)
{
  // if(absolute_coordinates)
  //   set_coordinate_layout(translate_x + this->x() - x, translate_y);
  move(x, this->y());
}
void component_window::set_y(jvb::int_ y)
{
  // if(absolute_coordinates)
  //   set_coordinate_layout(translate_x, translate_y + this->y() - y);
  move(this->x(), y);
}

void component_window::set_width(jvb::int_ w)
{
  std::cout << "component_window::set_width " << w << std::endl;
  resize(w, this->height());
}
void component_window::set_height(jvb::int_ h)
{
  std::cout << "component_window::set_height " << h << std::endl;
  resize(width(), h);
}

// void component_window::set_coordinate_layout(awt::Dimension d)
// {
//   std::cout << "set_coordinate_layout translate_x: " << x
//             << " translate_y: " << y << " this: " << this << std::endl;
//   translate_x = x; translate_y = y;
//   absolute_coordinates = true;

//   QObjectList childrens = children();
//   for(QObjectList::iterator first = childrens.begin()
//         , last = childrens.end(); first != last; ++first)
//   {
//     if(component_window* p = dynamic_cast<component_window*>(*first))
//     {
//       QPoint pos = p->pos();
//       p->set_coordinate_layout(x - pos.x(), y - pos.y());
//     }
//   }
// }

void component_window::animation_timeout()
{
  animation_handler();
}

} } }
