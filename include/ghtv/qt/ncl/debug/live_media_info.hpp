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

#ifndef GHTV_QT_DEBUG_LIVE_MEDIA_INFO_HPP
#define GHTV_QT_DEBUG_LIVE_MEDIA_INFO_HPP

#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/initialized_data.hpp>

#include <boost/mpl/begin.hpp>
#include <boost/mpl/end.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/type_traits/is_same.hpp>

#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QTextEdit>

#include <fstream>

namespace ghtv { namespace qt { namespace ncl {namespace debug {

class live_media_info : public QWidget
{
  Q_OBJECT
public:
  typedef ncl_widget::structure_document_traits document_traits;
  typedef document_traits::body_type body_type;
  typedef body_type::link_range link_range;
  typedef boost::range_value<link_range>::type link_type;

  live_media_info(std::string media_identifier, std::string context_identifier, ncl_widget* ncl
                  , QWidget* parent = 0);

  QWidget* create_media_code(gntl::parser::libxml2::dom::xml_node media_node);
  QWidget* create_link_code(gntl::parser::libxml2::dom::xml_node link_node);

  QTableWidget* properties_table;
  QTableWidget* links_table;
  QVBoxLayout* right_layout;
  QWidget* media_code_widget;
  QWidget* link_code_widget;
  ncl_widget* ncl;

public Q_SLOTS:
  void link_selection_changed();
};

} } } }

#endif
