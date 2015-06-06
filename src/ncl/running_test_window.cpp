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

#include <iostream>
#include <limits>
#include <cstring>
#include <cassert>

namespace ghtv { namespace qt { namespace ncl { namespace testsuite {

running_test_window::running_test_window(QString test)
  : test_name(0), passed_button(0), failed_button(0)
  , stop_button(0), code_button(0)
{
  QHBoxLayout* name_layout = new QHBoxLayout;
  QLabel* test_label = new QLabel("Name: ");
  test_label->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  name_layout->addWidget(test_label, Qt::AlignVCenter);
  test_name = new QLabel(test);
  test_name->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
  name_layout->addWidget(test_name, Qt::AlignVCenter);
  
  QHBoxLayout* buttons_layout = new QHBoxLayout;
  passed_button = new QPushButton("&Passed");
  QObject::connect(passed_button, SIGNAL(clicked()), this, SLOT(passed_clicked()));
  buttons_layout->addWidget(passed_button);
  failed_button = new QPushButton("&Failed");
  QObject::connect(failed_button, SIGNAL(clicked()), this, SLOT(failed_clicked()));
  buttons_layout->addWidget(failed_button);
  stop_button = new QPushButton("&Stop");
  QObject::connect(stop_button, SIGNAL(clicked()), this, SLOT(stop_clicked()));
  buttons_layout->addWidget(stop_button);
  code_button = new QPushButton("&Code");
  QObject::connect(code_button, SIGNAL(clicked()), this, SLOT(code_clicked()));

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addLayout(name_layout);

  normative = new QTextEdit;
  normative->setReadOnly(true);
  normative->setHtml("Normative:");
  layout->addWidget(normative);

  instruction = new QTextEdit;
  instruction->setReadOnly(true);
  instruction->setHtml("Instruction:");
  layout->addWidget(instruction);

  layout->addLayout(buttons_layout);
  setLayout(layout);
}

void running_test_window::passed_clicked()
{
  Q_EMIT passed();
}

void running_test_window::failed_clicked()
{
  Q_EMIT failed();
}

void running_test_window::stop_clicked()
{
  Q_EMIT stop();
}

void running_test_window::code_clicked()
{
  Q_EMIT code();
}

} } } }
