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

#ifndef GHTV_QT_TESTSUITE_STOPPED_TEST_WINDOW_HPP
#define GHTV_QT_TESTSUITE_STOPPED_TEST_WINDOW_HPP

#include <ghtv/qt/ncl/testsuite/test_case.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

namespace ghtv { namespace qt { namespace ncl { namespace testsuite {

class stopped_test_window : public QWidget
{
  Q_OBJECT
public:
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

  stopped_test_window(test_case_container test_cases);

  QTableWidget* tests_list;
  QPushButton *save_button, *upload_button;
Q_SIGNALS:
  void save();
  void upload();
public Q_SLOTS:
  void save_clicked();
  void upload_clicked();
};

} } } }

#endif
