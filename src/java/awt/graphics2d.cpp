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

#include <ghtv/qt/java/awt/graphics2d.hpp>
#include <ghtv/qt/java/component_window.hpp>

#include <jvb/adapt_class.hpp>

namespace ghtv { namespace qt { namespace java { namespace awt {

graphics2d::graphics2d()
  : translate_x(0), translate_y(0), painter(0), clip(0u, 0u, 0u, 0u)
{
  std::cout << "graphics2d::graphics2d" << std::endl;
}

void graphics2d::init_paint(QPainter& p)
{
  std::cout << "Init graphics2d" << std::endl;
  painter = &p;
  reset_clip();
}

void graphics2d::end_paint()
{
  painter = 0;
}

void graphics2d::reset_clip()
{
  clip.setX(0);
  clip.setY(0);
  QRect r = painter->window();
  std::cout << "Reset clip to " << 0 << 'x' << 0 << " "
            << r.width() - r.x() << 'x' << r.height() - r.y() << std::endl;
  clip.setSize(QSize(r.width() - r.x(), r.height() - r.y()));
}

void graphics2d::set_clip(jvb::int_ x, jvb::int_ y, jvb::int_ width
                          , jvb::int_ height)
{
  assert(painter != 0);
  std::cout << "graphics2d::setClip " << x << 'x' << y << ' ' << width
            << 'x' << height << std::endl;
  clip.setX(x + translate_x);
  clip.setY(y + translate_y);
  if(x + translate_x < 0 || y + translate_y < 0)
  {
    std::cout << "CLIP-WARNING " << x + translate_x
              << 'x' << y + translate_y << std::endl;
  }
  clip.setWidth(width);
  clip.setHeight(height);
}

void graphics2d::translate(int x, int y)
{
  std::cout << "New translate for graphics2d: " << x << 'x' << y << std::endl;
  translate_x = x; translate_y = y;
}

bool graphics2d::draw_image(JNIEnv* env, jvb::object img
                            , jvb::int_ x, jvb::int_ y
                            , jvb::object observer)
{
  // assert(painter != 0);
  // std::cout << "graphics2d::draw_image " << x << 'x' << y << std::endl;
  // painter->setClipRect(clip);
  // std::cout << "Translated: " << x + translate_x << 'x' << y + translate_y
  //           << std::endl;
  // painter->drawImage(QPoint(x + translate_x, y + translate_y)
  //                    , jvb::get_peer<image>(img).qt_image);
  return true;
}

// jvb::object graphics2d::get_clip(JNIEnv* e)
// {
//   jvb::env env(e);
//   jvb::class_ rect_class = env.find_class("java/awt/Rectangle");
//   jvb::object rectangle = rect_class
//     .constructor<jvb::object(jvb::int_, jvb::int_, jvb::int_
//                                   , jvb::int_)>()(rect_class, clip.x(), clip.y()
//                                                        , clip.width(), clip.height());
//   return rectangle;
// }


void graphics2d::dispose()
{
  std::cout << "graphics2d::dispose" << std::endl;
}

void graphics2d::draw_string(JNIEnv* env, jvb::string str, jvb::int_ x
                             , jvb::int_ y)
{
  // std::cout << "(commented) graphics2d::draw_string " << str
  //           << " " << x << 'x' << y << std::endl;
  // QString string = QString::fromUtf8(str.str().c_str());
  // painter->setClipRect(clip);

  // QFont const& font = painter->font();
  // QFontMetrics metrics(font);
  // QPoint point(x + translate_x, y + translate_y + metrics.height());

  // // //QPen pen(QColor(0, 0, 0));
  // // // painter.setPen(pen);
  // // //QFont font("Helvetica", 30);
  // // // painter.setFont(font);
  // painter->drawText(point, string);
}

} } } }
