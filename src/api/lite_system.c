#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>

#ifdef _WIN32
#define chdir _chdir
#include <windows.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif

#include "lite_api.h"
#include "lite_file.h"
#include "lite_memory.h"
#include "lite_rencache.h"
#include "lite_window.h"

static int f_poll_event(lua_State* L)
{
    LiteWindowEvent event = lite_window_poll_event();
    switch (event.type)
    {
    case LiteWindowEventType_Quit:
        lua_pushstringview(L, lite_string_lit("quit"));
        return 1;

    case LiteWindowEventType_Resized:
        lua_pushstringview(L, lite_string_lit("resized"));
        lua_pushnumber(L, (lua_Number)event.resized.width);
        lua_pushnumber(L, (lua_Number)event.resized.height);
        return 3;

    case LiteWindowEventType_Exposed:
        lite_rencache_invalidate();
        lua_pushstringview(L, lite_string_lit("exposed"));
        return 1;

    case LiteWindowEventType_DropFile:
        lua_pushstringview(L, lite_string_lit("filedropped"));
        lua_pushstringview(L, event.drop_file.file_path);
        lua_pushnumber(L, (lua_Number)event.drop_file.x);
        lua_pushnumber(L, (lua_Number)event.drop_file.y);
        return 4;

    case LiteWindowEventType_KeyDown:
        lua_pushstringview(L, lite_string_lit("keypressed"));
        lua_pushstringview(L, event.key_down.key_name);
        return 2;

    case LiteWindowEventType_KeyUp:
        lua_pushstringview(L, lite_string_lit("keyreleased"));
        lua_pushstringview(L, event.key_up.key_name);
        return 2;

    case LiteWindowEventType_TextInput:
        lua_pushstringview(L, lite_string_lit("textinput"));
        lua_pushstringview(L, event.text_input.text);
        return 2;

    case LiteWindowEventType_MouseDown:
        lua_pushstringview(L, lite_string_lit("mousepressed"));
        lua_pushstringview(L, event.mouse_down.button_name);

        lua_pushnumber(L, (lua_Number)event.mouse_down.x);
        lua_pushnumber(L, (lua_Number)event.mouse_down.y);
        lua_pushnumber(L, (lua_Number)event.mouse_down.clicks);
        return 5;

    case LiteWindowEventType_MouseUp:
        lua_pushstringview(L, lite_string_lit("mousereleased"));
        lua_pushstringview(L, event.mouse_up.button_name);
        lua_pushnumber(L, (lua_Number)event.mouse_up.x);
        lua_pushnumber(L, (lua_Number)event.mouse_up.y);
        lua_pushnumber(L, (lua_Number)event.mouse_up.clicks);
        return 5;

    case LiteWindowEventType_MouseMove:
        lua_pushstringview(L, lite_string_lit("mousemoved"));
        lua_pushnumber(L, (lua_Number)event.mouse_move.x);
        lua_pushnumber(L, (lua_Number)event.mouse_move.y);
        lua_pushnumber(L, (lua_Number)event.mouse_move.dx);
        lua_pushnumber(L, (lua_Number)event.mouse_move.dy);
        return 5;

    case LiteWindowEventType_MouseWheel:
        lua_pushstringview(L, lite_string_lit("mousewheel"));
        lua_pushnumber(L, (lua_Number)event.mouse_wheel.y);
        return 2;

    default: break;
    }

    return 0;
}

static int f_wait_event(lua_State* L)
{
    double n = luaL_checknumber(L, 1);
    lua_pushboolean(L, lite_window_wait_event((uint64_t)(n * 1000 * 1000)));
    return 1;
}

static int f_is_binary_file(lua_State* L)
{
    size_t      length;
    const char* file_path = luaL_checklstring(L, 1, &length);
    if (file_path == NULL)
    {
        const char errmsg[] =
            "is_binary_file: first parameter must be a string!";
        lua_pushnil(L);
        lua_pushlstring(L, errmsg, sizeof(errmsg) - 1);
        return 2;
    }

    bool result = lite_is_binary_file(lite_string_view(file_path, length, 0));
    lua_pushboolean(L, result);
    return 1;
}

static const char* cursor_opts[] = {"arrow", "ibeam", "sizeh",
                                    "sizev", "hand",  NULL};

static const LiteCursor cursor_enums[] = {LiteCursor_Arrow, LiteCursor_Ibeam,
                                          LiteCursor_SizeH, LiteCursor_SizeV,
                                          LiteCursor_Hand};

static int f_set_cursor(lua_State* L)
{
    int opt = luaL_checkoption(L, 1, "arrow", cursor_opts);
    lite_window_set_cursor(cursor_enums[opt]);
    return 0;
}

static int f_set_window_title(lua_State* L)
{
    const char* title = luaL_checkstring(L, 1);
    lite_window_set_title(title);
    return 0;
}

static int f_set_window_mode(lua_State* L)
{
    static const char* window_opts[] = {"normal", "maximized", "fullscreen", 0};
    static LiteWindowMode window_modes[] = {LiteWindowMode_Normal,
                                            LiteWindowMode_Maximized,
                                            LiteWindowMode_FullScreen, 0};
    int                   n = luaL_checkoption(L, 1, "normal", window_opts);
    lite_window_set_mode(window_modes[n]);
    return 0;
}

static int f_window_has_focus(lua_State* L)
{
    lua_pushboolean(L, lite_window_has_focus());
    return 1;
}

#if 0
static int f_get_window_opacity(lua_State* L)
{
    float opacity;
    if (SDL_GetWindowOpacity(window, &opacity))
    {
        lua_pushstring(L, "window is invalid");
        lua_error(L);
        return 0;
    }

    lua_pushnumber(L, opacity);
    return 1;
}

static int f_set_window_opacity(lua_State* L)
{
    float opacity = (float)luaL_checknumber(L, 1);
    SDL_SetWindowOpacity(window, opacity);
    return 0;
}
#endif

static int f_show_confirm_dialog(lua_State* L)
{
    const char* title = luaL_checkstring(L, 1);
    const char* msg   = luaL_checkstring(L, 2);

    lua_pushboolean(L, lite_window_confirm_dialog(title, msg));
#if _WIN32
    // int id = MessageBoxA(0, msg, title, MB_YESNO | MB_ICONWARNING);
    // lua_pushboolean(L, id == IDYES);
#else

#endif
    return 1;
}

static int f_chdir(lua_State* L)
{
    const char* path = luaL_checkstring(L, 1);
#if _WIN32
    BOOL err = SetCurrentDirectoryA(path);
    if (!err)
    {
        luaL_error(L, "SetCurrentDirectory() failed");
    }
#else
    int err = chdir(path);
    if (err)
    {
        luaL_error(L, "chdir() failed");
    }
#endif
    return 0;
}

static int f_file_time(lua_State* L)
{
    size_t      len;
    const char* path = luaL_optlstring(L, 1, NULL, &len);
    if (path == NULL)
    {
        const char errmsg[] = "list_dir: first parameter must be a string!";
        lua_pushnil(L);
        lua_pushlstring(L, errmsg, sizeof(errmsg) - 1);
        return 2;
    }

    uint64_t file_time =
        lite_file_write_time(lite_string_view(path, (uint32_t)len, 0u));
    lua_Number lua_file_time = (lua_Number)file_time;
    lua_pushnumber(L, lua_file_time);
    return 1;
}

static int f_list_dir(lua_State* L)
{
    size_t      len;
    const char* path = luaL_optlstring(L, 1, nullptr, &len);
    if (path == nullptr)
    {
        const char errmsg[] = "list_dir: first parameter must be a string!";
        lua_pushnil(L);
        lua_pushlstring(L, errmsg, sizeof(errmsg) - 1);
        return 2;
    }

#if _WIN32
    char path_to_readdir[1024];
    sprintf(path_to_readdir, "%s\\*", path);

    WIN32_FIND_DATAA ffd;
    HANDLE           hFind = FindFirstFileA(path_to_readdir, &ffd);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        DWORD dw = GetLastError();

        char  lpMsgBuf[1024];
        DWORD nMsgBufLen = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), lpMsgBuf,
            sizeof(lpMsgBuf), NULL);

        lua_pushnil(L);
        lua_pushlstring(L, lpMsgBuf, nMsgBufLen);
        return 2;
    }

    lua_newtable(L);
    int i = 1;
    do {
        if (strcmp(ffd.cFileName, ".") == 0)
        {
            continue;
        }
        if (strcmp(ffd.cFileName, "..") == 0)
        {
            continue;
        }
        lua_pushstring(L, ffd.cFileName);
        lua_rawseti(L, -2, i);
        i++;
    } while (FindNextFileA(hFind, &ffd) != 0);

    FindClose(hFind);
#else
    DIR* dir = opendir(path);
    if (!dir)
    {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    lua_newtable(L);
    int            i = 1;
    struct dirent* entry;
    while ((entry = readdir(dir)))
    {
        if (strcmp(entry->d_name, ".") == 0)
        {
            continue;
        }
        if (strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        lua_pushstring(L, entry->d_name);
        lua_rawseti(L, -2, i);
        i++;
    }

    closedir(dir);
#endif

    return 1;
}

/* EXPERIMENTAL
static int f_list_dir_iter_closure(lua_State* L)
{

}

static int f_list_dir_iter(lua_State* L)
{
    lua_pushcclosure(L, f_list_dir_iter_closure, 0);
    lua_pushnil(L);
    lua_pushvalue(L, 1);

    return 3;
}
*/

#ifdef _WIN32
#include <windows.h>
#define realpath(x, y) _fullpath(y, x, MAX_PATH)
#endif

#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m)&S_IFMT) == S_IFREG)
#endif

#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m)&S_IFMT) == S_IFDIR)
#endif

static int f_absolute_path(lua_State* L)
{
    const char* path = luaL_checkstring(L, 1);
    char*       res  = realpath(path, NULL);
    if (!res)
    {
        return 0;
    }
    lua_pushstring(L, res);
    free(res);
    return 1;
}

static int f_get_file_info(lua_State* L)
{
    const char* path = luaL_checkstring(L, 1);

    struct stat s;
    int         err = stat(path, &s);
    if (err < 0)
    {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    lua_newtable(L);
    lua_pushnumber(L, (lua_Number)s.st_mtime);
    lua_setfield(L, -2, "modified");

    lua_pushnumber(L, s.st_size);
    lua_setfield(L, -2, "size");

    if (S_ISREG(s.st_mode))
    {
        lua_pushstring(L, "file");
    }
    else if (S_ISDIR(s.st_mode))
    {
        lua_pushstring(L, "dir");
    }
    else
    {
        lua_pushnil(L);
    }
    lua_setfield(L, -2, "type");

    return 1;
}

static int f_get_clipboard(lua_State* L)
{
    LiteStringView text = lite_clipboard_get();
    if (!text.buffer)
    {
        return 0;
    }

    lua_pushstringview(L, text);
    return 1;
}

static int f_set_clipboard(lua_State* L)
{
    size_t      length;
    const char* text = luaL_checklstring(L, 1, &length);
    lite_clipboard_set(lite_string_view(text, (size_t)length, 0));
    return 0;
}

static int f_get_time(lua_State* L)
{
    double n = (double)lite_cpu_ticks() / (double)lite_cpu_frequency();
    lua_pushnumber(L, n);
    return 1;
}

static int f_sleep(lua_State* L)
{
    double n = luaL_checknumber(L, 1);
    lite_usleep((uint32_t)(n * 1000 * 1000));
    return 0;
}

static int f_exec(lua_State* L)
{
    size_t      len;
    const char* cmd = luaL_checklstring(L, 1, &len);
    char*       buf = malloc(len + 32);
    if (!buf)
    {
        luaL_error(L, "buffer allocation failed");
        return 0;
    }
#if _WIN32
    sprintf(buf, "cmd /c \"%s\"", cmd);
    // WinExec(buf, SW_HIDE);
    system(buf);
#else
    sprintf(buf, "%s &", cmd);
    int res = system(buf);
    (void)res;
#endif
    free(buf);
    return 0;
}

static int f_fuzzy_match(lua_State* L)
{
    const char* str   = luaL_checkstring(L, 1);
    const char* ptn   = luaL_checkstring(L, 2);
    int         score = 0;
    int         run   = 0;

    while (*str && *ptn)
    {
        while (*str == ' ')
        {
            str++;
        }
        while (*ptn == ' ')
        {
            ptn++;
        }
        if (tolower(*str) == tolower(*ptn))
        {
            score += run * 10 - (*str != *ptn);
            run++;
            ptn++;
        }
        else
        {
            score -= 10;
            run = 0;
        }
        str++;
    }
    if (*ptn)
    {
        return 0;
    }

    lua_pushnumber(L, (lua_Number)score - (lua_Number)strlen(str));
    return 1;
}

static int f_begin_frame(lua_State* L)
{
    lite_frame_arena_begin();
    return 0;
}

static int f_end_frame(lua_State* L)
{
    lite_frame_arena_end();
    return 0;
}

static int f_mkdir_recursive(lua_State* L)
{
    size_t      length;
    const char* path = luaL_checklstring(L, 1, &length);

    const bool result = lite_create_directory_recursive(lite_string_view(path, length, 0));
    lua_pushboolean(L, result);
    return 1;
}

static int f_parent_directory(lua_State* L)
{
    size_t      length;
    const char* path = luaL_checklstring(L, 1, &length);

    const LiteStringView result = lite_parent_directory(lite_string_view(path, length, 0));
    lua_pushstringview(L, result);
    return 1;
}

static const luaL_Reg lib[] = {
    {"poll_event",          f_poll_event         },
    {"wait_event",          f_wait_event         },
    {"set_cursor",          f_set_cursor         },
    {"set_window_title",    f_set_window_title   },
    {"set_window_mode",     f_set_window_mode    },
 //{"get_window_opacity",  f_get_window_opacity },
  //{"set_window_opacity",  f_set_window_opacity },
    {"window_has_focus",    f_window_has_focus   },
    {"show_confirm_dialog", f_show_confirm_dialog},
    {"chdir",               f_chdir              },
    {"file_time",           f_file_time          },
    {"is_binary_file",      f_is_binary_file     },
    {"list_dir",            f_list_dir           },
    {"absolute_path",       f_absolute_path      },
    {"get_file_info",       f_get_file_info      },
    {"get_clipboard",       f_get_clipboard      },
    {"set_clipboard",       f_set_clipboard      },
    {"get_time",            f_get_time           },
    {"sleep",               f_sleep              },
    {"exec",                f_exec               },
    {"fuzzy_match",         f_fuzzy_match        },
    {"begin_frame",         f_begin_frame        },
    {"end_frame",           f_end_frame          },
    
    {"mkdir_recursive",     f_mkdir_recursive    },
    {"parent_directory",    f_parent_directory   },

    {NULL,                  NULL                 }
};

int luaopen_system(lua_State* L)
{
    luaL_newlib(L, lib);
    return 1;
}

//! EOF
