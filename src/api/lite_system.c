#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
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
#include "lite_rencache.h"

extern SDL_Window* window;

static const char* button_name(int button)
{
    switch (button)
    {
        case 1: return "left";
        case 2: return "middle";
        case 3: return "right";
        default: return "?";
    }
}

static char* key_name(char* dst, int sym)
{
    strcpy(dst, SDL_GetKeyName(sym));
    char* p = dst;
    while (*p)
    {
        *p = tolower(*p);
        p++;
    }
    return dst;
}

static int f_poll_event(lua_State* L)
{
    char      buf[16];
    int       mx, my, wx, wy;
    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            lua_pushstring(L, "quit");
            return 1;

        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                lua_pushstring(L, "resized");
                lua_pushnumber(L, e.window.data1);
                lua_pushnumber(L, e.window.data2);
                return 3;
            }
            else if (e.window.event == SDL_WINDOWEVENT_EXPOSED)
            {
                lite_rencache_invalidate();
                lua_pushstring(L, "exposed");
                return 1;
            }

            // on some systems, when alt-tabbing to the window SDL will queue up
            // several KEYDOWN events for the `tab` key; we flush all keydown
            // events on focus so these are discarded
            if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
            {
                SDL_FlushEvent(SDL_KEYDOWN);
            }
            break;

        case SDL_DROPFILE:
            SDL_GetGlobalMouseState(&mx, &my);
            SDL_GetWindowPosition(window, &wx, &wy);
            lua_pushstring(L, "filedropped");
            lua_pushstring(L, e.drop.file);
            lua_pushnumber(L, (lua_Number)mx - (lua_Number)wx);
            lua_pushnumber(L, (lua_Number)my - (lua_Number)wy);
            SDL_free(e.drop.file);
            return 4;

        case SDL_KEYDOWN:
            lua_pushstring(L, "keypressed");
            lua_pushstring(L, key_name(buf, e.key.keysym.sym));
            return 2;

        case SDL_KEYUP:
            lua_pushstring(L, "keyreleased");
            lua_pushstring(L, key_name(buf, e.key.keysym.sym));
            return 2;

        case SDL_TEXTINPUT:
            lua_pushstring(L, "textinput");
            lua_pushstring(L, e.text.text);
            return 2;

        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT)
            {
                SDL_CaptureMouse(true);
            }
            lua_pushstring(L, "mousepressed");
            lua_pushstring(L, button_name(e.button.button));
            lua_pushnumber(L, e.button.x);
            lua_pushnumber(L, e.button.y);
            lua_pushnumber(L, e.button.clicks);
            lua_pushnumber(L, (double)e.button.timestamp);
            return 6;

        case SDL_MOUSEBUTTONUP:
            if (e.button.button == 1)
            {
                SDL_CaptureMouse(false);
            }
            lua_pushstring(L, "mousereleased");
            lua_pushstring(L, button_name(e.button.button));
            lua_pushnumber(L, e.button.x);
            lua_pushnumber(L, e.button.y);
            lua_pushnumber(L, (double)e.button.timestamp);
            return 5;

        case SDL_MOUSEMOTION:
            lua_pushstring(L, "mousemoved");
            lua_pushnumber(L, e.motion.x);
            lua_pushnumber(L, e.motion.y);
            lua_pushnumber(L, e.motion.xrel);
            lua_pushnumber(L, e.motion.yrel);
            lua_pushnumber(L, (double)e.motion.timestamp);
            return 6;

        case SDL_MOUSEWHEEL:
            lua_pushstring(L, "mousewheel");
            lua_pushnumber(L, e.wheel.y);
            return 2;

        default: break;
        }
    }

    return 0;
}

static int f_wait_event(lua_State* L)
{
    double n = luaL_checknumber(L, 1);
    lua_pushboolean(L, SDL_WaitEventTimeout(NULL, (int)(n * 1000)));
    return 1;
}

static int f_is_binary_file(lua_State* L)
{
    size_t length;
    const char* file_path = luaL_checklstring(L, 1, &length);
    if (file_path == NULL)
    {
        const char errmsg[] = "is_binary_file: first parameter must be a string!";
        lua_pushnil(L);
        lua_pushlstring(L, errmsg, sizeof(errmsg) - 1);
        return 2;
    }

    bool result = lite_is_binary_file(lite_string_view(file_path, length, 0));
    lua_pushboolean(L, result);
    return 1;
}

static SDL_Cursor* cursor_cache[SDL_SYSTEM_CURSOR_HAND + 1];

static const char* cursor_opts[] = {"arrow", "ibeam", "sizeh",
    "sizev", "hand",  NULL};

static const int cursor_enums[] = {
    SDL_SYSTEM_CURSOR_ARROW, SDL_SYSTEM_CURSOR_IBEAM, SDL_SYSTEM_CURSOR_SIZEWE,
    SDL_SYSTEM_CURSOR_SIZENS, SDL_SYSTEM_CURSOR_HAND};

static int f_set_cursor(lua_State* L)
{
    int         opt    = luaL_checkoption(L, 1, "arrow", cursor_opts);
    int         n      = cursor_enums[opt];
    SDL_Cursor* cursor = cursor_cache[n];
    if (!cursor)
    {
        cursor          = SDL_CreateSystemCursor(n);
        cursor_cache[n] = cursor;
    }
    SDL_SetCursor(cursor);
    return 0;
}

static int f_set_window_title(lua_State* L)
{
    const char* title = luaL_checkstring(L, 1);
    SDL_SetWindowTitle(window, title);
    return 0;
}

static const char* window_opts[] = {"normal", "maximized", "fullscreen", 0};
enum
{
    WIN_NORMAL,
    WIN_MAXIMIZED,
    WIN_FULLSCREEN
};

static int f_set_window_mode(lua_State* L)
{
    int n = luaL_checkoption(L, 1, "normal", window_opts);
    SDL_SetWindowFullscreen(
                            window, n == WIN_FULLSCREEN ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    if (n == WIN_NORMAL)
    {
        SDL_RestoreWindow(window);
    }
    if (n == WIN_MAXIMIZED)
    {
        SDL_MaximizeWindow(window);
    }
    return 0;
}

static int f_window_has_focus(lua_State* L)
{
    unsigned flags = SDL_GetWindowFlags(window);
    lua_pushboolean(L, flags & SDL_WINDOW_INPUT_FOCUS);
    return 1;
}

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

static int f_show_confirm_dialog(lua_State* L)
{
    const char* title = luaL_checkstring(L, 1);
    const char* msg   = luaL_checkstring(L, 2);

#if _WIN32
    int id = MessageBoxA(0, msg, title, MB_YESNO | MB_ICONWARNING);
    lua_pushboolean(L, id == IDYES);

#else
    SDL_MessageBoxButtonData buttons[] = {
        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes"},
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No" },
    };
    SDL_MessageBoxData data = {
        .title      = title,
        .message    = msg,
        .numbuttons = 2,
        .buttons    = buttons,
    };
    int buttonid;
    SDL_ShowMessageBox(&data, &buttonid);
    lua_pushboolean(L, buttonid == 1);
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

    uint64_t file_time = lite_file_write_time(lite_string_view(path, (uint32_t)len, 0u));
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
    char* text = SDL_GetClipboardText();
    if (!text)
    {
        return 0;
    }
    lua_pushstring(L, text);
    SDL_free(text);
    return 1;
}

static int f_set_clipboard(lua_State* L)
{
    const char* text = luaL_checkstring(L, 1);
    SDL_SetClipboardText(text);
    return 0;
}

static int f_get_time(lua_State* L)
{
    double n =
        SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency();
    lua_pushnumber(L, n);
    return 1;
}

static int f_sleep(lua_State* L)
{
    double n = luaL_checknumber(L, 1);
    SDL_Delay((Uint32)(n * 1000));
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

static const luaL_Reg lib[] = {
    {"poll_event",          f_poll_event         },
    {"wait_event",          f_wait_event         },
    {"set_cursor",          f_set_cursor         },
    {"set_window_title",    f_set_window_title   },
    {"set_window_mode",     f_set_window_mode    },
    {"get_window_opacity",  f_get_window_opacity },
    {"set_window_opacity",  f_set_window_opacity },
    {"window_has_focus",    f_window_has_focus   },
    {"show_confirm_dialog", f_show_confirm_dialog},
    {"chdir",               f_chdir              },
    {"file_time", f_file_time },
    {"is_binary_file", f_is_binary_file },
    {"list_dir",            f_list_dir           },
    {"absolute_path",       f_absolute_path      },
    {"get_file_info",       f_get_file_info      },
    {"get_clipboard",       f_get_clipboard      },
    {"set_clipboard",       f_set_clipboard      },
    {"get_time",            f_get_time           },
    {"sleep",               f_sleep              },
    {"exec",                f_exec               },
    {"fuzzy_match",         f_fuzzy_match        },
    {NULL,                  NULL                 }
};

int luaopen_system(lua_State* L)
{
    luaL_newlib(L, lib);
    return 1;
}