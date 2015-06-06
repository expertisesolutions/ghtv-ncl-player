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

#ifndef GHTV_QT_TESTSUITE_TESTSUITE_WINDOW_HPP
#define GHTV_QT_TESTSUITE_TESTSUITE_WINDOW_HPP

#include <ghtv/qt/ncl/testsuite/start_test_window.hpp>
#include <ghtv/qt/ncl/testsuite/running_test_window.hpp>
#include <ghtv/qt/ncl/testsuite/stopped_test_window.hpp>
#include <ghtv/qt/ncl/testsuite/test_case.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include <QWidget>

#include <vector>

namespace ghtv { namespace qt { namespace ncl { namespace testsuite {

class testsuite_window : public QWidget
{
  Q_OBJECT
public:
  testsuite_window();
  ~testsuite_window();                    

  testsuite::start_test_window* start_test_window;
  testsuite::running_test_window* running_test_window;
  testsuite::stopped_test_window* stopped_test_window;
  typedef boost::multi_index::multi_index_container
  <
    test_case
    , boost::multi_index::indexed_by
    <
      boost::multi_index::ordered_unique
      <boost::multi_index::identity<test_case>, test_url_comparator>
      , boost::multi_index::sequenced<>
    >
  >
  test_case_container;
  test_case_container test_cases;
  int current_test;
  QNetworkAccessManager* network_manager;
  QNetworkReply *xml_reply;

  void run_testcase(std::size_t case_);
Q_SIGNALS:
  void destroy_window();
  void run_test(QString);
  void stop_test();
public Q_SLOTS:
  void start();
  void description_finished();
  void description_error(QNetworkReply::NetworkError);
  void passed();
  void failed();
  void stop();
  void code();
  void save();
  void upload();
};

} } } }

#endif
