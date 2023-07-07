#include <stdio.h>
#include <string.h>
#include <assert.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "lite_window.h"

HWND window;

static LRESULT WINAPI lite_win32_window_proc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    return DefWindowProcW(hwnd, msg, wParam, lParam);
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
    SetProcessDPIAware();
    SetThreadExecutionState(ES_CONTINUOUS);

    //SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    //atexit(SDL_Quit);

    // Acquire focus when mouse click
    //#if SDL_VERSION_ATLEAST(2, 0, 5)
    //SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    //#endif

    // Get current display monitor size
#if 0
//     HMONITOR hMonitor = ;
//     MONITORINFO mi;

//     SDL_DisplayMode dm;
//     SDL_GetCurrentDisplayMode(0, &dm);
#else
    // Use old-style function to get monitor size
    DWORD monitor_width = GetSystemMetrics(SM_CXSCREEN);
    DWORD monitor_height = GetSystemMetrics(SM_CYSCREEN);
#endif

    const char* window_class = "lite_window_class";
    WNDCLASSA wc = {0};
    wc.lpfnWndProc    = lite_win32_window_proc;
    wc.hInstance      = GetModuleHandle(nullptr);
    wc.lpszClassName  = window_class;
    if (!RegisterClassA(&wc))
    {
        assert(0);
        return;
    }

    DWORD window_width = (DWORD)((float)monitor_width * 0.8f);
    DWORD window_height = (DWORD)((float)monitor_height * 0.8f);

    DWORD window_x = (monitor_width - window_width) / 2;
    DWORD window_y = (monitor_height - window_height) / 2;

    window = CreateWindowA(
        window_class,
        "",
        WS_OVERLAPPEDWINDOW,
        window_x, window_y,
        window_width, window_height,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );
    if (window == nullptr)
    {
        // @todo(maihd): handle error
        return;
    }

    ShowWindow(window, true);
}

void lite_window_close(void)
{
    DestroyWindow(window);
    window = nullptr;
}

void* lite_window_handle(void)
{
    return (void*)(uintptr_t)window;
}

void lite_window_show(void)
{
    ShowWindow(window, true);
}

void lite_window_hide(void)
{
    ShowWindow(window, false);
}

float lite_window_dpi(void)
{
    return (float)GetDpiForWindow(window);
}

//! EOF

