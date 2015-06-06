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

#ifndef GHTV_QT_INITIALIZED_DATA_HPP
#define GHTV_QT_INITIALIZED_DATA_HPP

#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/executor.hpp>
#include <ghtv/qt/ncl/executor_traits_def.hpp>

#include <gntl/parser/libxml2/dom/document.hpp>
#include <gntl/parser/libxml2/dom/xml_document.hpp>
#include <gntl/parser/libxml2/dom/head.hpp>

namespace ghtv { namespace qt { namespace ncl {

struct ncl_widget::initialized_data
{
  std::auto_ptr<gntl::parser::libxml2::dom::xml_document> xml_document; // for ownership
  std::vector<boost::shared_ptr<gntl::parser::libxml2::dom::xml_document> > xml_documents;
  std::string root_document_uri;
  std::auto_ptr<structure_document> document;
  
  initialized_data(std::auto_ptr<gntl::parser::libxml2::dom::xml_document> xml_document
                   , std::vector<boost::shared_ptr<gntl::parser::libxml2::dom::xml_document> > xml_documents
                   , std::string root_document_uri)
    : xml_document(xml_document), xml_documents(xml_documents), root_document_uri(root_document_uri)
  {}
private:
  initialized_data(initialized_data const&);
  initialized_data& operator=(initialized_data const&);
};

} } }

#endif
