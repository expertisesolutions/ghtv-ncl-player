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

#ifndef GHTV_QT_JAVA_REMOTE_KEYS_HPP
#define GHTV_QT_JAVA_REMOTE_KEYS_HPP

namespace ghtv { namespace qt { namespace java {

namespace remote_keys {

enum remote_key
{
  dimmer = 40
  , bass_up = 72
  , bass_down = 73
  , info = 74
  , confirm = 80
  , back = 81
};

}

using remote_keys::remote_key;

} } }

#endif
