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

#ifndef GHTV_QT_JAVA_FILE_INPUT_STREAM_HPP
#define GHTV_QT_JAVA_FILE_INPUT_STREAM_HPP

#include <jvb/adapt_class.hpp>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <QFile>
#include <QDir>

namespace ghtv { namespace qt { namespace java {

struct file_input_stream
{
  boost::shared_ptr<QFile> file;
  QDir root_dir;

  file_input_stream(JNIEnv* env, QDir root_dir)
    : root_dir(root_dir)
  {
  }

  void constructor(JNIEnv* env, jvb::string s);
  jvb::int_ read_array(jvb::array<jvb::byte> b, jvb::int_ off
                            , jvb::int_ len);
  jvb::int_ available() const;
  void close();
};

} } }

#endif
