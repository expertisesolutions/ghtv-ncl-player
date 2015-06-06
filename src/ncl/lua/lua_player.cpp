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
#include <ghtv/qt/ncl/player/url_file.hpp>
#include <ghtv/qt/ncl/player/create_url.hpp>

#include <boost/algorithm/string.hpp>

namespace ghtv { namespace qt { namespace ncl { namespace lua {

namespace {

void* require_loaded_sentinel = 0;

}

void activation_frame::signal_execution_finished ()
{
  Q_EMIT execution_finished();
}

void lua_player::activate_frame(std::string const& file_base, std::string const& filename)
{
#ifndef NDEBUG
  int old_top = lua_gettop(L);
#endif
  lua_State* thread = lua_newthread(L);
  lua_pushlightuserdata(L, thread);
  lua_insert(L, -2);
  lua_settable(L, LUA_REGISTRYINDEX);
  assert(old_top == lua_gettop(L));
  activated_frames.push_back(new activation_frame(thread, file_base, filename));
}

lua_State* lua_player::current_frame()
{
  if(activated_frames.empty())
    return 0;
  else
    return activated_frames.back()->thread;
}

activation_frame* lua_player::current_activation_frame ()
{
  if(activated_frames.empty())
    return 0;
  else
    return activated_frames.back();
}

void lua_player::pop_frame()
{
  // should remove thread from LUA_REGISTRYINDEX so it can be gc'ed
  delete activated_frames.back();
  activated_frames.pop_back();
}

void lua_player::global_scope_finished ()
{
  pop_frame ();
  send_start_event ();
  Q_EMIT signal_try_unqueue_event ();
}

void lua_player::run_require (std::string file)
{
  Q_EMIT run_require_signal (file);
}

void lua_player::run_global_scope()
{
  QObject::disconnect(this, SIGNAL (started ()));

  luaL_openlibs(L);

  luabind::open(L);

  luabind::module(L, "ghtv")
  [
   luabind::class_<lua_player>("lua_player")
  ];

  init_sandbox();
  init_canvas();
  init_event();

  start_time = boost::posix_time::microsec_clock::universal_time();

  QObject::connect(this, SIGNAL(resume_current_frame_signal (int)), this
                   , SLOT(resume_current_frame (int)), Qt::QueuedConnection);
  QObject::connect(this, SIGNAL(run_require_signal (std::string)), this
                   , SLOT(run_require_slot (std::string)), Qt::QueuedConnection);

  try
  {
    main_file_url = player::create_url(path, root_path);
    lua_path = main_file_url.path ().toStdString ();

    {
      std::string::reverse_iterator
        iterator = std::find(lua_path.rbegin(), lua_path.rend(), '/')
        , iterator_backslash = std::find(lua_path.rbegin(), lua_path.rend(), '\\');
      iterator = (std::min)(iterator, iterator_backslash);
      if(iterator != lua_path.rend())
        lua_path.erase(boost::prior(iterator.base()), lua_path.end());
      else
      {
        std::string error_msg = "Couldn't create a absolute path from the path for the NCL file: ";
        error_msg += lua_path;
        ncl_window->error_occurred(error_msg);
        return;
      }
    }

    activate_frame(path, main_file_url.toString ().toStdString ());

    QObject::connect(current_activation_frame (), SIGNAL(execution_finished ())
                     , this, SLOT(global_scope_finished ()));
    QObject::connect(this, SIGNAL(signal_all_execution_finished ())
                     , this, SLOT(try_unqueue_events ()), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(signal_try_unqueue_event ())
                     , this, SLOT(try_unqueue_event ()), Qt::QueuedConnection);

    assert(pending_download_file == 0);
    pending_download_file = new player::url_file(main_file_url, this);
    if(!pending_download_file->local())
    {
      QObject::connect(this, SIGNAL(download_lua_signal ()), this
                       , SLOT(download_global ()), Qt::QueuedConnection);
      QObject::connect(pending_download_file, SIGNAL(download_finished_signal()), this
                       , SLOT(download_lua()));
      QObject::connect(pending_download_file, SIGNAL(error_signal(std::string)), this
                       , SLOT(download_error(std::string)));
    }
    else
    {
      QObject::connect(this, SIGNAL(download_lua_signal ()), this
                       , SLOT(download_global ()), Qt::QueuedConnection);
      Q_EMIT download_lua_signal();
    }
  }
  catch(std::exception const& e)
  {
    std::string error = "Error loading file ";
    error += root_path;
    error += '/';
    error += path;
    error += " with error: ";
    error += e.what();
    ncl_window->error_occurred(error);
  }
}

void lua_player::expired_finished ()
{
  pop_frame ();
  assert(activated_frames.empty ());
  resume_current_frame (); // allow signaling of completely finished
}

void lua_player::require_finished ()
{
  assert(activated_frames.size() > 1);
  activation_frame* frame_before_this = *boost::next(activated_frames.rbegin(), 1);
  if(lua_gettop(current_frame ()))
  {
    std::abort ();
  }
  else
  {
    lua_pushlightuserdata(current_frame (), &require_loaded_sentinel);
    luabind::object sentinel_object(luabind::from_stack(current_frame (), -1));
    lua_pop(current_frame (), 1);
    if(luabind::registry(current_frame ())["_LOADED"][current_activation_frame ()->file_base] == sentinel_object)
    {
      luabind::registry(current_frame ())["_LOADED"][current_activation_frame ()->file_base] = true;
      luabind::globals(current_frame ())["package"]["loaded"][current_activation_frame ()->file_base] = true;
      lua_pushboolean(frame_before_this->thread, 1);
    }
    else
    {
      luabind::globals(current_frame ())["package"]["loaded"][current_activation_frame ()->file_base]
        = luabind::registry(current_frame ())["_LOADED"][current_activation_frame ()->file_base];
      luabind::object module_table
        = luabind::registry(current_frame ())["_LOADED"][current_activation_frame ()->file_base];
      assert(luabind::type(module_table) == LUA_TTABLE);
      module_table.push(current_frame ());
      lua_settop(frame_before_this->thread, 0);
      lua_xmove(current_frame (), frame_before_this->thread, 1);
      assert(lua_type(frame_before_this->thread, -1) == LUA_TTABLE);
    }
  }
  pop_frame ();
  Q_EMIT resume_current_frame_signal (1);
}

void lua_player::run_require_slot(std::string file)
{
  try
  {
    QUrl url = player::create_url(file + ".lua", lua_path);

    activate_frame(file, url.toString ().toStdString ());

    QObject::connect(current_activation_frame (), SIGNAL(execution_finished ())
                     , this, SLOT(require_finished ()));

    assert(pending_download_file == 0);
    pending_download_file = new player::url_file(url, this);

    if(!pending_download_file->local())
    {
      QObject::connect(this, SIGNAL(download_lua_signal ()), this
                       , SLOT(download_require ()), Qt::QueuedConnection);
      QObject::connect(pending_download_file, SIGNAL(download_finished_signal()), this
                       , SLOT(download_lua()));
      QObject::connect(pending_download_file, SIGNAL(error_signal(std::string)), this
                       , SLOT(download_error(std::string)));
    }
    else
    {
      QObject::connect(this, SIGNAL(download_lua_signal ()), this
                       , SLOT(download_require ()), Qt::QueuedConnection);
      Q_EMIT download_lua_signal();
    }
  }
  catch(std::exception const& e)
  {
    std::string error = "Error loading file ";
    error += lua_path;
    error += '/';
    error += path;
    error += " with error: ";
    error += e.what();
    ncl_window->error_occurred(error);
  }
}

void lua_player::run_dofile_slot(std::string file)
{
}

void lua_player::download_global()
{
  QObject::disconnect(this, SLOT(download_global ()));

  assert(pending_download_file != 0);
  QByteArray content = pending_download_file->io_device()->readAll();
  pending_download_file = 0;
  if(content.size() > 0)
  {
    int r = luaL_loadstring(current_frame(), content.constData());
    if(r != 0)
    {
      std::string error = "Error loading file ";
      // error += root_path;
      // error += '/';
      // error += path;
      error += " with error: ";
      if(lua_gettop(current_frame ()) >= 1)
      {
        luabind::object error_msg(luabind::from_stack(current_frame (), -1));
        if(luabind::type(error_msg) == LUA_TSTRING)
          error += luabind::object_cast<std::string>(error_msg);
      }
      ncl_window->error_occurred(error);
    }
    else
    {
      run_current_frame();
    }
  }  
  else
  {
    std::cout << "file was empty" << std::endl;
  }
}

void lua_player::download_require()
{
  QObject::disconnect(this, SLOT(download_require ()));

  assert(pending_download_file != 0);
  QByteArray content = pending_download_file->io_device()->readAll();
  pending_download_file = 0;
  if(content.size() > 0)
  {
    int r = luaL_loadstring(current_frame(), content.constData());
    if(r != 0)
    {
      std::string error = "Error loading file ";
      // error += root_path;
      // error += '/';
      // error += path;
      error += " with error: ";
      if(lua_gettop(current_frame ()) >= 1)
      {
        luabind::object error_msg(luabind::from_stack(current_frame (), -1));
        if(luabind::type(error_msg) == LUA_TSTRING)
          error += luabind::object_cast<std::string>(error_msg);
      }
      ncl_window->error_occurred(error);
    }
    else
    {
      lua_pushlightuserdata(current_frame (), &require_loaded_sentinel);
      luabind::object sentinel_object(luabind::from_stack(current_frame (), -1));
      lua_pop(current_frame (), 1);
      luabind::registry (current_frame ())["_LOADED"]
        [current_activation_frame()->file_base] = sentinel_object;
      lua_pushstring(current_frame (), current_activation_frame ()->file_base.c_str());

      run_current_frame(1);
    }
  }  
  else
  {
    std::cout << "file was empty" << std::endl;
  }
}

void lua_player::download_image()
{
}

void lua_player::download_error(std::string error)
{
  QObject::disconnect(this, SIGNAL(download_lua_signal ()));

  std::string error_msg = "Error while download file. ";
  error_msg += error;
  ncl_window->error_occurred(error_msg);
}

void lua_player::run_current_frame(int args)
{
  assert(current_frame() != 0);
  int r = lua_resume(current_frame(), args);
  if(r == LUA_YIELD)
  {
    std::cout << "Has yielded with " << lua_gettop(L) << " arguments" << std::endl;
  }
  else if(r != 0)
  {
    std::string error = "Lua error: ";
    if(lua_gettop(current_frame ()) >= 1)
    {
      luabind::object error_msg(luabind::from_stack(current_frame (), -1));
      if(luabind::type(error_msg) == LUA_TSTRING)
        error += luabind::object_cast<std::string>(error_msg);
    }
    ncl_window->error_occurred(error);
  }
  else
  {
    std::cout << "execution is finished" << std::endl;
    current_activation_frame ()->signal_execution_finished ();
  }
  std::cout << "lua_player::run_current_frame is finished" << std::endl;
}

void lua_player::resume_current_frame(int args)
{
  std::cout << "resuming with " << args << " arguments" << std::endl;
  if(current_frame() != 0)
  {
    int r = lua_resume(current_frame(), args);
    if(r == LUA_YIELD)
    {
    }
    else if(r != 0)
    {
      std::string error = "Lua error: ";
      if(lua_gettop(current_frame ()) >= 1)
      {
        luabind::object error_msg(luabind::from_stack(current_frame (), -1));
        if(luabind::type(error_msg) == LUA_TSTRING)
          error += luabind::object_cast<std::string>(error_msg);
      }
      ncl_window->error_occurred(error);
    }
    else
    {
      std::cout << "execution is finished" << std::endl;
      current_activation_frame ()->signal_execution_finished ();
    }
  }
  else
  {
    std::cout << "signal_all_execution_finished" << std::endl;
    Q_EMIT signal_all_execution_finished ();
  }
  std::cout << "resume_current_frame is finished" << std::endl;
}

void lua_player::try_unqueue_event ()
{
  std::cout << "lua_player::try_unqueue_event" << std::endl;
  if(!queued_events.empty () && current_frame () == 0)
  {
    run_queued_events ();
  }
}

void lua_player::run_queued_events ()
{
  std::cout << "lua_player::run_queued_events" << std::endl;
  assert(current_frame () == 0);
  assert(!queued_events.empty ());
  event_variant v = queued_events.front ();
  queued_events.pop_front ();
  if(keyboard_event* e = boost::get<keyboard_event>(&v))
  {
    key_process_impl(e->key, e->press);
  }
  else if(start_area_event* e = boost::get<start_area_event>(&v))
  {
    start_area_impl(e->name);
  }
  else if(commit_set_property_event* e = boost::get<commit_set_property_event>(&v))
  {
    commit_set_property_impl(e->name, e->value);
  }
  Q_EMIT signal_try_unqueue_event ();
}

} } } }
