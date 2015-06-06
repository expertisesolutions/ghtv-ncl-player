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

#ifndef GHTV_QT_DEBUG_PRESENTATIONS_TABLE_WIDGET_HPP
#define GHTV_QT_DEBUG_PRESENTATIONS_TABLE_WIDGET_HPP

#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/initialized_data.hpp>

#include <QWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>

namespace ghtv { namespace qt { namespace ncl { namespace debug {

class presentations_table_widget : public QWidget
{
  Q_OBJECT
public:
  typedef ncl_widget::structure_document_traits document_traits;

  struct add_presentation_visitor
  {
    typedef void result_type;
    template <typename Media>
    result_type operator()(Media media) const
    {
      typedef typename gntl::unwrap_parameter<Media>::type media_type;
      typedef gntl::concept::structure::media_traits<media_type> media_traits;
      typedef typename media_traits::presentation_range presentation_range;
      presentation_range presentations = media_traits::presentation_all(media);
      typedef typename boost::range_iterator<presentation_range>::type presentation_iterator;
      for(presentation_iterator first = boost::begin(presentations)
            , last = boost::end(presentations);first != last; ++first)
      {
        typedef typename boost::range_value<presentation_range>::type presentation_type;
        typedef gntl::concept::structure::presentation_traits<presentation_type> presentation_traits;
        typedef typename presentation_type::executor_type executor_type;
        std::size_t size = self->presentation_table->rowCount();
        self->presentation_table->insertRow(size);

        QString source (first->executor().source.c_str());
        QTableWidgetItem* source_item = new QTableWidgetItem(source);
        self->presentation_table->setItem(size, 3, source_item);

        QString state;
        if(presentation_traits::is_occurring(*first))
          state = "occurring";
        else if(presentation_traits::is_paused(*first))
          state = "paused";
        else if(presentation_traits::is_sleeping(*first))
          state = "sleeping";
        QTableWidgetItem* state_item = new QTableWidgetItem(state);
        self->presentation_table->setItem(size, 0, state_item);
      }
    }

    presentations_table_widget* self;
  };

  presentations_table_widget(std::string const& media_id, ncl_widget* ncl)
    : media_id(media_id), ncl(ncl)
  {
    QVBoxLayout* layout = new QVBoxLayout;

    presentation_table = new QTableWidget(0, 4);
    QStringList header;
    header.push_back("state");
    header.push_back("when created");
    header.push_back("descriptor");
    header.push_back("src");
    presentation_table->setHorizontalHeaderLabels(header);

    typedef document_traits::media_lookupable media_lookupable;
    media_lookupable media_lookup = document_traits::media_lookup(*ncl->initialized_data_->document);
    typedef gntl::concept::lookupable_traits<media_lookupable> lookupable_traits;
    typedef lookupable_traits::result_type lookup_result;

    lookup_result media = lookupable_traits::lookup(media_lookup, media_id);
    assert(media != lookupable_traits::not_found(media_lookup));

    add_presentation_visitor visitor = {this};
    visitor(*media);

    presentation_table->resizeColumnsToContents();
    presentation_table->verticalHeader()->hide();
    presentation_table->horizontalHeader()->setStretchLastSection(true);

    layout->addWidget(presentation_table);

    setLayout(layout);
  }

  std::string media_id;
  ncl_widget* ncl;
  QTableWidget* presentation_table;
};

} } } }

#endif
