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

#ifndef GHTV_QT_DSMCC_HPP
#define GHTV_QT_DSMCC_HPP

namespace ghtv { namespace qt {

struct module
{
  module(unsigned int module_id, unsigned int module_size
         , unsigned int module_version, unsigned int blocks)
    : module_id(module_id), module_size(module_size)
    , module_version(module_version)
    , blocks_finished(blocks)
    , content(module_size)
  {
  }

  bool finished()
  {
    for(std::vector<bool>::const_iterator first = blocks_finished.begin()
          , last = blocks_finished.end(); first != last; ++first)
    {
      if(!*first)
        return false;
    }
    return true;
  }

  unsigned int module_id, module_size, module_version;
  std::vector<bool> blocks_finished;
  std::vector<unsigned char> content;
};

} }

#endif
