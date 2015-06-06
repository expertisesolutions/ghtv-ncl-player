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

#include <ghtv/qt/ncl/testsuite/stopped_test_window.hpp>

#include <QVBoxLayout>
#include <QHeaderView>

#include <iostream>

namespace ghtv { namespace qt { namespace ncl { namespace testsuite {

stopped_test_window::stopped_test_window(test_case_container test_cases)
  : test_cases(test_cases)
{
  QVBoxLayout* layout = new QVBoxLayout;
  tests_list = new QTableWidget(0, 2);
  QStringList header;
  header.push_back("Test");
  header.push_back("Status");
  tests_list->setHorizontalHeaderLabels(header);

  typedef test_case_container::nth_index<1>::type index_type;
  index_type& index = this->test_cases.get<1>();
  typedef index_type::iterator iterator;
  for(iterator first = index.begin(), last = index.end(); first != last; ++first)
  {
    std::size_t size = tests_list->rowCount();
    tests_list->insertRow(size);

    QTableWidgetItem* name = new QTableWidgetItem(first->test_name);
    tests_list->setItem(size, 0, name);

    QString status_string;
    if(first->state == test_passed)
      status_string = "PASSED";
    else if(first->state == test_failed)
      status_string = "FAILED";
    else
      status_string = "SKIPPED";
    QTableWidgetItem* status = new QTableWidgetItem(status_string);
    tests_list->setItem(size, 1, status);
  }
  tests_list->verticalHeader()->hide();
  tests_list->resizeColumnsToContents();

  layout->addWidget(tests_list);
  QHBoxLayout* buttons_layout = new QHBoxLayout;

  save_button = new QPushButton ("&Save");
  buttons_layout->addWidget(save_button);
  
  QObject::connect(save_button, SIGNAL(clicked()), this, SLOT(save_clicked()));

  upload_button = new QPushButton ("&Upload");

  QObject::connect(upload_button, SIGNAL(clicked()), this, SLOT(upload_clicked()));

  layout->addLayout(buttons_layout);

  setLayout(layout);
}

void stopped_test_window::save_clicked()
{
  Q_EMIT save();
}

void stopped_test_window::upload_clicked()
{
  Q_EMIT upload();
}

} } } }
