#include "api.h"

#define __static_assert(cond) ((void)(int[(cond) ? 1 : -1]){0})
#define __count_of(array)                                                      \
    (__static_assert(sizeof(array) >= sizeof(array[0])),                       \
     (sizeof(array) / sizeof(array[0])))

int luaopen_system(lua_State* L);
int luaopen_renderer(lua_State* L);

static const luaL_Reg libs[] = {
    {"system",   luaopen_system  },
    {"renderer", luaopen_renderer},
};

static int lua_absindex(lua_State* L, int i)
{
    if (i < 0 && i > LUA_REGISTRYINDEX)
    {
        i += lua_gettop(L) + 1;
    }
    return i;
}

static int luaL_getsubtable(lua_State* L, int i, const char* name)
{
    int abs_i = lua_absindex(L, i);
    luaL_checkstack(L, 3, "not enough stack slots");
    lua_pushstring(L, name);
    lua_gettable(L, abs_i);
    if (lua_istable(L, -1))
    {
        return 1;
    }

    lua_pop(L, 1);
    lua_newtable(L);
    lua_pushstring(L, name);
    lua_pushvalue(L, -2);
    lua_settable(L, abs_i);
    return 0;
}

static void luaL_requiref(lua_State* L, const char* modname,
                          lua_CFunction openf, int glb)
{
    luaL_checkstack(L, 3, "not enough stack slots available");
    if (luaL_getsubtable(L, LUA_REGISTRYINDEX, "_LOADED"))
    {
        lua_getfield(L, -1, modname);
        lua_pop(L, 1);
        lua_pushcfunction(L, openf);
        lua_pushstring(L, modname);
        lua_call(L, 1, 1);
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, modname);
    }

    if (glb)
    {
        lua_pushvalue(L, -1);
        lua_setglobal(L, modname);
    }

    lua_replace(L, -2);
}

void api_load_libs(lua_State* L)
{
    for (int i = 0; i < __count_of(libs); i++)
    {
        luaL_requiref(L, libs[i].name, libs[i].func, 1);
    }
}
