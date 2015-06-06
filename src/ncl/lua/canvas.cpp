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

#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/lua/lua_player.hpp>
#include <ghtv/qt/ncl/player/create_url.hpp>
#include <ghtv/qt/ncl/player/url_file.hpp>

#include <luabind/out_value_policy.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/integer.hpp>

#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QMutexLocker>
#include <QCoreApplication>

namespace ghtv { namespace qt { namespace ncl { namespace lua {

namespace {

struct canvas
{
  boost::shared_ptr<QImage> image;
  bool root;
  lua_player* player;
  typedef boost::uint_t<8>::least color_channel_type;
  color_channel_type color_red, color_green, color_blue, color_alpha;

  canvas()
    : root(false), player(0)
    , color_red(0u), color_green(0u), color_blue(0u), color_alpha(255u)
    , scale_width(0), scale_height(0), clip_x(0), clip_y(0), clip_w(0)
    , clip_h(0)
  {}
  canvas(boost::shared_ptr<QImage> image, lua_player* player, bool root = false)
    : image(image), root(root), player(player), color_red(0u), color_green(0u)
    , color_blue(0u), color_alpha(255u), scale_width(0), scale_height(0)
    , clip_x(0), clip_y(0), clip_w(0), clip_h(0), crop_x(0), crop_y(0)
  {
    QSize size = image->size();
    crop_w = clip_w = size.width();
    crop_h = clip_h = size.height();
  }

  std::size_t scale_width, scale_height;
  std::size_t clip_x, clip_y, clip_w, clip_h;
  std::size_t crop_x, crop_y, crop_w, crop_h;

  QFont font;

  canvas canvas_image_new (std::string image_path)
  {
    QUrl url = ncl::player::create_url (image_path, player->lua_path);
    player::url_file file (url, player);

    boost::shared_ptr<QImage> image (new QImage);
    if(file.local())
      image->load(file.io_device (), "");
    return canvas (image, player);
  }
  canvas canvas_buffer_new (int w, int h)
  {
    boost::shared_ptr<QImage> image (new QImage(QSize(w, h), QImage::Format_ARGB32));
    image->fill(0);

    return canvas (image, player);
  }
  void attrSize(int& width, int& height)
  {
    QMutex* m = 0;
    if(root)
      m = &player->window->image_mutex;
    QMutexLocker locker(m);
    QSize size = image->size();
    width = size.width(); height = size.height();
  }
  void attrColor(int red, int green, int blue, int alpha)
  {
    color_red = red; color_green = green; color_blue = blue; color_alpha = alpha;
  }
  void attrColor_string(std::string color)
  {
    if(color == "white")
      attrColor(255u, 255u, 255u, 255u);
    else if(color == "black")
      attrColor(0u, 0u, 0u, 255u);
    else if(color == "silver")
      attrColor(192u,192u,192u, 255u);
    else if(color == "gray")
      attrColor(128u,128u,128u, 255u);
    else if(color == "red")
      attrColor(255u,0u,0u, 255u);
    else if(color == "maroon")
      attrColor(128u,0u,0u, 255u);
    else if(color == "fuchsia")
      attrColor(255u,0u,255u, 255u);
    else if(color == "purple")
      attrColor(128u,0u,128u, 255u);
    else if(color == "lime")
      attrColor(0u,255u,0u, 255u);
    else if(color == "green")
      attrColor(0u,128u,0u, 255u);
    else if(color == "yellow")
      attrColor(255u,255u,0u, 255u);
    else if(color == "olive")
      attrColor(128u,128u,0u, 255u);
    else if(color == "blue")
      attrColor(0u,0u,255u, 255u);
    else if(color == "navy")
      attrColor(0u,0u,128u, 255u);
    else if(color == "aqua")
      attrColor(0u,255u,255u, 255u);
    else if(color == "teal")
      attrColor(0u,128u,128u, 255u);
  }
  void get_attrColor(int& red, int&green, int& blue, int& alpha)
  {
    red = color_red; green = color_green; blue = color_blue; alpha = color_alpha;
  }
  void attrFont(std::string face, int size, std::string style)
  {
    font = QFont(QString(face.c_str()), size);
  }
  void attrFont_non_normative(std::string face, int size)
  {
    player->ncl_window->error_occurred("attrFont has 3 arguments and they are all required.");
    font = QFont(QString(face.c_str()), size);
  }
  void get_attrFont(lua_State* L)
  {
    lua_pushstring(L, "string");
    lua_pushnumber(L, 10);
    lua_pushstring(L, "anotherstring");
  }
  void attrClip(int x, int y, int w, int h)
  {
    clip_x = x; clip_y = y; clip_w = w; clip_h = h;
  }
  void get_attrClip(int& x, int& y, int& w, int& h)
  {
    x = clip_x; y = clip_y; w = clip_w; h = clip_h;
  }
  void attrCrop (int x, int y, int w, int h)
  {
    crop_x = x; crop_y = y; crop_w = w; crop_h = h;
  }
  void get_attrCrop (int& x, int& y, int& w, int& h)
  {
    x = crop_x; y = crop_y; w = crop_w; h = crop_h;
  }
  void attrFlip(bool h, bool v)
  {
    // TODO:
  }
  void get_attrFlip(bool& h, bool& v)
  {
    // TODO:
  }
  void attrOpacity(int alpha)
  {
    // TODO:
  }
  int get_attrOpacity()
  {
    // TODO:
    return 0;
  }
  void attrRotation(int r)
  {
    // TODO:
  }
  int get_attrRotation()
  {
    // TODO:
    return 0;
  }
  void attrScale(int w, int h)
  {
    scale_width = w; scale_height = h;
  }
  void get_attrScale(int& w, int& h)
  {
    w = scale_width; h = scale_height;
  }
  void drawLine(int x1, int y1, int x2, int y2)
  {
    // TODO:
  }
  void drawRect(std::string mode, int x, int y, int w, int h)
  {
    QMutex* m = 0;
    if(root)
      m = &player->window->image_mutex;
    QMutexLocker locker(m);
    drawRect_unlock(mode, x, y, w, h);  
  }
  void drawRect_unlock(std::string mode, int x, int y, int w, int h)
  {
    if(mode == "fill")
    {
      QPainter painter;
      painter.begin(&*image);
      QRect rect (x, y, w, h);
      QColor color (color_red, color_green, color_blue/*, color_alpha*/, 255u);
      painter.fillRect(rect, color);
      painter.end();
    }
  }
  void drawRoundRect(std::string mode, int x, int y, int w, int h, int arc_w, int arc_h)
  {
    // TODO:
  }
  // should return a C-function
  void drawPolygon(std::string mode)
  {
    // TODO:
  }
  void drawEllipse(std::string mode, int xc, int xy, int w, int h, int ang_s, int ang_e)
  {
    // TODO:
  }
  void drawText(int x, int y, std::string text)
  {
    {
      QMutex* m = 0;
      if(root)
        m = &player->window->image_mutex;
      QMutexLocker locker(m);
      QPainter painter;
      painter.begin(&*image);

      QFont const& font = painter.font();
      QFontMetrics metrics(font);

      QPoint point(x, y + metrics.height());
      QLatin1String latin_string(text.c_str());
      QString string(latin_string);
      QColor color (color_red, color_green, color_blue, color_alpha);
      QPen pen(color);
      painter.setPen(pen);
      painter.drawText(point, QString(text.c_str()));

      painter.end();      
    }
  }
  void clear(int x, int y, int w, int h)
  {
    drawRect("fill", x, y, w, h);
  }
  void clear_unlock(int x, int y, int w, int h)
  {
    drawRect_unlock("fill", x, y, w, h);
  }
  void clear_default()
  {
    QMutex* m = 0;
    if(root)
      m = &player->window->image_mutex;
    QMutexLocker locker(m);
    QSize size = image->size();
    clear_unlock(0, 0, size.width(), size.height());
  }
  void compose (int x, int y, canvas src)
  {
    {
      QMutex* m = 0;
      if(root)
        m = &player->window->image_mutex;
      QMutexLocker locker(m);

      QPainter painter;
      painter.begin(&*image);

      QRect src_rect (src.crop_x, src.crop_y, src.crop_w, src.crop_h);
      QSize src_size = src.image->size();
      if(src_size.width() == 0 || src_size.height() == 0)
      {
        painter.end();
        return;
      }
      float scale_width = src.scale_width/src_size.width();
      float scale_height = src.scale_height/src_size.height();

      QSize destination_size = image->size();
      int res_width = src.scale_width?(scale_width*src.crop_w):(src.crop_w);
      if(x + res_width > destination_size.width())
      {
        res_width = destination_size.width() - x;
        src_rect.setWidth(src.scale_width?(res_width/scale_width):(res_width));
      }

      int res_height = src.scale_height?(scale_height*src.crop_h):src.crop_h;
      if(y + res_height > destination_size.height())
      {
        res_height = destination_size.height() - y;
        src_rect.setHeight(src.scale_height?(res_height/scale_height):(res_height));
      }

      QRect dest_rect (x, y, res_width, res_height);

      painter.drawImage(dest_rect, *src.image, src_rect);

      painter.end();
    }
  }
  void pixel(int x, int y, int red, int green, int blue, int alpha)
  {
    // TODO:
  }
  void get_pixel(int& red, int& green, int& blue, int& alpha, int x, int y)
  {
    // TODO:
  }
  void measureText(int& dx, int& dy, std::string text)
  {
    QFontMetrics metrics(font);
    QString string(QString::fromLatin1(text.c_str()));
    QRect rect = metrics.boundingRect(string);
    dx = rect.width() - rect.x();
    dy = rect.height() - rect.y();
  }
  void flush()
  {
    if(root)
      QCoreApplication::postEvent(player->window, new repaint_event(player->window->repaint_event_type));
  }
};

}

void lua_widget::paintEvent(QPaintEvent* event)
{
  QMutexLocker lock(&image_mutex);

  assert(!!image);
  {
    QPainter painter;
    painter.begin(this);

    QSize size = image->size();
    assert(this->size() == size);
    QRect rect(0, 0, size.width(), size.height());
    painter.drawImage(rect, *image);

    painter.end();
  }
}

void lua_player::init_canvas()
{
  QMutexLocker lock(&window->image_mutex);
  canvas root (window->image, this, true);

  QCoreApplication::postEvent(window, new repaint_event(window->repaint_event_type));

  luabind::module(L, "ghtv")
  [
   luabind::class_<lua::canvas>("canvas")
     .def("new", &canvas::canvas_image_new)
     .def("new", &canvas::canvas_buffer_new)
     .def("attrSize", &canvas::attrSize, luabind::pure_out_value(_2)
          | luabind::pure_out_value(_3))
     .def("attrColor", &canvas::attrColor)
     .def("attrColor", &canvas::attrColor_string)
     .def("attrColor", &canvas::get_attrColor, luabind::pure_out_value(_2)
          | luabind::pure_out_value(_3) | luabind::pure_out_value(_4)
          | luabind::pure_out_value(_5))
     .def("attrFont", &canvas::attrFont)
     .def("attrFont", &canvas::attrFont_non_normative)
     .def("attrFont", &canvas::get_attrFont)
     .def("attrClip", &canvas::attrClip)
     .def("attrClip", &canvas::get_attrClip, luabind::pure_out_value(_2)
          | luabind::pure_out_value(_3) | luabind::pure_out_value(_4)
          | luabind::pure_out_value(_5))
     .def("attrCrop", &canvas::attrCrop)
     .def("attrCrop", &canvas::get_attrCrop, luabind::pure_out_value(_2)
          | luabind::pure_out_value(_3) | luabind::pure_out_value(_4)
          | luabind::pure_out_value(_5))
     .def("attrFlip", &canvas::attrFlip)
     .def("attrFlip", &canvas::get_attrFlip, luabind::pure_out_value(_2)
          | luabind::pure_out_value(_3))
     .def("attrOpacity", &canvas::attrOpacity)
     .def("attrOpacity", &canvas::get_attrOpacity)
     .def("attrRotation", &canvas::attrRotation)
     .def("attrRotation", &canvas::get_attrRotation)
     .def("attrScale", &canvas::attrScale)
     .def("attrScale", &canvas::get_attrScale, luabind::pure_out_value(_2)
          | luabind::pure_out_value(_3))
     .def("drawLine", &canvas::drawLine)
     .def("drawRect", &canvas::drawRect)
     .def("drawRoundRect", &canvas::drawRoundRect)
     .def("drawPolygon", &canvas::drawPolygon)
     .def("drawEllipse", &canvas::drawEllipse)
     .def("drawText", &canvas::drawText)
     .def("clear", &canvas::clear)
     .def("clear", &canvas::clear_default)
     .def("flush", &canvas::flush)
     .def("compose", &canvas::compose)
     .def("pixel", &canvas::pixel)
     .def("pixel", &canvas::get_pixel, luabind::pure_out_value(_2)
          | luabind::pure_out_value(_3) | luabind::pure_out_value(_4)
          | luabind::pure_out_value(_5))
     .def("measureText", &canvas::measureText, luabind::pure_out_value(_2)
          | luabind::pure_out_value(_3))
  ];

  luabind::globals(L)["canvas"] = root;
}

} } } }

