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

#ifndef GHTV_QT_PLAYER_DOWNLOAD_URL_HPP
#define GHTV_QT_PLAYER_DOWNLOAD_URL_HPP

// This include order is necessary for MSVC 2010
#include <boost/exception/exception.hpp>
#include <boost/exception/enable_error_info.hpp>
#include <boost/detail/sp_typeinfo.hpp>
#include <boost/exception/get_error_info.hpp>
#include <boost/exception/info.hpp>
#include <boost/throw_exception.hpp>

#include <QtNetwork>
#include <QUrl>

#include <cassert>
#include <stdexcept>

namespace ghtv { namespace qt { namespace ncl { namespace player {

class url_file : public QObject
{
  Q_OBJECT
public:
  url_file(QUrl url, QObject* parent);

  QUrl url;
  QNetworkAccessManager* network_manager;
  QNetworkReply* reply;
  QFile* local_file;

  bool local() const;
  QIODevice* io_device();

Q_SIGNALS:
  void download_finished_signal();
  void error_signal(std::string);
public Q_SLOTS:
  void reply_finished();
  void reply_error(QNetworkReply::NetworkError code);
};

} } } }

#endif
