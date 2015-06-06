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

#ifndef GHTV_QT_DEBUG_MEDIAS_TABLE_WIDGET_HPP
#define GHTV_QT_DEBUG_MEDIAS_TABLE_WIDGET_HPP

#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/initialized_data.hpp>

#include <boost/lexical_cast.hpp>

#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>

namespace ghtv { namespace qt { namespace ncl { namespace debug {

namespace debug_detail {

template <typename Media>
void add_media_to_list_media(Media& m, std::string const& path, QTableWidget* list)
{
  typedef gntl::concept::structure::media_traits<Media> media_traits;
  BOOST_CONCEPT_ASSERT((gntl::concept::structure::Media<Media>));

  std::string media_id = media_traits::identifier(m);
  QTableWidgetItem* media_id_item = new QTableWidgetItem(media_id.c_str());

  typedef typename media_traits::presentation_range presentation_range;
  presentation_range presentations = media_traits::presentation_all(m);

  std::size_t presentations_size = boost::distance(presentations);
  std::string presentations_string = boost::lexical_cast<std::string>(presentations_size);
  QString presentations_qstring (presentations_string.c_str());
  QTableWidgetItem* presentations_item = new QTableWidgetItem(presentations_qstring);

  QString path_qstring (path.c_str());
  QTableWidgetItem* path_item = new QTableWidgetItem(path_qstring);

  std::size_t size = list->rowCount();
  list->insertRow(size);
  list->setItem(size, 0, media_id_item);
  list->setItem(size, 1, presentations_item);
  list->setItem(size, 2, path_item);
}

template <typename Context>
void add_media_to_list_context(Context context, std::string path, QTableWidget* list)
{
  typedef gntl::concept::structure::context_traits<Context> context_traits;
  BOOST_CONCEPT_ASSERT((gntl::concept::structure::Context<Context>));

  path += '/';
  path += context_traits::identifier(context);

  typedef typename context_traits::media_range media_range;
  media_range medias = context_traits::media_all(context);
  typedef typename boost::range_iterator<media_range>::type media_iterator;
  for(media_iterator first = boost::begin(medias), last = boost::end(medias)
        ;first != last; ++first)
  {
    add_media_to_list_media(*first, path, list);
  }

  typedef typename context_traits::context_range context_range;
  context_range contexts = context_traits::context_all(context);
  typedef typename boost::range_iterator<context_range>::type context_iterator;
  for(context_iterator first = boost::begin(contexts), last = boost::end(contexts)
        ;first != last; ++first)
  {
    add_media_to_list_context(*first, path, list);
  }
}

template <typename Document>
void add_media_to_list(Document& document, QTableWidget* list)
{
  typedef gntl::concept::structure::document_traits<Document> document_traits;
  add_media_to_list_context(document_traits::body(document), "", list);
}

}

class medias_table_widget : public QWidget
{
  Q_OBJECT
public:
  medias_table_widget(ncl_widget* ncl)
    : ncl(ncl), media_table(0)
  {
    QVBoxLayout* layout = new QVBoxLayout;

    media_table = new QTableWidget(0, 3);
    QStringList header;
    header.push_back("Media name");
    header.push_back("Presentations active");
    header.push_back("Media path");
    media_table->setHorizontalHeaderLabels(header);

    debug_detail::add_media_to_list(*ncl->initialized_data_->document, media_table);

    media_table->resizeColumnsToContents();
    media_table->verticalHeader()->hide();
    media_table->horizontalHeader()->setStretchLastSection(true);

    layout->addWidget(media_table);

    setLayout(layout);
  }
  
  ncl_widget* ncl;
  QTableWidget* media_table;
};

} } } }

#endif
