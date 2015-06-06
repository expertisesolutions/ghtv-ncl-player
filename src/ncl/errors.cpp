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

#include <ghtv/qt/ncl/debug/errors.hpp>
#include <ghtv/qt/main_window.hpp>

#include <QVBoxLayout>
#include <QHeaderView>

namespace ghtv { namespace qt { namespace ncl { namespace debug {

errors::errors(main_window* main)
  : errors_table(0)
{
  setAttribute(Qt::WA_DeleteOnClose);
  QVBoxLayout* layout = new QVBoxLayout;
  errors_table = new QTableWidget(0, 1);
  QStringList header;
  header.push_back("Error message");
  errors_table->setHorizontalHeaderLabels(header);

  std::size_t table_size = 0;

  for(std::list<std::string>::const_iterator
        first = main->error_messages.begin()
        , last = main->error_messages.end()
        ;first != last; ++first)
  {
    std::size_t current_row = table_size++;
    errors_table->setRowCount(table_size);
    QString error (first->c_str());
    QTableWidgetItem* item = new QTableWidgetItem(error);
    errors_table->setItem(current_row, 0, item);
  }

  errors_table->resizeColumnsToContents();
  errors_table->resizeRowsToContents();
  errors_table->verticalHeader()->hide();
  errors_table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  errors_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  errors_table->horizontalHeader()->setStretchLastSection(true);

  if(errors_table->width() <= main->width())
    errors_table->setMinimumWidth(errors_table->width());
  else
    errors_table->setMinimumWidth(main->width());

  layout->addWidget(errors_table);
  setLayout(layout);

  // QObject::connect(main->ncl, SIGNAL(error_occurred_signal(std::string))
  //                  , this, SLOT(error_occurred(std::string)));
}

errors::~errors()
{
  Q_EMIT destroying_window_signal();
}

void errors::error_occurred(std::string error_msg)
{
  std::size_t current_row = errors_table->rowCount();
  errors_table->setRowCount(current_row+1);
  QString error (error_msg.c_str());
  QTableWidgetItem* item = new QTableWidgetItem(error);
  errors_table->setItem(current_row, 0, item);
}

} } } }
