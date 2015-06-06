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

#ifndef GHTV_QT_RESOLUTION_ACTION_HPP
#define GHTV_QT_RESOLUTION_ACTION_HPP

#include <QAction>

#include <sstream>

namespace ghtv { namespace qt {

class resolution_action : public QAction
{
  Q_OBJECT
public:
  resolution_action(std::size_t w, std::size_t h, QObject* parent)
    : QAction(parent), width(w), height(h)
  {
    std::stringstream s;
    s << w << 'x' << h;
    setText(QString::fromStdString(s.str()));
    QObject::connect(this, SIGNAL(triggered(bool)), this, SLOT(triggered_impl(bool)));
  }

  std::size_t width, height;
Q_SIGNALS:
  void change_resolution(std::size_t width, std::size_t height, ghtv::qt::resolution_action*);
public Q_SLOTS:
  void triggered_impl(bool checked)
  {
    if(checked)
      Q_EMIT change_resolution(width, height, this);
  }
};

} }

#endif
