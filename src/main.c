#include "renderer.h"
#include "lite_startup.h"

#include <SDL2/SDL.h>

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif

SDL_Window* window;

static void init_window_icon(void)
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

#ifdef _WIN32
#ifdef NDEBUG
#define USE_TERMINAL_CONSOLE 0
#else
#define USE_TERMINAL_CONSOLE 1
#endif
#else
#define USE_TERMINAL_CONSOLE 0
#endif

#ifdef _WIN32
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmd,
                   int nShowCmd)
#else
int main(int argc, char** argv)
#endif
{
#if defined(_WIN32)
    (void)hInstance;
    (void)hPrevInstance;
    (void)pCmd;
    (void)nShowCmd;
    
    int    argc = __argc;
    char** argv = __argv;
#endif
    
#if USE_TERMINAL_CONSOLE
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif
    
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
    
    window = SDL_CreateWindow(
                              "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (int)(dm.w * 0.8),
                              (int)(dm.h * 0.8),
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN);
    init_window_icon();
    lite_renderer_init(window);
    
    const LiteStartupParams startup_params = {
        .argc = (uint32_t)argc, 
        .argv = (const char**)argv,
        
        .title = "lite",
        .window_handle = window,
        
        .flags = LiteStartupFlags_None
    };
    lite_startup(startup_params);
    
    lite_renderer_deinit();
    SDL_DestroyWindow(window);
    
#if USE_TERMINAL_CONSOLE
    FreeConsole();
#endif
    
    return EXIT_SUCCESS;
}

//! EOF
