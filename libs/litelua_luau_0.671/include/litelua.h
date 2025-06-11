/// LiteLua v0.0.2-dev
/// Copyright: MaiHD @ 2025

#pragma once

// -----------------------------------
// Include header files
// -----------------------------------

#include <stdint.h>
#include <stdbool.h>

#include "litelua_config.h"

// -----------------------------------
// Typedef from Lua runtime
// @note(maihd): currently there are
//      no difference from other
//      Lua runtime
// -----------------------------------

// typedef struct lua_State    lua_State;
// typedef struct luaL_Reg     luaL_Reg;


// -----------------------------------
// Types
// -----------------------------------


typedef struct LiteLua LiteLua;


/// IO plugins
typedef struct LiteLuaIO
{
    void*       user_data;

    void*       (*load_file)(void* user_data, const char* path, size_t path_len, size_t* out_size);
    void        (*unload_file_data)(void* user_data, void* data, size_t size);
} LiteLuaIO;


/// Memory manager, GC customizations
typedef struct LiteLuaGC
{
    uint32_t    flags;

    void*       user_data;

    void*       (*alloc)(void* user_data, void* old_buffer, size_t old_size, size_t new_size);
    // void        (*free)(void* user_data, void* buffer);
} LiteLuaGC;


/// Context of LiteLua (Core components)
struct LiteLua
{
    lua_State*          L;

    // Callbacks, modules

    LiteLuaIO           io;
    LiteLuaGC           gc;

    // Runtime-based required fields

#if defined(LITELUA_USING_LUAU)
    lua_CompileOptions  compile_options;
#endif
};


/// Error code of LiteLua
typedef enum LiteLuaError
{
    LiteLuaError_None,
    LiteLuaError_NullParams,
    LiteLuaError_InvalidState,
    LiteLuaError_ExecuteFailure,
    LiteLuaError_CompileFailure,
    LiteLuaError_UnableToLoadFile,
} LiteLuaError;


/// Result when do execution
typedef struct LiteLuaResult
{
    LiteLuaError    error;      // Type of error
    const char*     message;    // Message of operations. @note(maihd): message memory is managed by system
    bool            success;    // Is operations success or not
} LiteLuaResult;


// -----------------------------------
// Runtime management
// -----------------------------------

#ifdef __cplusplus
extern "C" {
#endif

/// Create LiteLua context base on funcs, io, gc
LITELUA_API LiteLua         litelua_create(const LiteLuaIO* io, const LiteLuaGC* gc);

/// Destroy LiteLua context, release used memory
LITELUA_API void            litelua_destroy(LiteLua* context);

/// Load a chunk of code in memory (a string) to Lua stack
LITELUA_API LiteLuaResult   litelua_load_string(LiteLua* context, const char* str, size_t len);

/// Load a chunk of code in file to Lua stack
/// @note(maihd):
///     - Luau does not have one
///     - Lua and LuaJIT use stdio to load file
///     -> LiteLua use custom IO callbacks, and use litelua_load_string to push content to Lua stack
LITELUA_API LiteLuaResult   litelua_load_file(LiteLua* context, const char* path, size_t path_len);

/// Execute Lua chunk code without checking error, but still checking compile failure
LITELUA_API LiteLuaResult   litelua_execute(LiteLua* context, const char* str, size_t len);

/// Execute Lua chunk code with checking error, compile failure
LITELUA_API LiteLuaResult   litelua_execute_safe(LiteLua* context, const char* str, size_t len);

/// Execute Lua chunk code from file without checking error, but still checking compile failure
LITELUA_API LiteLuaResult   litelua_execute_file(LiteLua* context, const char* str, size_t len);

/// Execute Lua chunk code from file with checking error, compile failure
LITELUA_API LiteLuaResult   litelua_execute_file_safe(LiteLua* context, const char* str, size_t len);

/// Binding global C function
LITELUA_API LiteLuaResult   litelua_bind_func(LiteLua* context, const char* name, size_t len, lua_CFunction func);

/// Binding global C modules
LITELUA_API LiteLuaResult   litelua_bind_module(LiteLua* context, const char* name, size_t len, luaL_Reg* funcs, size_t count);

/// Execute Lua chunk code without checking error, but still checking compile failure
// LITELUA_API void            litelua_call(LiteLua* context, const char* str, size_t len);

/// Execute Lua chunk code with checking error, compile failure
// LITELUA_API void            litelua_pcall(LiteLua* context, const char* str, size_t len);


// -----------------------------------
// C <-> Lua
// -----------------------------------

#ifdef LITELUA_UNNEEDED

/// Get string from Lua runtime
LITELUA_API const char*     litelua_tostring(LiteLua* context);

/// Get string from Lua runtime
LITELUA_API const char*     litelua_tolstring(LiteLua* context, size_t* out_len);

/// Get number from Lua runtime
LITELUA_API double          litelua_tonumber(LiteLua* context);

/// Get number from Lua runtime
LITELUA_API float           litelua_tofloat(LiteLua* context);

/// Get int32 number from Lua runtime
/// @note(maihd): this is a extension
LITELUA_API int32_t         litelua_toint32(LiteLua* context);

/// Get int64 number from Lua runtime
/// @note(maihd): this is a extension
LITELUA_API int64_t         litelua_toint64(LiteLua* context);

/// Get uint32 number from Lua runtime
/// @note(maihd): this is a extension
LITELUA_API uint32_t        litelua_touint32(LiteLua* context);

/// Get uint64 number from Lua runtime
/// @note(maihd): this is a extension
LITELUA_API uint64_t        litelua_touint64(LiteLua* context);

#endif

#ifdef __cplusplus
}
#endif

//! EOF

