#pragma once

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "lite_string.h"


#define API_TYPE_FONT "Font"


void lite_api_load_libs(lua_State* L);


__forceinline void lua_pushstringview(lua_State* L, LiteStringView string)
{
    lua_pushlstring(L, string.buffer, string.length);
}


__forceinline LiteStringView lua_checkstringview(lua_State* L, int numArg)
{
	size_t length;
	const char* buffer = luaL_checklstring(L, numArg, &length);
	return lite_string_view(buffer, length);
}

//! EOF


