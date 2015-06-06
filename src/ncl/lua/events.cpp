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

#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/lua/lua_player.hpp>
#include <ghtv/qt/ncl/lua/lua_socket.hpp>
#include <ghtv/qt/ncl/initialized_data.hpp>

#include <luabind/out_value_policy.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/integer.hpp>
#include <boost/utility.hpp>
#include <boost/thread.hpp>

#include <QTimer>
#include <QTcpSocket>

namespace ghtv { namespace qt { namespace ncl { namespace lua {

void lua_timer::expired()
{
  assert(player->current_frame() == 0);

  std::cout << "lua_timer::expired" << std::endl;

  try
  {
    player->activate_frame(player->path, player->main_file_url.toString ().toStdString ());
#ifndef NDEBUG
    int old_top = lua_gettop(player->current_frame ());
#endif
    function.push(function.interpreter());
    lua_xmove(function.interpreter(), player->current_frame(), 1);
    assert(old_top + 1 == lua_gettop(player->current_frame ()));

    QObject::connect(player->current_activation_frame (), SIGNAL(execution_finished ())
                     , player, SLOT (expired_finished ())
                     , Qt::QueuedConnection);

    player->run_current_frame();
  }
  catch(luabind::error& e)
  {
    std::string error = "Error running lua timer event: ";
    if(lua_gettop(e.state()) != 0)
      error += lua_tostring(e.state(), -1);
    player->ncl_window->error_occurred(error);
  }

  std::vector<lua_timer*>::iterator
    iterator = std::find(player->timers.begin(), player->timers.end(), this);
  assert(iterator != player->timers.end());
  assert(*iterator == this);
  player->timers.erase(iterator);
  delete this;
}

namespace {

namespace event {

  bool post_out(luabind::object event, std::string& error_msg, lua_player* player)
  {
    try
    {
      std::string class_;
      if(event["class"] && luabind::type(event["class"]) == LUA_TSTRING)
        class_ = luabind::object_cast<std::string>(event["class"]);
        
      if(class_ == "ncl")
      {
        typedef ncl_widget::structure_document_traits::component_identifier identifier_type;
        boost::optional<identifier_type> area;
        if(event["label"])
        {
          area = luabind::object_cast<identifier_type>(event["label"]);
        }
        gntl::transition_type t = gntl::transition_enum::starts;
        if(event["action"])
        {
          luabind::object action = event["action"];
          if(luabind::type(action) == LUA_TSTRING)
          {
            std::string a = luabind::object_cast<std::string>(action);
            if(a == "start")
              ;
            else if(a == "stop")
              t = gntl::transition_enum::stops;
            else if(a == "abort")
              t = gntl::transition_enum::aborts;
            else if(a == "pause")
              t = gntl::transition_enum::pauses;
            else if(a == "resume")
              t = gntl::transition_enum::resumes;
          }
        }
        // ncl_widget::structure_document_traits::register_event
        //   (*player->ncl_window->initialized_data_->document
        //    , player->identifier
        //    , area, t, gntl::event_enum::presentation);
        player->ncl_window->idle_register();
        return true;
      }
      else if(class_ == "tcp")
      {
        if(event["type"])
        {
          std::string type;
          if(luabind::type(event["type"]) == LUA_TSTRING)
            type = luabind::object_cast<std::string>(event["type"]);
          if(type == "connect")
          {
            if(luabind::type(event["host"]) == LUA_TSTRING)
            {
              std::string host = luabind::object_cast<std::string>(event["host"]);
              if(luabind::type(event["port"]) == LUA_TNUMBER)
              {
                int port = luabind::object_cast<int>(event["port"]);
                ghtv::qt::ncl::lua::lua_socket* socket = new ghtv::qt::ncl::lua::lua_socket
                  (player, player->connection_identifier, host, port);
                player->lua_sockets[player->connection_identifier] = socket;
                QObject::connect(socket, SIGNAL(connected()), socket, SLOT(socket_connected()));
                QString host_(host.c_str());
                ++player->connection_identifier;
                socket->connectToHost(host_, port);
              }
            }

            error_msg = "Invalid host";
            return false;
          }
          else if(type == "data")
          {
            if(luabind::type(event["connection"]) == LUA_TNUMBER)
            {
              unsigned int connection_identifier = luabind::object_cast<int>(event["connection"]);
              std::map<unsigned int, lua_socket*>::iterator iterator
                = player->lua_sockets.find(connection_identifier);
              if(iterator != player->lua_sockets.end())
              {
                lua_socket* socket = iterator->second;

                if(luabind::type(event["value"]) == LUA_TSTRING)
                {
                  std::string value = luabind::object_cast<std::string>(event["value"]);
                  qint64 size = socket->write(value.c_str(), value.size());
                  if(size > 0)
                  {
                    qint64 value_size = value.size(); // Avoid warning
                    if(size == value_size)
                    {
                      return true;
                    }
                    else
                    {
                      error_msg = "Implementation TODO: should retry";
                      return false;
                    }
                  }
                  else if(size == 0)
                  {
                    error_msg = "Other side disconnected";
                    return false;
                  }
                  else if(size < 0)
                  {
                    error_msg = "Error sending value";
                    return false;
                  }
                }
                else
                {
                  error_msg = "Invalid value member";
                  return false;
                }
              }
              else
              {
                error_msg = "Invalid connection identifier";
                return false;
              }
            }
            else
            {
              error_msg = "Unknown connection identifier";
              return false;
            }
          }
          else
          {
            error_msg = "Unknown type for tcp class";
            return false;
          }
        }
        else
        {
          error_msg = "Not found type";
          return false;
        }
      }
      error_msg = "Unknown class type";
      return false;
    }
    catch(luabind::error const& e)
    {
      std::string error = "Error running event.post function: ";
      if(lua_gettop(e.state()) != 0)
        error += lua_tostring(e.state(), -1);
      player->ncl_window->error_occurred(error);
    }
    return false;
  }
  bool post(std::string dst, luabind::object event, std::string& error_msg
            , lua_player* player)
  {
    return post_out(event, error_msg, player);
  }
  luabind::object timer(int time, luabind::object f, lua_player* player)
  {
    if(luabind::type(f) != LUA_TFUNCTION)
    {
      lua_pushnil(f.interpreter());
      luabind::object nil(luabind::from_stack(f.interpreter (), -1));
      return nil;
    }
    else
    {
      lua_timer* timer = 0;

      timer = new lua_timer(f, player);

      player->timers.push_back(timer); // transfer ownership
      QObject::connect(timer, SIGNAL(timeout()), timer, SLOT(expired()));
      timer->start(time);

      lua_pushnil(f.interpreter());
      luabind::object nil(luabind::from_stack(f.interpreter(), -1));
      return nil;
    }
  }

  void cxx_register(lua_State* L, int pos, luabind::object handler
                    , std::string class_
                    , std::vector<luabind::object> filters, lua_player* player)
  {
    std::vector<event_handler>::iterator iterator = player->event_handlers.end();
    std::size_t unsigned_pos = pos;
    if(pos >= 0 && unsigned_pos <= player->event_handlers.size())
    {
      iterator = boost::next(player->event_handlers.begin(), pos);
    }

    handler.push(handler.interpreter());
    lua_xmove(handler.interpreter(), L, 1);
    luabind::object handler_(luabind::from_stack(L, -1));

    filters.clear();
    
    event_handler h = {handler_, class_, filters};
    player->event_handlers.insert(iterator, h);
  }
  
  int register_(lua_State* L)
  {
    try
    {
      lua_player* player = luabind::object_cast<lua_player*>(luabind::registry(L)["lua_player"]);
      assert(L == player->current_frame());
      try
      {
        if(lua_gettop(L) < 1)
        {
          lua_pushstring(L, "Not enough arguments");
          lua_error(L);
        }
        else if(lua_gettop(L) >= 1)
        {
          int i = 1;
          int pos = -1;
          luabind::object first(luabind::from_stack(L, i));
          luabind::object handler;
          if(luabind::type(first) == LUA_TFUNCTION)
            handler = first;
          else if(luabind::type(first) == LUA_TNUMBER && lua_gettop(L) == 1)
          {
            lua_pushstring(L, "Missing handler function argument");
            lua_error(L);
          }
          else if(luabind::type(first) == LUA_TNUMBER)
          {
            pos = luabind::object_cast<int>(first);
            handler = luabind::object(luabind::from_stack(L, ++i));
            if(luabind::type(handler) != LUA_TFUNCTION)
            {
              lua_pushstring(L, "Missing handler function argument (must be first or second argument)");
              lua_error(L);
            }
          }
          else
          {
            lua_pushstring(L, "First argument must be a position or a handler function)");
            lua_error(L);
          }

          std::string class_;
          ++i;
          if(lua_gettop(L) >= i)
          {
            luabind::object class_obj(luabind::from_stack(L, i));
            if(luabind::type(class_obj) != LUA_TSTRING)
            {
              lua_pushstring(L, "You must specifiy the event class before the filters");
              lua_error(L);
            }
            else
              class_ = luabind::object_cast<std::string>(class_obj);
          }

          std::vector<luabind::object> filters;
          for(++i;i <= lua_gettop(L);++i)
          {
            filters.push_back(luabind::object(luabind::from_stack(L, i)));
          }

          cxx_register(L, pos, handler, class_, filters, player);
        }
      }
      catch(luabind::error const& e)
      {
        std::string error = "Error running function event.register:\n";
        if(lua_gettop(e.state()) != 0)
          error += lua_tostring(e.state(), -1);
        player->ncl_window->error_occurred(error);
      }
      catch(std::exception const& e)
      {
        std::string error = "Error running function event.register:\n";
        error += e.what();
        player->ncl_window->error_occurred(error);
      }
    }
    catch(std::exception&)
    {
      std::abort();
    }

    return 0;
  }
  int uptime(lua_player* player)
  {
    int uptime_ = (boost::posix_time::microsec_clock::universal_time() - player->start_time)
      .total_milliseconds();
    return uptime_;
  }
}


}

void lua_player::init_event()
{
  luabind::registry(L)["lua_player"] = this;

  using luabind::tag_function;
  luabind::module(L, "event")
  [
   luabind::def("post", tag_function<bool(luabind::object, std::string&)>
                (boost::bind(&event::post_out, _1, _2, this))
                , luabind::pure_out_value(_2))
   // , luabind::def("post", tag_function<bool(std::string, luabind::object, std::string&)>
   //                (boost::bind(&event::post, _1, _2, _3, this))
   //                , luabind::pure_out_value(_3))
   , luabind::def("timer", tag_function<luabind::object(int, luabind::object)>
                  (boost::bind(&event::timer, _1, _2, this)))
   , luabind::def("uptime", tag_function<int()>(boost::bind(&event::uptime, this)))
  ];

  lua_pushcfunction(L, &event::register_);
  luabind::object register_(luabind::from_stack(L, -1));
  luabind::globals(L)["event"]["register"] = register_;
}

void lua_player::key_process_queue(std::string key, bool pressed)
{
  keyboard_event e = {key, pressed};
  queued_events.push_back(e);
  Q_EMIT signal_try_unqueue_event ();
}

void lua_player::key_process_impl(std::string key, bool pressed)
{
  std::cout << "lua_player::key_process_impl" << std::endl;
  assert(current_frame () == 0);
#ifndef NDEBUG
  int old_top = lua_gettop(L);
#endif
  for(std::vector<event_handler>::const_iterator first = event_handlers.begin()
        , last = event_handlers.end(); first != last; ++first)
  {
    if(first->class_.empty() || first->class_ == "key")
    {
      try
      {
#ifndef NDEBUG
        int old_top = lua_gettop(L);
#endif

        lua_newtable(L);
        luabind::object event(luabind::from_stack(L, -1));
        lua_pop(L, 1); // pop newtable

        event["class"] = "key";
        if(pressed)
          event["type"] = "press";
        else
          event["type"] = "release";
        event["key"] = key;

        luabind::setmetatable(event, event);

        assert(luabind::type(first->handler) == LUA_TFUNCTION);
        luabind::object r = luabind::call_function<luabind::object>(first->handler, event);
        if(luabind::type(r) == LUA_TBOOLEAN && luabind::object_cast<bool>(r))
           break;

        assert(lua_gettop(L) == old_top);
      }
      catch(luabind::error const& e)
      {
        std::string error = "Error running handler for key: ";
        if(lua_gettop(e.state()) != 0)
          error += lua_tostring(e.state(), -1);
        ncl_window->error_occurred(error);
      }
      catch(std::exception const& e)
      {
        std::string error = "Error running handler for key: ";
        error += e.what();
        ncl_window->error_occurred(error);
      }
    }
  }
  assert(lua_gettop(L) == old_top);
}

void lua_player::key_process(std::string const& key, bool pressed)
{
  Q_EMIT key_process_signal(key, pressed);
}

void lua_player::send_start_event()
{
  assert(current_frame () == 0);
#ifndef NDEBUG
  int old_top = lua_gettop(L);
#endif
  try
  {
    for(std::vector<event_handler>::const_iterator first = event_handlers.begin()
          , last = event_handlers.end(); first != last; ++first)
    {
      if(first->class_.empty() || first->class_ == "ncl")
      {
        lua_newtable(L);
        luabind::object event(luabind::from_stack(L, -1));
        lua_pop(L, 1); // pop newtable

        event["class"] = "ncl";
        event["type"] = "presentation";
        event["action"] = "start";
        // label == nil, meaning it is specific to the whole content anchor

        luabind::setmetatable(event, event);

        assert(luabind::type(first->handler) == LUA_TFUNCTION);
        luabind::object r = luabind::call_function<luabind::object>(first->handler, event);
        if(luabind::type(r) == LUA_TBOOLEAN && luabind::object_cast<bool>(r))
          break;
      }
    }
  }
  catch(luabind::error const& e)
  {
    std::string error = "Error running handler presentation event:\n";
    if(lua_gettop(e.state()) != 0)
      error += lua_tostring(e.state(), -1);
    ncl_window->error_occurred(error);
  }
  catch(std::exception const& e)
  {
    std::string error = "Error running handler presentation event:\n";
    error += e.what();
    ncl_window->error_occurred(error);
  }
  assert(lua_gettop(L) == old_top);
}

void lua_socket::socket_connected()
{
  assert(player->current_frame () == 0);
#ifndef NDEBUG
  int old_top = lua_gettop(player->L);
#endif
  try
  {
    for(std::vector<event_handler>::const_iterator first = player->event_handlers.begin()
          , last = player->event_handlers.end(); first != last; ++first)
    {
      if(first->class_.empty() || first->class_ == "tcp")
      {
        lua_newtable(player->L);
        luabind::object event(luabind::from_stack(player->L, -1));
        lua_pop(player->L, 1); // pop newtable

        event["class"] = "tcp";
        event["type"] = "connect";
        event["host"] = host;
        event["port"] = port;
        event["connection"] = connection_identifier;

        luabind::setmetatable(event, event);

        assert(luabind::type(first->handler) == LUA_TFUNCTION);
        luabind::object r = luabind::call_function<luabind::object>(first->handler, event);
        if(luabind::type(r) == LUA_TBOOLEAN && luabind::object_cast<bool>(r))
          break;
      }
    }
  }
  catch(luabind::error const& e)
  {
    std::string error = "Error in handler for connecting socket:\n";
    if(lua_gettop(e.state()) != 0)
      error += lua_tostring(e.state(), -1);
    player->ncl_window->error_occurred(error);
  }
  catch(std::exception const& e)
  {
    std::string error = "Error while connecting socket:\n";
    error += e.what();
    player->ncl_window->error_occurred(error);
  }
  assert(lua_gettop(player->L) == old_top);
}

void lua_socket::socket_read()
{
  QByteArray byte_array = readAll();

  std::size_t size = byte_array.size();
  const char* data = byte_array.data();
  std::string string(data, data + size);

  assert(player->current_frame () == 0);
#ifndef NDEBUG
  int old_top = lua_gettop(player->L);
#endif

  try
  {
    for(std::vector<event_handler>::const_iterator first = player->event_handlers.begin()
          , last = player->event_handlers.end(); first != last; ++first)
    {
      if(first->class_.empty() || first->class_ == "tcp")
      {
        lua_newtable(player->L);
        luabind::object event(luabind::from_stack(player->L, -1));
        lua_pop(player->L, 1); // pop newtable

        event["class"] = "tcp";
        event["type"] = "data";
        event["connection"] = connection_identifier;
        event["value"] = string;

        luabind::setmetatable(event, event);

        assert(luabind::type(first->handler) == LUA_TFUNCTION);
        luabind::object r = luabind::call_function<luabind::object>(first->handler, event);
        if(luabind::type(r) == LUA_TBOOLEAN && luabind::object_cast<bool>(r))
          break;
      }
    }
  }
  catch(luabind::error const& e)
  {
    std::string error = "Error running handler received data on socket:\n";
    if(lua_gettop(e.state()) != 0)
      error += lua_tostring(e.state(), -1);
    player->ncl_window->error_occurred(error);
  }
  catch(std::exception const& e)
  {
    std::string error = "Error while running handler received data on socket:\n";
    error += e.what();
    player->ncl_window->error_occurred(error);
  }
  assert(lua_gettop(player->L) == old_top);
}

void lua_socket::socket_error(QAbstractSocket::SocketError err)
{
  // TODO:
  std::string error = "Error on socket\n";
  player->ncl_window->error_occurred(error);
}

void lua_socket::socket_disconnected()
{
  assert(player->current_frame () == 0);
#ifndef NDEBUG
  int old_top = lua_gettop(player->L);
#endif
  player->lua_sockets.erase(connection_identifier);
  try
  {
    for(std::vector<event_handler>::const_iterator first = player->event_handlers.begin()
          , last = player->event_handlers.end(); first != last; ++first)
    {
      if(first->class_.empty() || first->class_ == "tcp")
      {
        lua_newtable(player->L);
        luabind::object event(luabind::from_stack(player->L, -1));
        lua_pop(player->L, 1); // pop newtable

        event["class"] = "tcp";
        event["type"] = "disconnect";
        event["connection"] = connection_identifier;

        luabind::setmetatable(event, event);

        assert(luabind::type(first->handler) == LUA_TFUNCTION);
        luabind::object r = luabind::call_function<luabind::object>(first->handler, event);
        if(luabind::type(r) == LUA_TBOOLEAN && luabind::object_cast<bool>(r))
          break;
      }
    }
  }
  catch(luabind::error const& e)
  {
    std::string error = "Error running handler when socket disconnected:\n";
    if(lua_gettop(e.state()) != 0)
      error += lua_tostring(e.state(), -1);
    player->ncl_window->error_occurred(error);
  }
  catch(std::exception const& e)
  {
    std::string error = "Error running handler when socket disconnected:\n";
    error += e.what();
    player->ncl_window->error_occurred(error);
  }
  assert(lua_gettop(player->L) == old_top);
}

bool lua_player::start_set_property(std::string const& name, std::string const& value)
{
  properties_to_set.insert(std::make_pair(name, value));
  return true;
}

bool lua_player::commit_set_property(std::string const& name)
{
  std::map<std::string, std::string>::iterator iterator = properties_to_set.find(name);
  assert(iterator != properties_to_set.end());
  Q_EMIT commit_set_property_signal(iterator->first, iterator->second);
  properties_to_set.erase(iterator);
  return true;
}

void lua_player::commit_set_property_queue(std::string name, std::string value)
{
  commit_set_property_event e = {name, value};
  queued_events.push_back(e);
  Q_EMIT signal_try_unqueue_event ();
}

void lua_player::commit_set_property_impl(std::string name, std::string value)
{
  std::cout << "lua_player::commit_set_property_impl" << std::endl;
  assert(current_frame () == 0);
#ifndef NDEBUG
  int old_top = lua_gettop(L);
#endif
  if(luabind::type(luabind::globals(L)[name]) == LUA_TFUNCTION)
  {
    try
    {
      luabind::call_function<void>(luabind::globals(L)[name], value);
    }
    catch(luabind::error const& e)
    {
      std::string error = "Error running function for setting property:\n";
      if(lua_gettop(e.state()) != 0)
        error += lua_tostring(e.state(), -1);
      ncl_window->error_occurred(error);
    }
    catch(std::exception const& e)
    {
      std::string error = "Error running function for setting property:\n";
      error += e.what();
      ncl_window->error_occurred(error);
    }
  }
  else
    luabind::globals(L)[name] = value;

  try
  {
    for(std::vector<event_handler>::const_iterator first = event_handlers.begin()
          , last = event_handlers.end(); first != last; ++first)
    {
      if(first->class_.empty() || first->class_ == "ncl")
      {
        lua_newtable(L);
        luabind::object event(luabind::from_stack(L, -1));
        lua_pop(L, 1); // pop newtable

        event["class"] = "ncl";
        event["name"] = name;
        event["value"] = value;
        event["action"] = "start";
        event["type"] = "attribution";

        luabind::setmetatable(event, event);

        assert(luabind::type(first->handler) == LUA_TFUNCTION);
        luabind::object r = luabind::call_function<luabind::object>(first->handler, event);
        if(luabind::type(r) == LUA_TBOOLEAN && luabind::object_cast<bool>(r))
          break;
      }
    }
  }
  catch(luabind::error const& e)
  {
    std::string error = "Error running handler for area";
    if(lua_gettop(e.state()) != 0)
      error += lua_tostring(e.state(), -1);
    ncl_window->error_occurred(error);
  }
  catch(std::exception const& e)
  {
    std::string error = "Error running handler for area";
    error += e.what();
    ncl_window->error_occurred(error);
  }
  assert(lua_gettop(L) == old_top);
}

void lua_player::start_area(std::string const& name)
{
  Q_EMIT start_area_signal(name);
}

void lua_player::start_area_queue(std::string name)
{
  start_area_event e = {name};
  queued_events.push_back(e);
  Q_EMIT signal_try_unqueue_event ();
}

void lua_player::start_area_impl(std::string name)
{
  std::cout << "lua_player::start_area_impl" << std::endl;
  assert(current_frame () == 0);
#ifndef NDEBUG
  int old_top = lua_gettop(L);
#endif
  try
  {
    for(std::vector<event_handler>::const_iterator first = event_handlers.begin()
          , last = event_handlers.end(); first != last; ++first)
    {
      if(first->class_.empty() || first->class_ == "ncl")
      {
        lua_newtable(L);
        luabind::object event(luabind::from_stack(L, -1));
        lua_pop(L, 1); // pop newtable

        event["class"] = "ncl";
        event["label"] = name;
        event["type"] = "presentation";

        luabind::setmetatable(event, event);

        assert(luabind::type(first->handler) == LUA_TFUNCTION);
        luabind::object r = luabind::call_function<luabind::object>(first->handler, event);
        if(luabind::type(r) == LUA_TBOOLEAN && luabind::object_cast<bool>(r))
          break;
      }
    }
  }
  catch(luabind::error const& e)
  {
    std::string error = "Error running handler for area";
    if(lua_gettop(e.state()) != 0)
      error += lua_tostring(e.state(), -1);
    ncl_window->error_occurred(error);
  }
  catch(std::exception const& e)
  {
    std::string error = "Error running handler for area";
    error += e.what();
    ncl_window->error_occurred(error);
  }
  assert(lua_gettop(L) == old_top);
}

} } } }
