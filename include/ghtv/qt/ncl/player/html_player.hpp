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

#ifndef GHTV_QT_PLAYER_HTML_PLAYER_HPP
#define GHTV_QT_PLAYER_HTML_PLAYER_HPP

#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/player/common_player.hpp>
#include <ghtv/qt/ncl/player/create_url.hpp>

#include <QWidget>
#include <QWebView>
#include <QUrl>

#include <stdexcept>

namespace ghtv { namespace qt { namespace ncl { namespace player {

class html_player : public common_player
{
  Q_OBJECT;
public:
  html_player(std::string const& source, std::string const& root_path, QWidget* player_container
              , ncl_widget* ncl, gntl::structure::composed::component_location
               <std::string, std::string> location
               , ncl::executor* e)
    : common_player(e, location), ncl(ncl), web_view(0)
  {
    std::auto_ptr<QWebView> web_view(new QWebView(player_container));
    web_view->move(QPoint(0, 0));
    web_view->resize(player_container->size());
    web_view->load(create_url(source, root_path));
    web_view->show();
    QObject::connect(web_view.get(), SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    this->web_view = web_view.release();
  }

  void pause() {}
  void resume() {}

  void key_process(std::string const& key, bool pressed) {}
  
  bool wants_keys() const { return false;  }
  bool start_set_property(std::string const& name, std::string const& value)
  {
    return false;
  }
  bool commit_set_property(std::string const& name)
  {
    return false;
  }
  void start_area(std::string const& name) {}

  ncl_widget* ncl;
  QWebView* web_view;
public Q_SLOTS:
  void loadFinished(bool ok)
  {
    if(ok)
      setup_timers();
    else
    {
      ncl->error_occurred("Error loading HTML");
    }
  }
};

} } } }

#endif
