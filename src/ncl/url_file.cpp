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

#include <ghtv/qt/ncl/player/url_file.hpp>

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

struct filename_tag {};

url_file::url_file(QUrl url, QObject* parent)
  : QObject(parent), url(url)
  , network_manager(new QNetworkAccessManager(this))
  , reply(0), local_file(0)
{
  assert(url.scheme() != "");
  if(url.scheme() == "file") // local file
  {
    local_file = new QFile(url.toLocalFile());
    if(!local_file->open(QFile::ReadOnly))
    {
      BOOST_THROW_EXCEPTION((::boost::enable_error_info
                             (std::runtime_error("Failed opening file as read-only"))
                             << boost::error_info<filename_tag, std::string>
                             (url.path().toStdString())));
    }
    assert(local_file->isOpen());
  }
  else
  {
    reply = network_manager->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(reply_finished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this
            , SLOT(reply_error(QNetworkReply::NetworkError)));
  }
}

bool url_file::local() const
{
 return local_file;
}

QIODevice* url_file::io_device()
{
  assert(reply || local_file);
  if(reply)
    return reply;
  else
    return local_file;
}

void url_file::reply_finished()
{
  Q_EMIT download_finished_signal();
}

void url_file::reply_error(QNetworkReply::NetworkError code)
{
  std::string error = "Error downloading url: ";
  error += url.toString().toStdString();
  Q_EMIT error_signal(error);
}

} } } }
