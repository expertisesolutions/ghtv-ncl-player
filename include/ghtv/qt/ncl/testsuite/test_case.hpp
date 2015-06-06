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

#ifndef GHTV_QT_TESTSUITE_TEST_CASE_HPP
#define GHTV_QT_TESTSUITE_TEST_CASE_HPP

#include <QString>

namespace ghtv { namespace qt { namespace ncl { namespace testsuite {

enum test_state
{
  test_passed, test_failed, test_untested
};

struct test_case
{
  QString test_url;
  QString test_name;
  test_state state;
  test_case(QString test_url, QString test_name)
    : test_url(test_url), test_name(test_name)
    , state(test_untested)
  {}
};

struct test_url_comparator
{
  typedef bool result_type;
  bool operator()(test_case const& lhs, test_case const& rhs) const
  {
    return lhs.test_url < rhs.test_url;
  }
};

} } } }

#endif
