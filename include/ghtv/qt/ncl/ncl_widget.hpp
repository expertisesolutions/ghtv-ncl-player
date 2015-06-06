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

#ifndef NCL_WINDOW_HPP
#define NCL_WINDOW_HPP

#include <ghtv/qt/ncl/executor.hpp>
#include <ghtv/qt/ncl/player/url_file.hpp>

#include <gntl/parser/libxml2/dom/document.hpp>
#include <gntl/parser/libxml2/dom/xml_document.hpp>
#include <gntl/parser/libxml2/dom/context.hpp>
#include <gntl/structure/composed/context.hpp>
#include <gntl/structure/composed/media.hpp>
#include <gntl/structure/composed/document.hpp>
#include <gntl/structure/composed/link.hpp>
#include <gntl/structure/composed/descriptor.hpp>
#include <gntl/structure/composed/descriptor_lookupable.hpp>
#include <gntl/structure/composed/region_lookupable.hpp>
#include <gntl/structure/composed/presentation.hpp>
#include <gntl/algorithm/structure/media.hpp>

#include <boost/optional.hpp>

#include <QWidget>
#include <QString>
#include <QTimer>

#ifdef GHTV_PHONON_LOWER_CASE_DIR
#include <phonon/videoplayer.h>
#else
#include <Phonon/VideoPlayer>
#endif

namespace ghtv { namespace qt { namespace ncl {

struct presentation_factory
{
  ncl_widget* root_widget;
  std::string root_path;

  typedef std::string document_uri_type;
  typedef gntl::structure::composed::descriptor
    <gntl::parser::libxml2::dom::descriptor
     , document_uri_type> descriptor_type;
  typedef gntl::parser::libxml2::dom::xml_string<> identifier_type;
  typedef gntl::structure::composed::presentation
    <executor, descriptor_type> presentation_type;
  typedef presentation_type result_type;
  result_type operator()(descriptor_type d) const;
};

class video_finished_event : public QObject
{
  Q_OBJECT
public:
  ncl_widget* this_;
  std::string id;
  std::string context;
  executor* executor_;
  bool expired;
  video_finished_event(ncl_widget* this_, std::string id, std::string context
                       , executor* executor_)
    : this_(this_), id(id), context(context), executor_(executor_), expired(false) {}
public Q_SLOTS:
  void finished_event();
};

class file_download : public QObject
{
  Q_OBJECT
public:

  file_download(ncl::player::url_file* file
                , QUrl filename, ncl_widget* window
                , QObject* parent
                , std::string document_uri)
    : QObject(parent), file(file), filename(filename), window(window)
    , expired(false), document_uri(document_uri)
  {}

  ncl::player::url_file* file;
  QUrl filename;
  ncl_widget* window;
  bool expired;
  std::string document_uri;
Q_SIGNALS:
  void downloaded(QIODevice& device, QUrl filename, std::string document_uri);
public Q_SLOTS:
  void download_finish();
  void download_error(std::string);
};

class import_document_downloaded_mapper : public QObject
{
  Q_OBJECT
public:
  typedef gntl::parser::libxml2::dom::document
    parser_document;
  typedef gntl::structure::composed::document
    <parser_document, presentation_factory, std::string>
    structure_document;

  import_document_downloaded_mapper(std::map<std::string, structure_document>* imported_documents)
    : imported_documents(imported_documents)
  {}

  std::map<std::string, structure_document>* imported_documents;
Q_SIGNALS:
  void import_document_downloaded(QIODevice& io_device, QUrl url, std::string string
                                  , std::map<std::string, structure_document>* imported_documents
                                  , bool b);
public Q_SLOTS:
  void downloaded(QIODevice& io_device, QUrl url, std::string string)
  {
    Q_EMIT import_document_downloaded(io_device, url, string, imported_documents, true);
  }
};

struct ncl_widget : QWidget
{
  Q_OBJECT
public:
  void stop_ncl();
  void start_structure_ncl();
  void start_ncl(QUrl file);
  void idle_register();
  void pause_events();
  void resume_events();

  ncl_widget();
  ~ncl_widget();

  typedef gntl::parser::libxml2::dom::document
    parser_document;
  typedef gntl::parser::libxml2::dom::head
    parser_head;
  typedef std::string document_uri_type;
  typedef gntl::structure::composed::descriptor
    <gntl::parser::libxml2::dom::descriptor
     , document_uri_type> descriptor_type;
  typedef gntl::structure::composed::presentation
    <executor, descriptor_type> presentation_type;
  typedef gntl::parser::libxml2::dom::head parser_head_type;
  typedef gntl::structure::composed::document
    <parser_document, presentation_factory, document_uri_type>
    structure_document;
  typedef gntl::concept::structure::document_traits
    <structure_document> structure_document_traits;
  typedef structure_document_traits::component_identifier component_identifier;
  typedef gntl::concept::parser::document_traits
  <gntl::parser::libxml2::dom::document> document_traits;
  typedef gntl::concept::parser::head_traits
  <gntl::parser::libxml2::dom::head> head_traits;
  typedef gntl::concept::parser::body_traits
  <gntl::parser::libxml2::dom::document::body_type> body_traits;
  typedef head_traits::connector_base_type connector_base_type;
  typedef gntl::concept::parser::connector_base_traits
    <connector_base_type> connector_base_traits;

  gntl::algorithm::structure::media::dimensions screen_dimensions;

  void import_document(document_uri_type document
                       , std::map<std::string, structure_document>* imported_documents);

  bool process_key(QKeyEvent* event, bool pressed);
  void video_signal_finish(executor* e, Phonon::VideoPlayer* video_player
                           , std::string id, std::string context);
  void natural_end(gntl::structure::composed::component_location
                   <std::string, std::string> location);
  void area_event(gntl::structure::composed::component_location
                  <std::string, std::string> l
                  , std::string interface_
                  , gntl::transition_type transition);

  struct initialized_data;
  initialized_data* initialized_data_;
  std::map<int, std::vector<QWidget*> > zindex_map;
  std::auto_ptr<QTimer> idle_timer;
  std::vector<video_finished_event*> video_finished_events;
  std::vector<video_finished_event*> cleanup_videos;
  std::vector<file_download*> outstanding_downloads;
  std::map<std::string, structure_document> temporary_imported_documents;
  std::string root_path;
  QLabel* background_label;

  bool events_paused;
  void media_clicked(std::string const& media_identifier, std::string const& context_identifier)
  {
    Q_EMIT media_clicked_signal(media_identifier, context_identifier);
  }
  void error_occurred(std::string const& error_message)
  {
    Q_EMIT error_occurred_signal(error_message);
  }

  void resizeEvent(QResizeEvent* event);
Q_SIGNALS:
  void media_clicked_signal(std::string media_identifier, std::string context_identifier);
  void error_occurred_signal(std::string);
public Q_SLOTS:
  void import_document_downloaded(QIODevice& file, QUrl url, std::string document_uri
                                  , std::map<std::string, structure_document>* imported_documents
                                  , bool start_if_no_outstanding);
  void start_ncl(QIODevice& file, QUrl filename, std::string document_uri);
  void processWhenIdle();
};

inline presentation_factory::result_type presentation_factory::operator()
  (descriptor_type d) const
{
  return result_type(executor(root_widget, root_path, root_widget->zindex_map), d);
}

} } }

#endif
