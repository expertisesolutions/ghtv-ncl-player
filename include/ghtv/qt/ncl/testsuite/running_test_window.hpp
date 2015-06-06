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

#ifndef GHTV_QT_TESTSUITE_RUNNING_TEST_WINDOW_HPP
#define GHTV_QT_TESTSUITE_RUNNING_TEST_WINDOW_HPP

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

namespace ghtv { namespace qt { namespace ncl { namespace testsuite {

class running_test_window : public QWidget
{
  Q_OBJECT
public:
  running_test_window(QString tests);

  QLabel* test_name;
  QPushButton *passed_button, *failed_button, *stop_button, *code_button;
  QTextEdit *normative, *instruction;

Q_SIGNALS:
  void passed();
  void failed();
  void stop();
  void code();
public Q_SLOTS:
  void passed_clicked();
  void failed_clicked();
  void stop_clicked();
  void code_clicked();
};

} } } }

#endif
