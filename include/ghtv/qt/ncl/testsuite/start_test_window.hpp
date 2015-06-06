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

#ifndef GHTV_QT_DEBUG_TESTSUITE_START_TEST_WINDOW_HPP
#define GHTV_QT_DEBUG_TESTSUITE_START_TEST_WINDOW_HPP

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QtNetwork>
#include <QUrl>

namespace ghtv { namespace qt { namespace ncl { namespace testsuite {

class start_test_window : public QWidget
{
  Q_OBJECT
public:
  start_test_window();

  QListWidget* tests_list;
  QLineEdit* line_edit;
  QPushButton *action_button, *remove_button, *down_button, *up_button;
  bool loaded, loading;
  QNetworkReply* reply;
Q_SIGNALS:
  void start();
public Q_SLOTS:
  void action_button_clicked();
  void remove_button_clicked();
  void up_button_clicked();
  void down_button_clicked();
  void address_changed(QString const&);
  void reply_finished();
  void reply_error(QNetworkReply::NetworkError code);
};

} } } }

#endif
