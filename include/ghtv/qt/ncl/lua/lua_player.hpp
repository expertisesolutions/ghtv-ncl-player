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

#ifndef GHTV_QT_LUA_LUA_HPP
#define GHTV_QT_LUA_LUA_HPP

#include <ghtv/qt/ncl/player/player_base.hpp>
#include <ghtv/qt/ncl/player/url_file.hpp>

#include <gntl/structure/composed/component_location.hpp>

#include <lua.hpp>

#include <luabind/tag_function.hpp>
#include <luabind/luabind.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/utility.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/variant.hpp>

#include <stack>

#include <QThread>
#include <QWidget>
#include <QMutex>
#include <QEvent>
#include <QImage>
#include <QTimer>
#include <QPainter>

namespace ghtv { namespace qt { namespace ncl {

struct ncl_widget;

namespace lua {

class lua_socket;

struct repaint_event : QEvent
{
  repaint_event(QEvent::Type x) : QEvent(x) {}
};

class lua_widget : public QWidget
{
  Q_OBJECT
public:
  lua_widget(QWidget* widget, QSize size)
    : QWidget(widget), image(new QImage(size, QImage::Format_ARGB32))
  {
    repaint_event_type = static_cast<QEvent::Type>(QEvent::registerEventType());
    image->fill(0);
  }

  ~lua_widget();

  QMutex image_mutex;
  boost::shared_ptr<QImage> image;

  QEvent::Type repaint_event_type;

protected:
  void paintEvent(QPaintEvent* event);
  bool event(QEvent* event)
  {
    if(event->type() == repaint_event_type)
    {
      repaint();
      event->accept();
      return true;
    }
    else
      return QWidget::event(event);
  }  
};

class lua_player;

class lua_timer : public QTimer
{
  Q_OBJECT
  luabind::object function;
  lua_player* player;
public:
  lua_timer(luabind::object function, lua_player* player)
    : function(function), player(player) {}
public Q_SLOTS:
  void expired();
};

struct event_handler
{
  // luabind::type(handler) == LUA_TFUNCTION
  luabind::object handler;
  // if class_.empty() then filters.empty()
  std::string class_;
  std::vector<luabind::object> filters;
};

class activation_frame : public QObject
{
  Q_OBJECT
public:
  activation_frame(lua_State* thread, std::string file_base, std::string filename)
    : thread(thread), file_base(file_base), filename(filename) {}

  lua_State* thread;
  std::string file_base;
  std::string filename;

  void signal_execution_finished ();
Q_SIGNALS:
  void execution_finished ();
};

class lua_player : public QThread, public player::player_base
{
  Q_OBJECT
public:
  lua_player(std::string path, std::string root_path, QWidget& widget, ncl_widget* ncl_window
             , gntl::structure::composed::component_location<std::string, std::string>
             location)
    : L(0), path(path), root_path(root_path), widget(&widget)
    , window(new lua_widget(&widget, widget.size())), ncl_window(ncl_window)/*, identifier(identifier)*/
    , connection_identifier(0u), pending_download_file(0)
  {
    qRegisterMetaType<std::string>("std::string");

    moveToThread(this);
    
    if(boost::algorithm::starts_with(this->root_path, "file://"))
      this->root_path.erase(this->root_path.begin(), boost::next(this->root_path.begin(), 7));
    else if(boost::algorithm::starts_with(this->root_path, "file:"))
      this->root_path.erase(this->root_path.begin(), boost::next(this->root_path.begin(), 5));

    window->move(QPoint(0, 0));
    window->resize(widget.size());
    window->show();
    L = luaL_newstate();

    connect(this, SIGNAL(started()), this, SLOT(run_global_scope())
            , Qt::QueuedConnection);

    start();
  }

  void pause() {}
  void resume() {}

  void init_sandbox();
  void init_canvas();
  void init_event();

  void run()
  {
    connect(this, SIGNAL(key_process_signal(std::string, bool)), this
            , SLOT(key_process_queue(std::string, bool))
            , Qt::QueuedConnection);
    connect(this, SIGNAL(commit_set_property_signal(std::string, std::string)), this
            , SLOT(commit_set_property_queue(std::string, std::string))
            , Qt::QueuedConnection);
    connect(this, SIGNAL(start_area_signal(std::string)), this
            , SLOT(start_area_queue(std::string))
            , Qt::QueuedConnection);
    QThread::exec();
  }

  ~lua_player()
  {
    QThread::quit();
    this->wait();

    for(std::vector<lua_timer*>::iterator first = timers.begin()
          , last = timers.end(); first != last; ++first)
    {
      delete *first;
    }
    event_handlers.clear();

    lua_close(L);
  }

  void key_process(std::string const& key, bool pressed);
  bool wants_keys() const { return true; }
  void send_start_event();

  bool start_set_property(std::string const& name, std::string const& value);
  bool commit_set_property(std::string const& name);
  void start_area(std::string const& name);

  void activate_frame(std::string const& file_base, std::string const& filename);
  lua_State* current_frame();
  activation_frame* current_activation_frame();
  void pop_frame();

  struct keyboard_event
  {
    std::string key;
    bool press;
  };
  struct start_area_event
  {
    std::string name;
  };
  struct commit_set_property_event
  {
    std::string name, value;
  };

  lua_State* L;
  std::string path, root_path, lua_path;
  QUrl main_file_url;
  QWidget* widget;
  lua_widget* window;
  std::vector<lua_timer*> timers;
  std::vector<event_handler> event_handlers;
  boost::posix_time::ptime start_time;
  ncl_widget* ncl_window;
  //std::string identifier;
  unsigned int connection_identifier;
  std::map<unsigned int, lua_socket*> lua_sockets;
  std::map<std::string, std::string> properties_to_set;
  std::vector<activation_frame*> activated_frames;
  player::url_file* pending_download_file;
  typedef boost::variant<keyboard_event, start_area_event, commit_set_property_event> event_variant;
  std::deque<event_variant> queued_events;

  void run_current_frame(int args = 0);

  void run_require(std::string file);

  void commit_set_property_impl(std::string property, std::string value);
  void key_process_impl(std::string key, bool pressed);
  void start_area_impl(std::string name);
Q_SIGNALS:
  void key_process_signal(std::string key, bool pressed);
  void commit_set_property_signal(std::string property, std::string value);
  void start_area_signal(std::string name);
  void signal_all_execution_finished ();
  void download_lua_signal ();
  void resume_current_frame_signal (int args = 0);
  void run_require_signal (std::string file);
  void signal_try_unqueue_event ();

public Q_SLOTS:
  void commit_set_property_queue(std::string property, std::string value);
  void key_process_queue(std::string key, bool pressed);
  void start_area_queue(std::string name);

  void run_global_scope();
  void run_require_slot(std::string file);
  void run_dofile_slot(std::string file);

  void download_require();
  void download_global();
  void download_image();

  void download_error(std::string);

  void resume_current_frame(int args = 0);
  void require_finished ();
  void global_scope_finished ();
  void expired_finished ();

  void try_unqueue_event ();
  void run_queued_events ();
};

} } } }

#endif
