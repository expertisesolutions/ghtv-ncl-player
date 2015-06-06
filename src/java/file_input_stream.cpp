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

#include <ghtv/qt/java/file_input_stream.hpp>

#include <vector>
#include <string>

namespace ghtv { namespace qt { namespace java {

// void file_input_stream::constructor(JNIEnv* env, jvb::string s)
// {
//   std::cout << "file_input_stream::constructor " << s.str(env) << std::endl;
//   std::string str = s.str(env);
//   QString path = root_dir.absolutePath() + "/" + QString::fromStdString(str);
//   std::cout << "Should open " << path.toStdString() << std::endl;
//   file.reset(new QFile(path));
//   file->open(QIODevice::ReadOnly);
// }

// jvb::int_ file_input_stream::read_array
//  (jvb::array<jvb::byte> b, jvb::int_ off, jvb::int_ len)
// {
//   std::cout << "file_input_stream::read_array " << off << std::endl;
//   std::cout << "size: " << file->size() << std::endl;
//   assert(!!file);
//   std::vector<jbyte> bytes((std::min<jint>)(b.length(), len));
//   if(!bytes.empty())
//   {
//     qint64 r = file->read(reinterpret_cast<char*>(&bytes[0]), bytes.size());
//     std::cout << "read " << r << " bytes" << std::endl;
//     if(r)
//     {
//       b.set(&bytes[0], r);
//       return r;
//     }
//   }
//   return -1;
// }

// jvb::int_ file_input_stream::available() const
// {
//   std::size_t av = file->size() - file->pos();
//   std::cout << "bytes available: " << av << std::endl;
//   return av;
// }

// void file_input_stream::close()
// {
//   file.reset();
// }

} } }
