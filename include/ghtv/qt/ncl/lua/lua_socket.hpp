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

#ifndef GHTV_QT_LUA_LUA_SOCKET_HPP
#define GHTV_QT_LUA_LUA_SOCKET_HPP

#include <ghtv/qt/ncl/lua/lua_player.hpp>

#include <QTcpSocket>

namespace ghtv { namespace qt { namespace ncl { namespace lua {

class lua_socket : public QTcpSocket
{
  Q_OBJECT
public:
  lua_socket(lua_player* player, unsigned int connection_identifier
             , std::string host, int port)
    : QTcpSocket(player), player(player)
    , connection_identifier(connection_identifier)
    , host(host), port(port)
  {
    QObject::connect(this, SIGNAL(readyRead()), this, SLOT(socket_read()));
    QObject::connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this
                     , SLOT(socket_error(QAbstractSocket::SocketError)));
    QObject::connect(this, SIGNAL(disconnected()), this, SLOT(socket_disconnected()));
  }

private:
  lua_player* player;
  unsigned int connection_identifier;
  std::string host;
  int port;

public Q_SLOTS:
  void socket_connected();
  void socket_error(QAbstractSocket::SocketError);
  void socket_disconnected();
  void socket_read();
};

} } } }

#endif



