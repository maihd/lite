#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <SDL2/SDL.h>

#ifdef _WIN32
//#include <windows.h>
#elif __linux__
#include <unistd.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif

#include "lite_window.h"

SDL_Window* window;

static void lite_window_load_icon(void)
{
#if !defined(_WIN32) && !defined(__MINGW32__)
#include "../res/icon.inl"
    (void)icon_rgba_len; /* unused */
    SDL_Surface* surf =
        SDL_CreateRGBSurfaceFrom(icon_rgba, 64, 64, 32, 64 * 4, 0x000000ff,
                                 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_SetWindowIcon(window, surf);
    SDL_FreeSurface(surf);
#endif
}

void lite_console_open(void)
{
#if defined(_WIN32)
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif
}

void lite_console_close(void)
{
#if defined(_WIN32)
    FreeConsole();
#endif
}

void lite_window_open(void)
{
#ifdef _WIN32
    HINSTANCE lib = LoadLibraryA("user32.dll");
    int (*SetProcessDPIAware)() =
    (void*)GetProcAddress(lib, "SetProcessDPIAware");
    if (SetProcessDPIAware != NULL)
    {
        SetProcessDPIAware();
    }
#endif
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_EnableScreenSaver();
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    atexit(SDL_Quit);
    
#ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR /* Available since 2.0.8 */
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
#endif
#if SDL_VERSION_ATLEAST(2, 0, 5)
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#endif
    
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    
    window = SDL_CreateWindow("", 
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (int)(dm.w * 0.8), (int)(dm.h * 0.8),
                              SDL_WINDOW_RESIZABLE 
                              | SDL_WINDOW_ALLOW_HIGHDPI 
                              | SDL_WINDOW_HIDDEN);
    if (window)
    {
        // @todo(maihd): handle error
    }
    
    lite_window_load_icon();
}

void lite_window_close(void)
{
    SDL_DestroyWindow(window);
    window = nullptr;
}

void* lite_window_handle(void)
{
    return window;
}

//! EOF

