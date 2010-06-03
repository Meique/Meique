/*
    This file is part of the Meique project
    Copyright (C) 2009-2010 Hugo Parente Lima <hugo.pl@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LUACPPUTIL_H
#define LUACPPUTIL_H

#include <string>
#include <sstream>
#include <list>
#include <cassert>
#include "lua.h"

/// Convert a lua type at index \p index to the C++ type \p T.
template<typename T>
T lua_tocpp(lua_State* L, int index);

template<>
inline std::string lua_tocpp<std::string>(lua_State* L, int index)
{
    const char* res = lua_tostring(L, index);
    return res ? res : std::string();
}

template<>
inline int lua_tocpp<int>(lua_State* L, int index)
{
    return lua_tointeger(L, index);
}

template<typename Map>
static void readLuaTable(lua_State* L, int tableIndex, Map& map)
{
    assert(tableIndex >= 0);
    assert(lua_istable(L, tableIndex));

    lua_pushnil(L);  /* first key */
    while (lua_next(L, tableIndex) != 0) {
        typename Map::key_type key = lua_tocpp<typename Map::key_type>(L, -2);
        typename Map::mapped_type value = lua_tocpp<typename Map::mapped_type>(L, lua_gettop(L));
        map[key] = value;
        lua_pop(L, 1); // removes 'value'; keeps 'key' for next iteration
    }
}

template<typename List>
static void readLuaList(lua_State* L, int tableIndex, List& list)
{
    assert(tableIndex >= 0);
    assert(lua_istable(L, tableIndex));

    lua_pushnil(L);  /* first key */
    while (lua_next(L, tableIndex) != 0) {
        typename List::value_type value = lua_tocpp<typename List::value_type>(L, lua_gettop(L));
        list.push_back(value);
        lua_pop(L, 1); // removes 'value'; keeps 'key' for next iteration
    }
}

template<typename T>
T getField(lua_State* L, const char* key, int tableIndex = -1)
{
    lua_getfield(L, tableIndex, key);
    T retval = lua_tocpp<T>(L, -1);
    lua_pop(L, 1);
    return retval;
}

class LuaError
{
public:
    LuaError(lua_State* L) : m_L(L) {}
    ~LuaError()
    {
        lua_pushstring(m_L, m_stream.str().c_str());
        lua_error(m_L);
    }
    template <typename T>
    std::ostream& operator<<(const T& t) { return m_stream << t; }
private:
    lua_State* m_L;
    std::ostringstream m_stream;
};

#endif
