#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "api/lite_api.h"
#include "lite_startup.h"
#include "lite_window.h"
#include "lite_rencache.h"

static double lite_get_scale(void)
{
#if _WIN32
    return (double)lite_window_dpi() / 96.0;
#else
    return 1.0;
#endif
}

static void lite_get_exe_filename(char* buf, int sz)
{
#if _WIN32
    int len  = GetModuleFileNameA(NULL, buf, sz - 1);
    buf[len] = '\0';
#elif __linux__
    char path[512];
    sprintf(path, "/proc/%d/exe", getpid());
    int len  = readlink(path, buf, sz - 1);
    buf[len] = '\0';
#elif __APPLE__
    unsigned size = sz;
    _NSGetExecutablePath(buf, &size);
#else
    strcpy(buf, "./lite");
#endif
}

static lua_State* lite_create_lua(uint32_t argc, const char** argv)
{
	lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lite_api_load_libs(L);

    lua_newtable(L);
    for (int i = 0; i < argc; i++)
    {
        lua_pushstring(L, argv[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setglobal(L, "ARGS");

    lua_pushstring(L, "1.11");
    lua_setglobal(L, "VERSION");

    lua_pushstring(L, "Windows");
    lua_setglobal(L, "PLATFORM");

    lua_pushnumber(L, lite_get_scale());
    lua_setglobal(L, "SCALE");

    char exename[128];
    lite_get_exe_filename(exename, sizeof(exename));

    lua_pushstring(L, exename);
    lua_setglobal(L, "EXEFILE");

	return L;
}

void lite_startup(const LiteStartupParams params)
{
    uint32_t     argc = params.argc;
    const char** argv = params.argv;

	lua_State* L = lite_create_lua(argc, argv);

    int errcode = luaL_dostring(
        L,
        "local core, err\n"
        "xpcall(function()\n"
        "  SCALE = tonumber(os.getenv(\"LITE_SCALE\")) or SCALE\n"
        "  PATHSEP = package.config:sub(1, 1)\n"
        //"  PATHSEP = \"/\"\n"
        "  EXEDIR = EXEFILE:match(\"^(.+)[/\\\\].*$\")\n"
        "  package.path = EXEDIR .. '/data/?.lua;' .. package.path\n"
        "  package.path = EXEDIR .. '/data/?/init.lua;' .. package.path\n"
        "  core = require('core')\n"
        "  core.init()\n"
        "  core.run()\n"
        "end, function(...)\n"
        "  err = ...\n"
        "  if core and core.on_error then\n"
        "    pcall(core.on_error, err)\n"
        "  end\n"
        //"  os.exit(1)\n"
        "end)\n"
        "print(\"Execute end checking error...\")\n"
        "if err then\n"
        "  print('Error: ' .. tostring(err))\n"
        "  print(debug.traceback(nil, 2))\n"
        "  error(err)\n"
        "end\n");
    if (errcode != 0)
    {
        const char* title  = params.title;
        const char* errmsg = lua_tostring(L, -1);
        if (errmsg == NULL)
        {
            errmsg = "Unknown error!";
        }

        char dialog_message[4096];
        sprintf(dialog_message, "Cannot launch application. Error:\n%s\n\nLaunch application with safe mode?", errmsg);
        if (lite_window_confirm_dialog(title, dialog_message))
        {
			lua_close(L);
			
			L = lite_create_lua(argc, argv);

            errcode = luaL_dostring(
                L,
                "local core, err\n"
                "xpcall(function()\n"
                "  SCALE = tonumber(os.getenv(\"LITE_SCALE\")) or SCALE\n"
                "  PATHSEP = package.config:sub(1, 1)\n"
                //"  PATHSEP = \"/\"\n"
                "  EXEDIR = EXEFILE:match(\"^(.+)[/\\\\].*$\")\n"
                "  package.path = EXEDIR .. '/fallback/?.lua;' .. package.path\n"
                "  package.path = EXEDIR .. '/fallback/?/init.lua;' .. package.path\n"
                "  core = require('core')\n"
                "  core.init()\n"
                "  core.run()\n"
                "end, function(...)\n"
                "  err = ...\n"
                "  if core and core.on_error then\n"
                "    pcall(core.on_error, err)\n"
                "  end\n"
                //"  os.exit(1)\n"
                "end)\n"
                "print(\"Execute end checking error...\")\n"
                "if err then\n"
                "  print('Error: ' .. tostring(err))\n"
                "  print(debug.traceback(nil, 2))\n"
                "  error(err)\n"
                "end\n");
            if (errcode != 0)
            {
                title  = params.title;
                errmsg = lua_tostring(L, -1);
                if (errmsg == NULL)
                {
                    errmsg = "Unknown error!";
                }

                sprintf(dialog_message, "Cannot launch application in safe mode. Error:\n%s\n\n"
                                        "Did you edited the scripts in fallback folder?", errmsg);
                lite_window_message_box(title, dialog_message);
            }
        }
    }

    lua_close(L);
}

//! EOF
