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

#ifndef GHTV_QT_PLAYER_PLAYER_FACTORY_HPP
#define GHTV_QT_PLAYER_PLAYER_FACTORY_HPP

#include <ghtv/qt/ncl/executor.hpp>

#include <gntl/structure/composed/component_location.hpp>

#include <boost/shared_ptr.hpp>

#include <QLabel>

namespace ghtv { namespace qt { namespace ncl { struct ncl_widget; } } }

namespace ghtv { namespace qt { namespace ncl { namespace player {

boost::shared_ptr<player_base> player_factory(std::string source, QWidget* player_container
                                              , std::string const& root_path
                                              , gntl::structure::composed::component_location<std::string, std::string>
                                              , ncl_widget* ncl_window
                                              , executor* executor_, boost::optional<std::string> interface_);

} } } }

#endif
