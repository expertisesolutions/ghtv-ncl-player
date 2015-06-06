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

#include <ghtv/qt/ncl/lua/lua_player.hpp>
#include <ghtv/qt/ncl/player/url_file.hpp>
#include <ghtv/qt/ncl/player/create_url.hpp>

namespace ghtv { namespace qt { namespace ncl { namespace lua {

namespace {

int require(lua_State* L)
{
  std::cout << "require" << std::endl;

  if(lua_gettop(L) > 0)
  {
    if(lua_type(L, 1) == LUA_TSTRING)
    {
      if(luabind::type(luabind::registry (L)["_LOADED"]) == LUA_TTABLE)
      {
        luabind::object file_object(luabind::from_stack(L, 1));
        std::string file = luabind::object_cast<std::string>(file_object);
        luabind::object table = luabind::registry (L) ["_LOADED"][file];
        if(!luabind::type(table) != LUA_TNIL)
        {
          lua_player* player = luabind::object_cast<lua_player*>(luabind::registry(L)["lua_player"]);
          assert(player != 0);
          player->run_require(file);
        }
        else
        {
          table.push (L);
          return 1;
        }
      }
      else
      {
        std::cout << "_LOADED is not a table, signal error" << std::endl;
      }
    }    
  }
  return lua_yield(L, 0);
}

int dofile(lua_State* L)
{
  std::cout << "dofile" << std::endl;
  lua_pushnil(L);
  return 1;
}

int seeall(lua_State* L)
{
  const int globals =
#if LUA_VERSION_NUM == 502
    LUA_RIDX_GLOBALS;
#else
    LUA_GLOBALSINDEX;
#endif
  luaL_checktype(L, 1, LUA_TTABLE);
  if (!lua_getmetatable(L, 1)) {
    lua_createtable(L, 0, 1); /* create new metatable */
    lua_pushvalue(L, -1);
    lua_setmetatable(L, 1);
  }
  lua_pushvalue(L, globals);
  lua_setfield(L, -2, "__index");  /* mt.__index = _G */
  return 0;
}

int module(lua_State* L)
{
  if(lua_gettop(L) >= 1)
  {
    const char* modname = luaL_checkstring(L, 1);
    std::cout << "module function for: " << modname << std::endl;

    std::size_t load_last = lua_gettop(L);
    if(luabind::type(luabind::registry(L)["_LOADED"]) == LUA_TTABLE)
    {
      luabind::object table = luabind::registry(L)["_LOADED"][modname];
      if(luabind::type(table) != LUA_TTABLE)
      {
        luabind::registry(L)["_LOADED"][modname] = luabind::newtable(L);
        table = luabind::registry(L)["_LOADED"][modname];
      }
      table["_NAME"] = modname;
      table["_M"] = table;

      table.push(L);
      lua_Debug ar;
      // This pushes the activated function to the stack
      if (lua_getstack(L, 1, &ar) == 0 ||
          lua_getinfo(L, "f", &ar) == 0 ||  /* get calling function */
          lua_iscfunction(L, -1))
        return 0;

      lua_pushvalue(L, -2);
      lua_setfenv(L, -2);
      lua_pop(L, 1);

      for(int i = 2; i != load_last + 1; ++i)
      {
        lua_pushvalue(L, i);
        table.push (L);
        lua_call(L, 1, 0);
      }

      table.push (L);
      return 1;
    }
  }
  return 0;
}

}

void lua_player::init_sandbox()
{
  {
    lua_newtable(L);
    luabind::object empty_table(luabind::from_stack(L, -1));
    lua_pop(L, 1);
    luabind::globals(L)["package"] = empty_table;
  }

  {
    lua_newtable(L);
    luabind::object empty_table(luabind::from_stack(L, -1));
    lua_pop(L, 1);
    luabind::registry(L)["_LOADED"] = empty_table;
  }

  {
    lua_newtable(L);
    luabind::object empty_table(luabind::from_stack(L, -1));
    lua_pop(L, 1);
    luabind::globals(L)["package"]["loaded"] = empty_table;
  }

  lua_pushcfunction(L, &lua::require);
  luabind::object require_obj(luabind::from_stack(L, -1));
  lua_pop(L, 1);

  lua_pushcfunction(L, &lua::dofile);
  luabind::object dofile_obj(luabind::from_stack(L, -1));
  lua_pop(L, 1);

  lua_pushcfunction(L, &lua::seeall);
  luabind::object seeall_obj(luabind::from_stack(L, -1));
  lua_pop(L, 1);

  lua_pushcfunction(L, &lua::module);
  luabind::object module_obj(luabind::from_stack(L, -1));
  lua_pop(L, 1);

  luabind::globals(L)["require"] = require_obj;
  luabind::globals(L)["package"]["require"] = require_obj;
  luabind::globals(L)["dofile"] = dofile_obj;
  luabind::globals(L)["package"]["dofile"] = dofile_obj;
  luabind::globals(L)["seeall"] = seeall_obj;
  luabind::globals(L)["package"]["seeall"] = seeall_obj;
  luabind::globals(L)["module"] = module_obj;
  luabind::globals(L)["package"]["module"] = module_obj;
  luabind::globals(L)["coroutine"] = luabind::nil;
}

} } } }
