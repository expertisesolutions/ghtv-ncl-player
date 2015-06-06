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

#ifndef GHTV_QT_EXECUTOR_WIDGET_HPP
#define GHTV_QT_EXECUTOR_WIDGET_HPP

#include <ghtv/qt/ncl/executor.hpp>
#include <ghtv/qt/ncl/ncl_widget.hpp>

#include <QWidget>
#include <QMouseEvent>

namespace ghtv { namespace qt { namespace ncl {

class executor_widget : public QWidget
{
  Q_OBJECT
public:
  executor_widget(QWidget* parent, ncl::executor& executor)
    : QWidget(parent), executor(&executor)
  {}

  void mouseDoubleClickEvent(QMouseEvent* event)
  {
    if(event->type() == QEvent::MouseButtonDblClick)
    {
      //executor->root_window->media_clicked(executor->media_identifier, executor->context_identifier);
    }
  }

  ncl::executor* executor;
};

} } }

#endif
