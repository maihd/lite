#if !defined(LITE_SYSTEM_SDL2) && defined(_WIN32)

#include "lite_api.h"
#include "lite_file.h"
#include "lite_rencache.h"

int luaopen_system(lua_State* L)
{
    //luaL_newlib(L, lib);
    return 1;
}

#endif

//! EOF
