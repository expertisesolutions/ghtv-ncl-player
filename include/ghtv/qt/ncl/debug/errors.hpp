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

#ifndef GHTV_QT_DEBUG_ERRORS_HPP
#define GHTV_QT_DEBUG_ERRORS_HPP

#include <QWidget>
#include <QTableWidget>

namespace ghtv { namespace qt {

class main_window;

namespace ncl { namespace debug {

class errors : public QWidget
{
  Q_OBJECT
public:
  errors(main_window* main);
  ~errors();

  QTableWidget* errors_table;

Q_SIGNALS:
  void destroying_window_signal();
public Q_SLOTS:
  void error_occurred(std::string error_msg);
};

} } } }

#endif
