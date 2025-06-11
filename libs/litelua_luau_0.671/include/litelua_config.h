/// LiteLua v0.0.2-dev
/// Copyright: MaiHD @ 2025

#pragma once

// Un-comment to make LiteLua use LuaJIT
// #define LITELUA_USING_LUA 1

// Un-comment to make LiteLua use LuaJIT
// #define LITELUA_USING_LUAJIT 1

// Un-comment to make LiteLua use Luau
#define LITELUA_USING_LUAU 1


// Include Lua or LuaJIT headers
#if defined(LITELUA_USING_LUAJIT) || defined(LITELUA_USING_LUA)

    #ifndef LITELUA_LUA_CORE
    #define LITELUA_LUA_CORE <lua/lua.h>        // Change the path to Lua/LuaJIT lua.h
    #endif

    #ifndef LITELUA_LUA_LIBS
    #define LITELUA_LUA_LIBS <lua/lualib.h>     // Change the path to Lua/LuaJIT lualib.h
    #endif

    #ifndef LITELUA_LUA_XLIBS
    #define LITELUA_LUA_XLIBS <lua/lauxlib.h>   // Change the path to Lua/LuaJIT lauxlib.h
    #endif

    #include LITELUA_LUA_CORE
    #include LITELUA_LUA_LIBS
    #include LITELUA_LUA_XLIBS

#elif defined(LITELUA_USING_LUAU)

    #ifndef LITELUA_LUAU_CORE
    #define LITELUA_LUAU_CORE <lua.h>      // Change the path to Luau's lua.h
    #endif

    #ifndef LITELUA_LUAU_LIBS
    #define LITELUA_LUAU_LIBS <lualib.h>   // Change the path to Luau's lualib.h
    #endif

    #ifndef LITELUA_LUAU_CODE
    #define LITELUA_LUAU_CODE <luacode.h>  // Change the path to Luau's luacode.h
    #endif

    #include LITELUA_LUAU_CORE
    #include LITELUA_LUAU_LIBS
    #include LITELUA_LUAU_CODE
#endif

// Config attributes
#ifndef LITELUA_API
#define LITELUA_API
#endif

//! EOF


