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

#ifndef GHTV_QT_JAVA_IMAGE_HPP
#define GHTV_QT_JAVA_IMAGE_HPP

#include <jvb/adapt_class.hpp>

#include <QImage>
#include <QFileInfo>
#include <QDir>
#include <QString>

namespace ghtv { namespace qt { namespace java { namespace awt {

struct image
{
  image(jvb::environment e, QString root_path, jvb::string path)
    : root_path(root_path)
  {
    std::cout << "image::image" << std::endl;
    QString path_string (QString::fromStdString(path.str(e)));

    std::cout << "Root path " << root_path.toStdString() << std::endl;
    while(!path_string.isEmpty() && path_string[0] == '/')
      path_string.remove(0, 1);
    info = QFileInfo(QDir(root_path), path_string);
    std::cout << "Loading " << info.absoluteFilePath().toStdString() << std::endl;
    qt_image.load(info.absoluteFilePath());
  }

  jvb::int_ width() const
  {
    std::cout << "image width: " << qt_image.width() << std::endl;
    return qt_image.width();
  }

  jvb::int_ height() const
  {
    std::cout << "image height: " << qt_image.height() << std::endl;
    return qt_image.height();
  }

  void set_scaling(jvb::int_ w, jvb::int_ h)
  {
  }
  void set_scaling_width(jvb::int_ w)
  {
  }
  void set_scaling_height(jvb::int_ h)
  {
  }


  // jvb::string path(JNIEnv* env) const
  // {
  //   return jvb::string(env, env->NewStringUTF(info.absoluteFilePath().toUtf8()));
  // }

  QFileInfo info;
  QString root_path;
  QImage qt_image;
};

} } } }

#endif
