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

#include <ghtv/qt/ncl/testsuite/testsuite_window.hpp>

#include <libxml/parser.h>

#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFileDialog>

#include <iostream>
#include <limits>
#include <cstring>
#include <cassert>

namespace ghtv { namespace qt { namespace ncl { namespace testsuite {

testsuite_window::testsuite_window()
  : start_test_window(0), running_test_window(0)
  , current_test(-1)
  , network_manager(new QNetworkAccessManager(this))
  , xml_reply(0)
{
  setAttribute(Qt::WA_DeleteOnClose);
  QHBoxLayout* layout = new QHBoxLayout;
  start_test_window = new testsuite::start_test_window;
  layout->addWidget(start_test_window);
  setLayout(layout);

  QObject::connect(start_test_window, SIGNAL(start()), this, SLOT(start()));
}

testsuite_window::~testsuite_window()
{
  Q_EMIT destroy_window();
}

void testsuite_window::run_testcase(std::size_t case_)
{
  std::cout << "run_testcase " << case_ << std::endl;

  delete running_test_window;
  running_test_window = 0;

  Q_EMIT stop_test();

  typedef test_case_container::nth_index<1>::type sequenced_index_type;
  sequenced_index_type& sequenced_index = test_cases.get<1>();
  typedef sequenced_index_type::iterator iterator;
  iterator test = boost::next(sequenced_index.begin(), case_);

  running_test_window = new testsuite::running_test_window(test->test_url);
  QObject::connect(running_test_window, SIGNAL(passed()), this, SLOT(passed()));
  QObject::connect(running_test_window, SIGNAL(failed()), this, SLOT(failed()));
  QObject::connect(running_test_window, SIGNAL(stop()), this, SLOT(stop()));
  QObject::connect(running_test_window, SIGNAL(code()), this, SLOT(code()));
  layout()->addWidget(running_test_window);
  layout()->invalidate();
  updateGeometry();

  QString ncl_file = test->test_url;
  std::cout << "ncl_file " << ncl_file.toStdString() << " testcase: " << case_ << std::endl;
  if(ncl_file.endsWith(".ncl"))
  {
    QString xml_file = ncl_file;
    xml_file.resize(xml_file.length()-4);
    xml_file.insert(xml_file.length(), ".xml");
    QUrl url = xml_file;
    assert(xml_reply == 0);
    xml_reply = network_manager->get(QNetworkRequest(url));
    connect(xml_reply, SIGNAL(finished()), this, SLOT(description_finished()));
    connect(xml_reply, SIGNAL(error(QNetworkReply::NetworkError)), this
            , SLOT(description_error(QNetworkReply::NetworkError)));
    running_test_window->passed_button->setEnabled(false);
  }

  Q_EMIT run_test(test->test_url);
}

void testsuite_window::start()
{
  {
    test_cases.clear();
    typedef test_case_container::nth_index<1>::type sequenced_index_type;
    sequenced_index_type& sequenced_index = test_cases.get<1>();
    for(std::size_t i = 0; i != start_test_window->tests_list->count(); ++i)
    {
      test_case c(start_test_window->tests_list->item(i)->text()
                  , start_test_window->tests_list->item(i)->text());
      sequenced_index.push_back(c);
    }
  }

  delete start_test_window;
  start_test_window = 0;

  current_test = 0;

  if(!test_cases.empty())
    run_testcase(0);
}

void testsuite_window::description_finished()
{
  QByteArray xml_content = xml_reply->readAll();
  const char* xml_data = xml_content.constData();

  ::xmlParserCtxtPtr parser_context = ::xmlNewParserCtxt();
  ::xmlDocPtr xmldoc = ::xmlCtxtReadMemory(parser_context
                                           , xml_data
                                           , xml_content.length()
                                           , "description.xml", 0, 0);
  if(xmldoc)
  {
    ::xmlNodePtr root = ::xmlDocGetRootElement(xmldoc);
    
    if(!std::strcmp(static_cast<const char*>(static_cast<const void*>(root->name)), "testcase"))
    {
      ::xmlNodePtr name_node = root->children;
      while(name_node && !(name_node->type == XML_ELEMENT_NODE
                                && !std::strcmp(static_cast<const char*>
                                   (static_cast<const void*>(name_node->name)), "name")))
        name_node = name_node->next;
      if(name_node)
      {
        const char* name = static_cast<const char*>
          (static_cast<const void*>
           ( ::xmlNodeGetContent(name_node)));
        if(name)
        {
          typedef test_case_container::nth_index<1>::type sequenced_index_type;
          sequenced_index_type& sequenced_index = test_cases.get<1>();
          typedef sequenced_index_type::iterator iterator;
          iterator test = boost::next(sequenced_index.begin(), current_test);

          QString name_string (name);
          test_case c = *test;
          c.test_name = name_string;
          sequenced_index.replace(test, c);
          running_test_window->test_name->setText(name_string);
        }
        else
          ;
      }
      else
        ;
      ::xmlNodePtr normative_node = root->children;
      while(normative_node && !(normative_node->type == XML_ELEMENT_NODE
                                && !std::strcmp(static_cast<const char*>
                                   (static_cast<const void*>(normative_node->name)), "normative-text")))
        normative_node = normative_node->next;
      if(normative_node)
      {
        const char* normative = static_cast<const char*>
          (static_cast<const void*>
           ( ::xmlNodeGetContent(normative_node)));
        if(normative)
        {
          QString string ("Normative: ");
          string += QString::fromUtf8(normative);
          running_test_window->normative->setHtml(string);
        }
        else
          ;
      }
      else
        ;

      ::xmlNodePtr instruction_node = root->children;
      while(instruction_node && !(instruction_node->type == XML_ELEMENT_NODE
                                && !std::strcmp(static_cast<const char*>
                                   (static_cast<const void*>(instruction_node->name)), "instruction-text")))
        instruction_node = instruction_node->next;
      if(instruction_node)
      {
        const char* instruction = static_cast<const char*>
          (static_cast<const void*>
           ( ::xmlNodeGetContent(instruction_node)));
        if(instruction)
        {
          QString string ("Instruction: ");
          string += QString::fromUtf8(instruction);
          running_test_window->instruction->setHtml(string);
        }
        else
          ;
      }
      else
        ;
    }
    else
    {
    }

    ::xmlFreeDoc(xmldoc);
  }
  else
  {
  }

  xml_reply = 0;
  running_test_window->passed_button->setEnabled(true);
}

void testsuite_window::description_error(QNetworkReply::NetworkError)
{
  if(running_test_window)
  {
    running_test_window->normative->setHtml
      ("Normative: <b>Couldn't download description</b>");
    running_test_window->instruction->setHtml
      ("Instruction: <b>Couldn't download description</b>");
  }
  xml_reply = 0;
  running_test_window->passed_button->setEnabled(true);
}

void testsuite_window::passed()
{
  std::cout << "passed current_test " << current_test << std::endl;
  typedef test_case_container::nth_index<1>::type sequenced_index_type;
  sequenced_index_type& sequenced_index = test_cases.get<1>();
  typedef sequenced_index_type::iterator iterator;
  iterator test = boost::next(sequenced_index.begin(), current_test);

  test_case c = *test;
  c.state = test_passed;
  sequenced_index.replace(test, c);

  if(current_test + 1 != test_cases.size())
    run_testcase(++current_test);
  else
    stop();
}

void testsuite_window::failed()
{
  std::cout << "failed current_test " << current_test << std::endl;
  typedef test_case_container::nth_index<1>::type sequenced_index_type;
  sequenced_index_type& sequenced_index = test_cases.get<1>();
  typedef sequenced_index_type::iterator iterator;
  iterator test = boost::next(sequenced_index.begin(), current_test);

  test_case c = *test;
  c.state = test_failed;
  sequenced_index.replace(test, c);

  if(current_test + 1 != test_cases.size())
    run_testcase(++current_test);
  else
    stop();
}

void testsuite_window::stop()
{
  delete running_test_window;
  running_test_window = 0;

  stopped_test_window = new testsuite::stopped_test_window
    (test_cases);

  QObject::connect(stopped_test_window, SIGNAL(save()), this, SLOT(save()));
  QObject::connect(stopped_test_window, SIGNAL(upload()), this, SLOT(upload()));
  
  layout()->addWidget(stopped_test_window);
  layout()->invalidate();
  updateGeometry();
}

void testsuite_window::code()
{
  
}

void testsuite_window::upload()
{
  
}

void testsuite_window::save()
{
  QString filename = QFileDialog::getSaveFileName(this, "Save report"
                                                  , QDir::currentPath()
                                                  , "CSV files (*.csv);;All Files (*.*)"
                                                  , 0, 0);

  if(filename.size())
  {
    QFile file(filename);
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);

    typedef test_case_container::nth_index<1>::type index_type;
    index_type& index = test_cases.get<1>();
    typedef index_type::iterator iterator;
    
    for(iterator first = index.begin(), last = index.end(); first != last; ++first)
    {
      char separator[] = ",";
      char breakline[] = "\r\n";
      file.write(static_cast<const char*>(static_cast<const void*>(first->test_name.toLatin1().constData())));
      file.write(separator);
      file.write(static_cast<const char*>(static_cast<const void*>(first->test_url.toLatin1().constData())));
      file.write(separator);
      if(first->state == test_passed)
        file.write("PASSED");
      else if(first->state == test_failed)
        file.write("FAILED");
      else
        file.write("SKIPPED");
      file.write(breakline);
    }
  }
}

} } } }
