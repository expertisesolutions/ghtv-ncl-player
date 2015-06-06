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

#ifndef GHTV_QT_PLAYER_CREATE_URL_HPP
#define GHTV_QT_PLAYER_CREATE_URL_HPP

#include <QUrl>
#include <QString>
#include <QFileInfo>

#include <cassert>

#include <iostream>

namespace ghtv { namespace qt { namespace ncl { namespace player {

inline QUrl create_url(std::string const& source, std::string const& root_path)
{
  QUrl url(source.c_str());
  if(url.scheme() == "")
  {
    if(url.isRelative())
    {
      QString path(root_path.c_str());
      path += "/";
      path += source.c_str();
      QUrl url(path);
      if(url.scheme() == "")
        return QUrl::fromLocalFile(path);
      else
        return url;
    }
    else
    {
      QUrl url = QUrl::fromLocalFile(QString::fromStdString(source));
      return url;
    }
  }
  else
  {
    return url;
  }
}

} } } }

#endif
