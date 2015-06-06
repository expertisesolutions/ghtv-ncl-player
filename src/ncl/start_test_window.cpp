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

#include <ghtv/qt/ncl/testsuite/start_test_window.hpp>

#include <libxml/parser.h>

#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include <iostream>
#include <limits>
#include <cstring>
#include <cassert>

namespace ghtv { namespace qt { namespace ncl { namespace testsuite {

start_test_window::start_test_window()
  : tests_list(0), line_edit(0), action_button(0), loaded(false)
  , loading(false), reply(0)
{
  QGridLayout* layout = new QGridLayout;

  tests_list = new QListWidget;
  tests_list->setSelectionMode(QAbstractItemView::ContiguousSelection);
  layout->addWidget(tests_list, 0, 0, 1, 3);

  remove_button = new QPushButton("&Remove");
  layout->addWidget(remove_button, 1, 0);

  up_button = new QPushButton("&Up");
  layout->addWidget(up_button, 1, 1);

  down_button = new QPushButton("&Down");
  layout->addWidget(down_button, 1, 2);

  QLabel* address = new QLabel("Address:");
  address->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  layout->addWidget(address, 2, 0, 1, 1);

  line_edit = new QLineEdit("http://testsuite.ghtv.com.br/gingancl/files/implementation");
  line_edit->setSizePolicy
    (QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));
  line_edit->setMinimumWidth(600);
  layout->addWidget(line_edit, 2, 1, 1, 1);

  action_button = new QPushButton("&Load");
  action_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  layout->addWidget(action_button, 2, 2, 1, 1);

  layout->setSpacing(1);

  setLayout(layout);

  QObject::connect(action_button, SIGNAL(clicked()), this, SLOT(action_button_clicked()));
  QObject::connect(remove_button, SIGNAL(clicked()), this, SLOT(remove_button_clicked()));
  QObject::connect(up_button, SIGNAL(clicked()), this, SLOT(up_button_clicked()));
  QObject::connect(down_button, SIGNAL(clicked()), this, SLOT(down_button_clicked()));
  QObject::connect(line_edit, SIGNAL(textChanged(QString const&)), this, SLOT(address_changed(QString const&)));

  remove_button->setEnabled(false);
  up_button->setEnabled(false);
  down_button->setEnabled(false);
}

void start_test_window::address_changed(QString const&)
{
  if(loaded)
  {
    loaded = false;
    tests_list->clear();
    action_button->setText("&Load!");

    remove_button->setEnabled(false);
    up_button->setEnabled(false);
    down_button->setEnabled(false);
  }
}

void start_test_window::remove_button_clicked()
{
  assert(loaded);

  std::cout << "start_test_window::remove_button_clicked" << std::endl;
  QList<QListWidgetItem*> selected = tests_list->selectedItems();

  std::cout << "Selected " << selected.size() << " items" << std::endl;
  
  for(QList<QListWidgetItem*>::iterator first = selected.begin()
        , last = selected.end(); first != last; ++first)
    delete *first;
}

void start_test_window::up_button_clicked()
{
  assert(loaded);

  QList<QListWidgetItem*> selected = tests_list->selectedItems();
  if(!selected.empty())
  {
    std::size_t row_first = tests_list->row(selected.front())
      , row_last = tests_list->row(selected.back());

    if(row_first) // if = 0 then we can't go up
    {
      std::size_t from = row_first - 1, to = row_last;
      QListWidgetItem* item = tests_list->takeItem(from);
      assert(item != 0);
      tests_list->insertItem(to, item);
    }
  }
}

void start_test_window::down_button_clicked()
{
  assert(loaded);

  QList<QListWidgetItem*> selected = tests_list->selectedItems();
  if(!selected.empty())
  {
    std::size_t row_first = tests_list->row(selected.front())
      , row_last = tests_list->row(selected.back());

    if(row_last + 1 != tests_list->count()) // if = 0 then we can't go up
    {
      std::size_t from = row_last + 1, to = row_first;
      QListWidgetItem* item = tests_list->takeItem(from);
      assert(item != 0);
      tests_list->insertItem(to, item);
    }
  }
}

void start_test_window::action_button_clicked()
{
  if(!loaded)
  {
    QNetworkAccessManager* network_manager = new QNetworkAccessManager(this);
    QString url = line_edit->text();
    url += "/tests.txt";
    reply = network_manager->get(QNetworkRequest(url));

    connect(reply, SIGNAL(finished()), this, SLOT(reply_finished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this
            , SLOT(reply_error(QNetworkReply::NetworkError)));

    loading = true;
    action_button->setEnabled(false);
    line_edit->setEnabled(false);
    remove_button->setEnabled(false);
    up_button->setEnabled(false);
    down_button->setEnabled(false);
  }
  else
    Q_EMIT start();
}

void start_test_window::reply_finished()
{
  {
    QIODevice* device = reply;

    QByteArray line;
    do
    {
      line = device->readLine((std::numeric_limits<unsigned short>::max)());
      if(line.length())
        tests_list->addItem(QString(line.trimmed()));
    }
    while(line.length());
  }

  loaded = true;
  loading = false;
  action_button->setText("&Start!");
  action_button->setEnabled(true);
  line_edit->setEnabled(true);
  remove_button->setEnabled(true);
  up_button->setEnabled(true);
  down_button->setEnabled(true);
}

void start_test_window::reply_error(QNetworkReply::NetworkError code)
{
  //  xml_reply = 0;
}

} } } }
