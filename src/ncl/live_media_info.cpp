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

#include <ghtv/qt/ncl/debug/live_media_info.hpp>

#include <gntl/algorithm/structure/condition/fold.hpp>
#include <gntl/algorithm/structure/action/fold.hpp>

#include <boost/utility/enable_if.hpp>

namespace ghtv { namespace qt { namespace ncl { namespace debug {

namespace {

typedef live_media_info::link_type link_type;

struct table_link_identifier_item : QTableWidgetItem
{
  table_link_identifier_item (std::string const& string
                              , link_type link)
    : QTableWidgetItem(QString(string.c_str()))
    , link(link)
  {}
  
  link_type link;
};

struct add_properties_visitor
{
  typedef void result_type;
  template <typename Media>
  result_type operator()(Media media) const
  {
    typedef typename gntl::unwrap_parameter<Media>::type media_type;
    typedef gntl::concept::structure::media_traits<media_type> media_traits;
    typedef typename media_traits::property_range property_range;
    property_range properties = media_traits::property_all(media);
    typedef typename boost::range_iterator<property_range>::type property_iterator;
    for(property_iterator first = boost::begin(properties)
          , last = boost::end(properties);first != last; ++first)
    {
      typedef typename boost::range_value<property_range>::type property_type;
      typedef gntl::concept::structure::property_traits<property_type> property_traits;
      std::size_t size = self->properties_table->rowCount();
      self->properties_table->insertRow(size);

      std::string name_str = property_traits::name(*first);
      QString name (name_str.c_str());
      QTableWidgetItem* name_item = new QTableWidgetItem(name);
      self->properties_table->setItem(size, 0, name_item);

      std::string value_str = property_traits::value_as_string(*first);
      QString value (value_str.c_str());
      QTableWidgetItem* value_item = new QTableWidgetItem(value);
      self->properties_table->setItem(size, 1, value_item);
    }
  }
  
  live_media_info* self;
};

template <typename Identifier>
struct link_find_action
{
  link_find_action(Identifier id)
    : id(id) {}

  typedef bool result_type;
  template <typename SimpleAction>
  bool operator()(bool r, SimpleAction a) const
  {
    if(!r)
    {
      typedef typename gntl::unwrap_parameter<SimpleAction>::type simple_action_type;
      typedef gntl::concept::structure::bound_simple_action_traits<simple_action_type> simple_action_traits;
      typedef typename simple_action_traits::component_range component_range;
      component_range components = simple_action_traits::component_all(a);
      typedef typename boost::range_iterator<component_range>::type component_iterator;
      for(component_iterator first = boost::begin(components)
            , last = boost::end(components); first != last; ++first)
      {
        typedef typename boost::range_value<component_range>::type component_type;
        typedef gntl::concept::structure::action_bound_component_traits<component_type> component_traits;
        if(component_traits::component(*first) == id)
          return true;
      }
    }
    return r;
  }

  Identifier id;
};

template <typename Identifier>
struct link_evaluation
{
  link_evaluation(Identifier id)
    : id(id) {}

  struct enabler1 {};
  struct enabler2 {};

  typedef bool result_type;
  template <typename SimpleCondition>
  result_type operator()(bool r, SimpleCondition c
                         , typename boost::enable_if
                           <
                             typename gntl::concept::structure::bounded_simple_condition_traits
                             <typename gntl::unwrap_parameter<SimpleCondition>::type>
                             ::is_bounded_simple_condition
                           , enabler1>::type* = 0) const
  {
    if(!r)
    {
      typedef typename gntl::unwrap_parameter<SimpleCondition>::type simple_condition_type;
      typedef gntl::concept::structure::bounded_simple_condition_traits<simple_condition_type>
        simple_condition_traits;
      typedef typename simple_condition_traits::component_range component_range;
      component_range components = simple_condition_traits::components(c);
      typedef typename boost::range_iterator<component_range>::type component_iterator;
      for(component_iterator first = boost::begin(components), last = boost::end(components)
            ;first != last; ++first)
      {
        typedef typename boost::range_value<component_range>::type component_value_type;
        typedef typename gntl::unwrap_parameter<component_value_type>::type component_type;
        typedef gntl::concept::structure::condition_bound_component_traits<component_type> component_traits;
        if(component_traits::component(*first) == id)
        {
          return true;
        }
      }
    }
    return r;
  }

  template <typename AssessmentStatement>
  result_type operator()(bool r, AssessmentStatement c
                         , typename boost::enable_if
                           <
                             typename gntl::concept::structure::bound_assessment_statement_traits
                             <typename gntl::unwrap_parameter<AssessmentStatement>::type>
                             ::is_bound_assessment_statement
                           , enabler2>::type* = 0) const
  {
    return r;
  }

  Identifier id;
};

template <typename Media>
struct add_links_visitor
{
  typedef void result_type;
  template <typename Context>
  result_type operator()(Context context) const
  {
    typedef typename gntl::unwrap_parameter<Context>::type context_type;
    typedef gntl::concept::structure::context_traits<context_type> context_traits;
    typedef typename gntl::unwrap_parameter<Media>::type media_type;
    typedef gntl::concept::structure::media_traits<media_type> media_traits;
    typedef typename context_traits::link_range link_range;
    link_range links = context_traits::link_all(context);
    typedef typename boost::range_iterator<link_range>::type link_iterator;
    for(link_iterator first = boost::begin(links)
          , last = boost::end(links);first != last; ++first)
    {
      typedef typename boost::range_value<link_range>::type link_type;
      typedef gntl::concept::structure::link_traits<link_type> link_traits;

      bool condition = false;
      try
      {
        condition = (gntl::algorithm::structure::condition::fold
                     (link_traits::condition_expression(*first)
                      , false
                      , link_evaluation<typename media_traits::identifier_type>
                      (media_traits::identifier(media))));
      }
      catch(std::exception const&)
      {}
      bool action = false;
      try
      {
        action = (gntl::algorithm::structure::action::fold
                  (link_traits::action_expression(*first)
                   , false
                   , link_find_action<typename media_traits::identifier_type>
                   (media_traits::identifier(media))));
      }
      catch(std::exception const&)
      {}

      if(condition || action)
      {
        std::size_t size = self->links_table->rowCount();
        self->links_table->insertRow(size);

        std::string identifier_str;
        if(link_traits::has_identifier(*first))
          identifier_str = link_traits::identifier(*first);
        else
          identifier_str = "[no-identifier]";
        QTableWidgetItem* identifier_item = new table_link_identifier_item(identifier_str, *first);
        self->links_table->setItem(size, 0, identifier_item);

        QString relationship;
        if(condition && action)
          relationship = "conditional and action";
        else if(condition)
          relationship = "conditional";
        else if(action)
          relationship = "action";
        else
          relationship = "ERROR";

        self->links_table->setItem(size, 1
                                   , new QTableWidgetItem(relationship));
      }
    }
  }

  live_media_info* self;
  Media media;
};

}

live_media_info::live_media_info(std::string media_identifier, std::string context_identifier, ncl_widget* ncl
                                 , QWidget* parent)
  : QWidget(parent), properties_table(0), right_layout(0), media_code_widget(0)
  , link_code_widget(0), ncl(ncl)
{
  QVBoxLayout* left_layout = new QVBoxLayout;

  QString media_string = "Media: ";
  media_string += media_identifier.c_str();
  QLabel* media_label = new QLabel(media_string);
  left_layout->addWidget(media_label);

  QLabel* properties_label = new QLabel("properties:");
  left_layout->addWidget(properties_label);

  properties_table = new QTableWidget(0, 3);
  QStringList properties_header;
  properties_header.push_back("Name");
  properties_header.push_back("Value");
  properties_header.push_back("Intrinsic");
  properties_table->setHorizontalHeaderLabels(properties_header);

  typedef document_traits::media_lookupable media_lookupable;
  media_lookupable media_lookup = document_traits::media_lookup(*ncl->initialized_data_->document);
  typedef gntl::concept::lookupable_traits<media_lookupable> media_lookupable_traits;
  typedef media_lookupable_traits::result_type media_lookup_result;

  media_lookup_result media = media_lookupable_traits::lookup(media_lookup, media_identifier);
  assert(media != media_lookupable_traits::not_found(media_lookup));

  add_properties_visitor properties_visitor = {this};
  properties_visitor(*media);

  properties_table->resizeColumnsToContents();
  properties_table->verticalHeader()->hide();
  properties_table->horizontalHeader()->setStretchLastSection(true);

  left_layout->addWidget(properties_table);

  QLabel* links_label = new QLabel("links:");
  left_layout->addWidget(links_label);

  links_table = new QTableWidget(0, 2);
  QStringList links_header;
  links_header.push_back("Link name");
  links_header.push_back("Relationship");
  links_table->setHorizontalHeaderLabels(links_header);

  typedef document_traits::context_lookupable context_lookupable;
  context_lookupable context_lookup = document_traits::context_lookup(*ncl->initialized_data_->document);
  typedef gntl::concept::lookupable_traits<context_lookupable> context_lookupable_traits;
  typedef context_lookupable_traits::result_type context_lookup_result;

  context_lookup_result context = context_lookupable_traits::lookup(context_lookup, context_identifier);
  assert(context != context_lookupable_traits::not_found(context_lookup));

  add_links_visitor<media_lookupable_traits::value_type> link_visitor = {this, *media};
  (*context).apply_visitor(link_visitor);

  links_table->resizeColumnsToContents();
  links_table->verticalHeader()->hide();
  links_table->horizontalHeader()->setStretchLastSection(true);

  left_layout->addWidget(links_table);

  QObject::connect(links_table, SIGNAL(itemSelectionChanged()), this, SLOT(link_selection_changed()));

  QHBoxLayout* split_layout = new QHBoxLayout;
  split_layout->addLayout(left_layout);

  right_layout = new QVBoxLayout;

  media_lookupable_traits::value_type ref_media = *media;

  media_code_widget = create_media_code(boost::unwrap_ref(ref_media).parser_media.node);
  right_layout->addWidget(media_code_widget);

  split_layout->addLayout(right_layout);

  setLayout(split_layout);

  links_table->setCurrentCell(0,0);
}

QWidget* live_media_info::create_media_code(gntl::parser::libxml2::dom::xml_node media_node)
{
  const char* document_url = reinterpret_cast<char const*>
    (media_node.node->doc->URL);
  assert(document_url != 0);

  std::size_t line_number = media_node.node->line;
  std::string document_line = document_url;
  document_line += ':' + boost::lexical_cast<std::string>(line_number);
  QLabel* document_line_label = new QLabel(document_line.c_str());
  right_layout->addWidget(document_line_label);

  QFrame* code_frame = new QFrame;
  code_frame->setLineWidth(1);
  code_frame->setFrameShape(QFrame::Box);
  code_frame->setLayout(new QHBoxLayout);
  QTextEdit* code_widget = new QTextEdit();
  code_frame->layout()->addWidget(code_widget);

  const std::size_t lines = 5; 
  const std::size_t before_lines = 2;
  {
    if(line_number >= before_lines)
      line_number -= before_lines;
    std::ifstream file(document_url);
    if(file)
    {
      std::size_t current_line = 0;
      std::string line;
      while(current_line < line_number
            && std::getline(file, line))
        ++current_line;
      if(file.good() && !file.eof())
      {
        do
        {
          QTextCharFormat format;
          if(current_line == line_number + before_lines)
          {
            QBrush brush(QColor("red"));
            format.setForeground(brush);
          }
          code_widget->textCursor().insertText(line.c_str(), format);
          ++current_line;
        }
        while(current_line < (line_number + lines)
              && std::getline(file, line));
      }
    }
  }
  
  return code_frame;
}

QWidget* live_media_info::create_link_code(gntl::parser::libxml2::dom::xml_node link_node)
{
  QFrame* code_frame = new QFrame;
  code_frame->setLineWidth(1);
  code_frame->setFrameShape(QFrame::Box);
  code_frame->setLayout(new QHBoxLayout);
  QTextEdit* code_widget = new QTextEdit();
  code_frame->layout()->addWidget(code_widget);

  const char* document_url = reinterpret_cast<char const*>
    (link_node.node->doc->URL);

  std::size_t line_number = link_node.node->line;

  const std::size_t lines = 5; 
  const std::size_t before_lines = 2;
  {
    if(line_number >= before_lines)
      line_number -= before_lines;
    std::ifstream file(document_url);
    if(file)
    {
      std::size_t current_line = 0;
      std::string line;
      while(current_line < line_number
            && std::getline(file, line))
        ++current_line;
      if(file.good() && !file.eof())
      {
        do
        {
          QTextCharFormat format;
          if(current_line == line_number + before_lines)
          {
            QBrush brush(QColor("red"));
            format.setForeground(brush);
          }
          code_widget->textCursor().insertText(line.c_str(), format);
          ++current_line;
        }
        while(current_line < (line_number + lines)
              && std::getline(file, line));
      }
    }
  }
  return code_frame;
}

void live_media_info::link_selection_changed()
{
  delete link_code_widget;
  link_code_widget = 0;

  QTableWidgetItem* item = links_table->item(links_table->currentRow(), 0);
  assert(!!item);
  table_link_identifier_item* link_item = dynamic_cast<table_link_identifier_item*>(item);
  assert(!!link_item);

  link_code_widget = create_link_code(link_item->link.link_parser.node);
  right_layout->addWidget(link_code_widget);
}

} } } }
