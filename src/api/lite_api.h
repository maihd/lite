#pragma once

#include "lauxlib.h"
#include "lite_string.h"
#include "lua.h"
#include "lualib.h"

#define API_TYPE_FONT "Font"

void lite_api_load_libs(lua_State* L);

static __forceinline void lua_pushstringview(lua_State*     L,
                                             LiteStringView string)
{
    lua_pushlstring(L, string.buffer, string.length);
}

//! EOF


