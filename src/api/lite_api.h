#ifndef API_H
#define API_H

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "lite_string.h"

#define API_TYPE_FONT "Font"

void api_load_libs(lua_State* L);

static __forceinline void lua_pushstringview(lua_State* L, LiteStringView string)
{
    lua_pushlstring(L, string.buffer, string.length);
}

#endif
