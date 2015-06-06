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

#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/executor.hpp>
#include <ghtv/qt/ncl/executor_traits_def.hpp>
#include <ghtv/qt/ncl/initialized_data.hpp>
#include <ghtv/qt/ncl/player/url_file.hpp>

#include <gntl/parser/libxml2/dom/document.hpp>
#include <gntl/parser/libxml2/dom/xml_document.hpp>
#include <gntl/parser/libxml2/dom/head.hpp>

#include <gntl/algorithm/structure/document/search.hpp>
#include <gntl/algorithm/structure/context.hpp>
#include <gntl/algorithm/structure/context/evaluate_links.hpp>
#include <gntl/algorithm/parser/causal_connector.hpp>
#include <gntl/algorithm/structure/link.hpp>
#include <gntl/structure/composed/link.hpp>
#include <gntl/algorithm/structure/condition/evaluate.hpp>
#include <gntl/algorithm/structure/action.hpp>
#include <gntl/algorithm/structure/action/defaults.hpp>
#include <gntl/algorithm/structure/component/abort.ipp>
#include <gntl/algorithm/structure/component/pause.ipp>
#include <gntl/algorithm/structure/component/resume.ipp>
#include <gntl/algorithm/structure/component/start.ipp>
#include <gntl/algorithm/structure/component/stop.ipp>
#include <gntl/algorithm/structure/switch/start.ipp>
#include <gntl/algorithm/structure/switch/start_action_traits.ipp>
#include <gntl/algorithm/structure/switch/stop.ipp>
#include <gntl/algorithm/structure/switch/stop_action_traits.ipp>
#include <gntl/algorithm/structure/switch/abort.ipp>
#include <gntl/algorithm/structure/context/start.ipp>
#include <gntl/algorithm/structure/context/start_action_traits.ipp>
#include <gntl/algorithm/structure/context/abort.ipp>
#include <gntl/algorithm/structure/context/abort_action_traits.ipp>
#include <gntl/algorithm/structure/context/pause.ipp>
#include <gntl/algorithm/structure/context/pause_action_traits.ipp>
#include <gntl/algorithm/structure/context/resume.ipp>
#include <gntl/algorithm/structure/context/resume_action_traits.ipp>
#include <gntl/algorithm/structure/context/generic_algo.ipp>
#include <gntl/algorithm/structure/context/stop.ipp>
#include <gntl/algorithm/structure/context/stop_action_traits.ipp>
#include <gntl/algorithm/structure/port/abort.ipp>
#include <gntl/algorithm/structure/port/abort_action_traits.ipp>
#include <gntl/algorithm/structure/port/pause.ipp>
#include <gntl/algorithm/structure/port/pause_action_traits.ipp>
#include <gntl/algorithm/structure/port/resume.ipp>
#include <gntl/algorithm/structure/port/resume_action_traits.ipp>
#include <gntl/algorithm/structure/port/start.ipp>
#include <gntl/algorithm/structure/port/start_action_traits.ipp>
#include <gntl/algorithm/structure/port/stop.ipp>
#include <gntl/algorithm/structure/port/stop_action_traits.ipp>
#include <gntl/algorithm/structure/media/abort.ipp>
#include <gntl/algorithm/structure/media/abort_action_traits.ipp>
#include <gntl/algorithm/structure/media/pause.ipp>
#include <gntl/algorithm/structure/media/pause_action_traits.ipp>
#include <gntl/algorithm/structure/media/resume.ipp>
#include <gntl/algorithm/structure/media/resume_action_traits.ipp>
#include <gntl/algorithm/structure/media/set.ipp>
#include <gntl/algorithm/structure/media/set_action_traits.ipp>
#include <gntl/algorithm/structure/media/start.ipp>
#include <gntl/algorithm/structure/media/start_action_traits.ipp>
#include <gntl/algorithm/structure/media/stop.ipp>
#include <gntl/algorithm/structure/media/stop_action_traits.ipp>
#include <gntl/algorithm/structure/media/natural_end_action_traits.hpp>
#include <gntl/algorithm/structure/media/natural_end_action_traits.ipp>
#include <gntl/algorithm/structure/media/natural_end.ipp>
#include <gntl/algorithm/structure/media/natural_end_normal_action.hpp>
#include <gntl/algorithm/structure/context/transitions.hpp>
#include <gntl/algorithm/structure/condition/iteration.hpp>
#include <gntl/algorithm/structure/switch_functor.hpp>
#include <gntl/algorithm/structure/media/select.ipp>
#include <gntl/algorithm/parser/document.hpp>

#include <boost/mpl/int.hpp>
#include <boost/utility.hpp>
#include <boost/bind.hpp>

#include <QFile>
#include <QTextStream>
#include <QKeyEvent>

namespace ghtv { namespace qt { namespace ncl {

namespace lua {

lua_widget::~lua_widget() {}

}

ncl_widget::ncl_widget() : initialized_data_(0), events_paused(false)
{
  background_label = new QLabel(this);
  QPalette p(background_label->palette());
  p.setColor(QPalette::Background, Qt::white);
  background_label->setAutoFillBackground(true);
  background_label->setPalette(p);
  background_label->move(0, 0);
  background_label->resize(size ());
}

void ncl_widget::resizeEvent(QResizeEvent* event)
{
  background_label->resize(event->size());
}

void ncl_widget::pause_events()
{
  events_paused = true;
}

void ncl_widget::resume_events()
{
  events_paused = false;
  idle_register();
}

void ncl_widget::stop_ncl()
{
  if(events_paused) return;

  for(std::vector<video_finished_event*>::iterator
        first = video_finished_events.begin(), last = video_finished_events.end()
        ;first != last;++first)
    (*first)->expired = true;
  for(std::vector<file_download*>::iterator
        first = outstanding_downloads.begin()
        , last = outstanding_downloads.end()
        ;first != last; ++first)
    (*first)->expired = true;
  video_finished_events.clear();
  outstanding_downloads.clear();
  temporary_imported_documents.clear();
  if(idle_timer.get())
  {
    idle_timer->stop();
    idle_timer.reset();
  }
  zindex_map.clear();
  idle_timer.reset();
  delete initialized_data_;
  initialized_data_ = 0;
}

void ncl_widget::import_document_downloaded(QIODevice& iodevice, QUrl url, std::string document_uri
                                            , std::map<std::string, structure_document>* imported_documents
                                            , bool start_if_no_outstanding)
{
  std::vector<boost::shared_ptr<gntl::parser::libxml2::dom::xml_document> >&
    xml_documents = initialized_data_->xml_documents;
  std::map<document_uri_type, structure_document>& 
    imported_structure_documents = *imported_documents;//temporary_imported_documents;

  QTextStream stream(&iodevice);
  stream.seek(0);
  QString data = stream.readAll();
  if(!data.isEmpty())
  {
    std::string to_string = data.toStdString();
    assert(!to_string.empty());
    ::xmlDocPtr xmldoc = xmlParseDoc
        (const_cast<xmlChar*> // needed for older libxml2 versions like found in Symbian SDK
         (static_cast<const xmlChar*>
          (static_cast<const void*>(to_string.c_str()))));
    if(!xmldoc)
    {
      std::string error_msg = "XML malformed for " + url.toString().toStdString();
      Q_EMIT error_occurred_signal(error_msg);
      return;
    }
    
    boost::shared_ptr<gntl::parser::libxml2::dom::xml_document>
      xml_imported(new gntl::parser::libxml2::dom::xml_document(xmldoc)); // gets ownership
    xml_documents.push_back(xml_imported);
    gntl::parser::libxml2::dom::document d (xml_imported->root ());
    
    presentation_factory factory
      =
      {
        this
        , root_path
      };

    std::map<document_uri_type, structure_document> documents;
    gntl::algorithm::parser::document::for_each_import_document_uri
      (gntl::ref(d)
       , boost::bind(&ncl_widget::import_document, this, _1, &documents));

    imported_structure_documents.insert
      (std::make_pair(document_uri, structure_document(d, document_uri, factory, documents)));

    if(start_if_no_outstanding && outstanding_downloads.empty())
      start_structure_ncl();
  }
  else
  {
    std::string error_msg = "Couldn't load " + document_uri;
    Q_EMIT error_occurred_signal(error_msg);
  }
}

void ncl_widget::import_document(document_uri_type document_uri
                                 , std::map<std::string, structure_document>* imported_documents)
{ 
  std::string filename_std = document_uri;
  filename_std = root_path + '/' + filename_std;
  std::auto_ptr<qt::ncl::player::url_file> url_file
    (new qt::ncl::player::url_file(QString::fromStdString(filename_std), this));
  if(url_file->local())
  {
    import_document_downloaded(*url_file->io_device(), QUrl(filename_std.c_str()), document_uri
                               , imported_documents, false);
  }
  else
  {
    std::auto_ptr<file_download> download(new file_download(url_file.release(), QUrl(filename_std.c_str())
                                                            , this, this, document_uri));
    QObject::connect(download->file, SIGNAL(download_finished_signal())
                     , download.get(), SLOT(download_finish()));
    QObject::connect(download->file, SIGNAL(error_signal(std::string))
                     , download.get(), SLOT(download_error(std::string)));
    import_document_downloaded_mapper* mapper = new import_document_downloaded_mapper(imported_documents);
    QObject::connect(mapper
                     , SIGNAL(import_document_downloaded
                              (QIODevice&, QUrl, std::string
                               , std::map<std::string, structure_document>*
                               , bool))
                     , this, SLOT(import_document_downloaded
                              (QIODevice&, QUrl, std::string
                               , std::map<std::string, structure_document>*
                               , bool)));
    QObject::connect(download.get(), SIGNAL(downloaded(QIODevice&, QUrl, std::string))
                     , mapper, SLOT(downloaded(QIODevice&, QUrl, std::string)));
    outstanding_downloads.push_back(download.release());
  }
}

void ncl_widget::start_ncl(QIODevice& file, QUrl filename, std::string)
{
  qint64 size = file.size();
  if(size)
  {
    QTextStream stream(&file);
    QString data = stream.readAll();
    std::string to_string = data.toStdString();

    ::xmlParserCtxtPtr parser_context = ::xmlNewParserCtxt();
    assert(!!parser_context);
    ::xmlDocPtr xmldoc = ::xmlCtxtReadMemory(parser_context
                                             , to_string.c_str()
                                             , to_string.size()
                                             , filename.toString().toStdString().c_str()
                                             , 0, 0);
    if(!xmldoc)
      throw std::runtime_error("Malformed NCL");

    std::auto_ptr<gntl::parser::libxml2::dom::xml_document>
      xmldocument(new gntl::parser::libxml2::dom::xml_document(xmldoc)); // gets ownership
    BOOST_CONCEPT_ASSERT((gntl::concept::parser::Document<gntl::parser::libxml2::dom::document>));
    gntl::parser::libxml2::dom::document d (xmldocument->root ());

    root_path = filename.toString().toStdString();
    std::string root_document_uri;
    {
      std::string::reverse_iterator
        iterator = std::find(root_path.rbegin(), root_path.rend(), '/')
        , iterator_backslash = std::find(root_path.rbegin(), root_path.rend(), '\\');
      iterator = (std::min)(iterator, iterator_backslash);
      root_document_uri.insert(root_document_uri.end()
                               , iterator.base(), root_path.end());
      if(iterator != root_path.rend())
        root_path.erase(boost::prior(iterator.base()), root_path.end());
      else
      {
        std::string error_msg = "Couldn't create a absolute path from the path for the NCL file: ";
        error_msg += root_path;
        Q_EMIT error_occurred_signal(error_msg);
      }
    }

    gntl::parser::libxml2::dom::descriptor_base descriptor_base;
    gntl::parser::libxml2::dom::head head = document_traits::head(d);
    if(head_traits::has_descriptor_base (head))
      descriptor_base = head_traits::descriptor_base (head);

    BOOST_CONCEPT_ASSERT((gntl::concept::structure::Document<structure_document>));

    std::vector<boost::shared_ptr<gntl::parser::libxml2::dom::xml_document> > xml_documents;

    assert(initialized_data_ == 0);
    initialized_data_ = new initialized_data(xmldocument, xml_documents, root_document_uri);

    gntl::algorithm::parser::document::for_each_import_document_uri
      (gntl::ref(d)
       , boost::bind(&ncl_widget::import_document, this, _1, &temporary_imported_documents));

    if(outstanding_downloads.empty())
      start_structure_ncl();
  }
}

void ncl_widget::start_structure_ncl()
{
  presentation_factory factory
    =
    {
      this
      , root_path
    };

  gntl::parser::libxml2::dom::document d (initialized_data_->xml_document->root ());
  initialized_data_->document.reset
    (new structure_document(d, initialized_data_->root_document_uri, factory, temporary_imported_documents));

  QRect rect = geometry();
  gntl::algorithm::structure::media::dimensions screen_dimensions_
    = {0, 0, rect.width(), rect.height(), 0};
  screen_dimensions = screen_dimensions_;
  try
  {
    descriptor_type descriptor;
    gntl::algorithm::structure::context::start (gntl::ref(initialized_data_->document->body)
                                                , initialized_data_->document->body_location()
                                                , descriptor
                                                , gntl::ref(*initialized_data_->document)
                                                , screen_dimensions);
  }
  catch(boost::exception const& e)
  {
    Q_EMIT error_occurred_signal(boost::diagnostic_information(e));
  }
  catch(std::exception const& e)
  {
    Q_EMIT error_occurred_signal(boost::diagnostic_information(e));
  }
}

void file_download::download_finish()
{
  if(!expired)
  {
    std::vector<file_download*>::iterator
      iterator = std::find(window->outstanding_downloads.begin()
                           , window->outstanding_downloads.end()
                           , this);
    assert(iterator != window->outstanding_downloads.end());
    window->outstanding_downloads.erase(iterator);
    if(file)
    {
      try
      {
        Q_EMIT downloaded(*file->io_device(), filename, document_uri);
      }
      catch(std::exception& e)
      {
        window->error_occurred(boost::diagnostic_information(e));
      }
      file = 0;
    }
    delete this;
  }
}

void file_download::download_error(std::string error_msg)
{
  if(!expired)
  {
    std::vector<file_download*>::iterator
      iterator = std::find(window->outstanding_downloads.begin()
                           , window->outstanding_downloads.end()
                           , this);
    assert(iterator != window->outstanding_downloads.end());
    window->outstanding_downloads.erase(iterator);
    window->error_occurred(error_msg);
    file = 0;
    delete this;
  }
}

void ncl_widget::start_ncl(QUrl filename)
{
  std::auto_ptr<qt::ncl::player::url_file> file
    (new qt::ncl::player::url_file(filename, this));
  if(file->local())
  {
    start_ncl(*file->io_device(), filename, "");
  }
  else
  {
    std::auto_ptr<file_download> download(new file_download(file.release(), filename, this, this, ""));
    QObject::connect(download->file, SIGNAL(download_finished_signal())
                     , download.get(), SLOT(download_finish()));
    QObject::connect(download->file, SIGNAL(error_signal(std::string))
                     , download.get(), SLOT(download_error(std::string)));
    QObject::connect(download.get(), SIGNAL(downloaded(QIODevice&, QUrl, std::string)), this
                     , SLOT(start_ncl(QIODevice&, QUrl, std::string)));
    outstanding_downloads.push_back(download.release());
  }
}

ncl_widget::~ncl_widget()
{
  stop_ncl();
}

void ncl_widget::area_event(gntl::structure::composed::component_location
                            <std::string, std::string> l, std::string interface_
                            , gntl::transition_type transition)
{
  ncl_widget::structure_document_traits::register_event
    (*initialized_data_->document, l.identifier(), interface_
     , transition, gntl::event_enum::presentation);
  idle_register();
}

void ncl_widget::video_signal_finish(executor* executor_, Phonon::VideoPlayer* video_player
                                     , std::string id, std::string context)
{
  video_finished_event* e = new video_finished_event(this, id, context, executor_);
  video_finished_events.push_back(e);
  connect(video_player, SIGNAL(finished()), e, SLOT(finished_event()));
}

// namespace {

// template <typename Media, typename Descriptor, typename Document>
// struct natural_end_aux
// {
//   natural_end_aux(Media m, Descriptor d, Document doc
//                   , ncl_widget* self)
//     : m(m), d(d), doc(doc), self(self) {}

//   typedef void result_type;
//   template <typename Context>
//   void operator ()(Context c) const
//   {
//     std::cout << "natural_end" << std::endl;
//     gntl::algorithm::structure::media::natural_end(m, c, d, doc);

//     self->processWhenIdle();
  
//     std::cout << "stop_transition" << std::endl;
//     gntl::algorithm::structure::context::stop_transition(c, doc);
//   }

//   Media m;
//   Descriptor d;
//   Document doc;
//   ncl_widget* self;
// };

// template <typename Media, typename Descriptor, typename Document>
// natural_end_aux<Media, Descriptor, Document> make_natural_end_aux (Media m, Descriptor d, Document doc, ncl_widget* self)
// {
//   return natural_end_aux<Media, Descriptor, Document>(m, d, doc, self);
// }

// }

void ncl_widget::natural_end(gntl::structure::composed::component_location
                             <std::string, std::string> location)
{
  std::cout << "ncl_widget::natural_end" << std::endl;
  // typedef structure_document_traits::media_lookupable lookupable_type;
  // typedef gntl::concept::lookupable_traits<lookupable_type> lookupable_traits;
  // lookupable_type lookupable = structure_document_traits::media_lookup(*initialized_data_->document);
  // typedef lookupable_traits::result_type result_type;
  // typedef lookupable_traits::value_type media_type;
  // result_type r_m = lookupable_traits::lookup(lookupable, id);
  // if(r_m != lookupable_traits::not_found(lookupable))
  // {
  //   media_type m = *r_m;
  //   typedef structure_document_traits::context_lookupable lookupable_type;
  //   typedef gntl::concept::lookupable_traits<lookupable_type> lookupable_traits;
  //   lookupable_type lookupable = structure_document_traits::context_lookup(*initialized_data_->document);
  //   typedef lookupable_traits::result_type result_type;
  //   typedef lookupable_traits::value_type context_variant_type;
  //   result_type r_c = lookupable_traits::lookup(lookupable, c);
  //   if(r_c != lookupable_traits::not_found(lookupable))
  //   {
  //     context_variant_type c = *r_c;
  //     typedef structure_document_traits::descriptor_type descriptor_type;
  //     descriptor_type descriptor;
  //     //boost::apply_visitor(make_natural_end_aux(m, descriptor, gntl::ref(*initialized_data_->document), this), c);
  //   }
  // }
  // std::cout << "idle register" << std::endl;
  // idle_register();
}

void video_finished_event::finished_event()
{
  // if(!expired)
  // {
  //   if(executor_->frame.get())
  //     executor_->frame->hide();
  //   // ncl_widget::structure_document_traits::register_event
  //   //   (*this_->initialized_data_->document, id, context, boost::none, gntl::transition_enum::stops
  //   //    , gntl::event_enum::presentation);
  //   std::vector<video_finished_event*>::iterator
  //     it = std::find(this_->video_finished_events.begin(), this_->video_finished_events.end(), this);
  //   assert(it != this_->video_finished_events.end());
  //   this_->video_finished_events.erase(it);
  //   this_->cleanup_videos.push_back(this);
  //   this_->natural_end(id, context);
  //   // this_->idle_register();
  // }
}

void ncl_widget::idle_register()
{
  if(!idle_timer.get())
  {
    std::cout << "no idle timer" << std::endl;
    if(!events_paused)
    {
      idle_timer.reset(new QTimer);
      connect(idle_timer.get(), SIGNAL(timeout()), this, SLOT(processWhenIdle()));
      idle_timer->setSingleShot(true);
      idle_timer->start(0);
    }
  }
  else
    std::cout << "wont register" << std::endl;
}

bool ncl_widget::process_key(QKeyEvent* event, bool pressed)
{
  if(events_paused) return false;

  assert(event != 0);
  char buf[2] = "A";

  try
  {
    if(initialized_data_)
    {
      std::string key;
      if(event->key() == Qt::Key_F1)
        key = "RED";
      else if(event->key() == Qt::Key_F2)
        key = "GREEN";
      else if(event->key() == Qt::Key_F3)
        key = "YELLOW";
      else if(event->key() == Qt::Key_F4)
        key = "BLUE";
      else if(event->key() >= Qt::Key_A
              && event->key() <= Qt::Key_Z)
      {
        buf[0] = 'A' + event->key() - Qt::Key_A;
        key = buf;
      }
      else if(event->key() >= Qt::Key_0
              && event->key() <= Qt::Key_9)
      {
        buf[0] = '0' + event->key() - Qt::Key_0;
        key = buf;
      }
      else if(event->key() == Qt::Key_Left)
        key = "CURSOR_LEFT";
      else if(event->key() == Qt::Key_Up)
        key = "CURSOR_UP";
      else if(event->key() == Qt::Key_Right)
        key = "CURSOR_RIGHT";
      else if(event->key() == Qt::Key_Down)
        key = "CURSOR_DOWN";
      else if(event->key() == Qt::Key_Return)
        key = "ENTER";
      else if(event->key() == Qt::Key_Space)
        key = "INFO";
      else
        return false;

      if(!key.empty())
      {
        bool processed_current_key_master = false;
        if(structure_document_traits::has_current_key_master(*initialized_data_->document))
        {
          if(key == "BACK" && pressed)
          {
            // TODO: should unset current key master
          }
          else if(key != "BACK")
          {
            typedef structure_document_traits::property_type property_type;
            typedef gntl::concept::structure::property_traits<property_type> property_traits;
            
            if(structure_document_traits::has_property
               (*initialized_data_->document, "service.currentFocus")
               && property_traits::is_integer
               (structure_document_traits::get_property
                (*initialized_data_->document, "service.currentFocus"))
               && structure_document_traits::focused_media(*initialized_data_->document)
               == structure_document_traits::current_key_master(*initialized_data_->document)
               )
            {
              assert(structure_document_traits::has_focus(*initialized_data_->document));

              typedef structure_document_traits::focus_index_presentation_lookupable
                presentation_lookupable;
              presentation_lookupable lookupable = structure_document_traits::focus_index_presentation_lookup
                (*initialized_data_->document);
              typedef gntl::concept::lookupable_traits<presentation_lookupable> lookupable_traits;
              typedef lookupable_traits::result_type lookup_result;
              lookup_result r = lookupable_traits::lookup
                (lookupable
                 , property_traits::integer_value
                 (structure_document_traits::get_property
                  (*initialized_data_->document, "service.currentFocus")));

              if(r != lookupable_traits::not_found(lookupable))
              {
                typedef lookupable_traits::value_type lookup_value_type;
                lookup_value_type v = *r;
                typedef lookup_value_type::const_iterator iterator;
                for(iterator first = v.begin (), last = v.end (); first != last; ++first)
                {
                  typedef lookup_value_type::value_type value_type;
                  typedef value_type::second_type ref_presentation_type;
                  typedef gntl::unwrap_parameter<ref_presentation_type>::type
                    presentation_type;
                  typedef gntl::concept::structure::presentation_traits
                    <presentation_type> presentation_traits;
                  if(presentation_traits::is_occurring(first->second))
                  {
                    boost::unwrap_ref(boost::unwrap_ref(first->second).executor())
                      .key_process(key, pressed);
                    processed_current_key_master = true;
                    break;
                  }
                }
              }
              else
              {
                Q_EMIT error_occurred_signal("not found current key master presentation");
              }
            }
            
            if(!processed_current_key_master)
            {
              typedef structure_document_traits::media_lookupable
                media_lookupable;
              media_lookupable lookupable = structure_document_traits::media_lookup
                (*initialized_data_->document);
              typedef gntl::concept::lookupable_traits<media_lookupable> lookupable_traits;
              typedef lookupable_traits::result_type lookup_result;
              lookup_result r = lookupable_traits::lookup
                (lookupable
                 , structure_document_traits::current_key_master(*initialized_data_->document));
              if(r != lookupable_traits::not_found(lookupable))
              {
                typedef lookupable_traits::value_type media_value_type;
                typedef gntl::unwrap_parameter<media_value_type>::type media_type;
                typedef gntl::concept::structure::media_traits<media_type> media_traits;
                typedef media_traits::presentation_range presentation_range;
                presentation_range presentations = media_traits::presentation_all(*r);
                typedef boost::range_iterator<presentation_range>::type presentation_iterator;
                for(presentation_iterator first = boost::begin(presentations)
                      , last = boost::end(presentations); first != last; ++first)
                {
                  typedef boost::range_value<presentation_range>::type presentation_type;
                  typedef gntl::concept::structure::presentation_traits<presentation_type> presentation_traits;
                  if(presentation_traits::is_occurring(*first))
                  {
                    boost::unwrap_ref(boost::unwrap_ref(*first).executor())
                      .key_process(key, pressed);
                    processed_current_key_master = true;
                    break;
                  }
                  else
                  {
                    Q_EMIT error_occurred_signal("Not found a running presentation");
                  }
                }
                Q_EMIT error_occurred_signal("No running presentation was found");
              }
              else
              {
                std::string error = "Not found media for currentKeyMaster "
                  + structure_document_traits::current_key_master(*initialized_data_->document);
                Q_EMIT error_occurred_signal(error);
              }
            }
          }
        }
        if(pressed && !processed_current_key_master)
        {
          gntl::algorithm::structure::context::evaluate_select_links
            (gntl::ref(initialized_data_->document->body)
             , initialized_data_->document->body_location()
             , gntl::ref(*initialized_data_->document)
             , key, screen_dimensions);
          if(structure_document_traits::has_focus (*initialized_data_->document)
             && structure_document_traits::is_focus_bound(*initialized_data_->document))
          {
            typedef structure_document_traits::media_lookupable media_lookupable;
            media_lookupable lookupable = structure_document_traits::media_lookup(*initialized_data_->document);
            typedef gntl::concept::lookupable_traits<media_lookupable> lookupable_traits;
            typedef lookupable_traits::result_type lookup_result;

            lookup_result r = lookupable_traits::lookup
              (lookupable
               , structure_document_traits::focused_media(*initialized_data_->document));

            if(r != lookupable_traits::not_found(lookupable))
            {
              gntl::algorithm::structure::media::focus_select
                (*r, gntl::ref(*initialized_data_->document), key);
            }
            else
              Q_EMIT error_occurred_signal("Couldn't find focused media");
          }      
        }
      }
      processWhenIdle();
      return true;
    }
  }
  catch(boost::exception const& e)
  {
    Q_EMIT error_occurred_signal(boost::diagnostic_information(e));
  }
  catch(std::exception const& e)
  {
    Q_EMIT error_occurred_signal(boost::diagnostic_information(e));
  }

  return false;
}

void ncl_widget::processWhenIdle()
{
  if(events_paused) return;

  for(std::vector<video_finished_event*>::iterator
        first = cleanup_videos.begin(), last = cleanup_videos.end()
        ;first != last;++first)
  {
    if(!(*first)->expired)
    {
      (*first)->executor_->stop();
      delete *first;
    }
  }
  cleanup_videos.clear();

  try
  {
    typedef gntl::concept::structure
      ::document_traits<structure_document> document_traits;
    while(document_traits::pending_events(*initialized_data_->document))
    {
      document_traits::event_type event = document_traits::top_event (*initialized_data_->document);
      document_traits::pop_event (*initialized_data_->document);

      //setUpdatesEnabled(false);
      try
      {
      gntl::algorithm::structure::context::evaluate_links
        (gntl::ref(initialized_data_->document->body)
         , initialized_data_->document->body_location()
         , gntl::ref(*initialized_data_->document)
         , event, screen_dimensions);
      if(event.event_ == gntl::event_enum::attribution
         && event.transition == gntl::transition_enum::starts
         && event.interface_)
      {
        typedef document_traits::media_lookupable media_lookupable;
        media_lookupable lookupable = document_traits::media_lookup(*initialized_data_->document);
        typedef gntl::concept::lookupable_traits<media_lookupable> lookupable_traits;
        typedef lookupable_traits::result_type lookup_result;

        lookup_result r = lookupable_traits::lookup(lookupable, event.component_identifier);
        typedef lookupable_traits::value_type media_type;

        if(r != lookupable_traits::not_found(lookupable))
        {
          typedef document_traits::context_lookupable context_lookupable;
          context_lookupable lookupable = document_traits::context_lookup(*initialized_data_->document);
          typedef gntl::concept::lookupable_traits<context_lookupable> lookupable_traits;
          typedef lookupable_traits::result_type lookup_result;

          gntl::algorithm::structure::media::commit_set
            (*r, *event.interface_
             , gntl::ref(*initialized_data_->document));
        }
      }
      }
      catch(...)
      {
        setUpdatesEnabled(true);
        throw;
      }
      setUpdatesEnabled(true);
      repaint();
    }
    if(idle_timer.get())
    {
      idle_timer->stop();
      //idle_timer.release ()->deleteLater();
    }
  }
  catch(boost::exception const& e)
  {
    Q_EMIT error_occurred_signal(boost::diagnostic_information(e));
  }
  catch(std::exception const& e)
  {
    Q_EMIT error_occurred_signal(boost::diagnostic_information(e));
  }
}

} } }
