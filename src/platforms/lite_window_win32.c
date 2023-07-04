#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "lite_window.h"

HWND window;

static void lite_window_load_icon(void)
{
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
    HINSTANCE lib = LoadLibraryA("user32.dll");
    int (*SetProcessDPIAware)() =
    (void*)GetProcAddress(lib, "SetProcessDPIAware");
    if (SetProcessDPIAware != NULL)
    {
        SetProcessDPIAware();
    }
    SetThreadExecutionState(ES_CONTINUOUS);
    
    //SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    //atexit(SDL_Quit);
    
    // Acquire focus when mouse click
    //#if SDL_VERSION_ATLEAST(2, 0, 5)
    //SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    //#endif
    
    // Get current display monitor size
#if 0
    HMONITOR hMonitor = ;
    MONITORINFO mi;
    
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
#else
    // Use old-style function to get monitor size
#endif
    
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

