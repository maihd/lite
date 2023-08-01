#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#endif

#include "lite_memory.h"
#include "lite_string.h"
#include "lite_window.h"
#include "lite_renderer.h"

enum { LITE_EVENT_QUEUE_SIZE = 64 };

static HWND     window;

static HDC      hDC;
static HDC      hSurface;
static HBITMAP  hSurfaceBitmap;

static void*    surface_pixels;
static int32_t  surface_width;
static int32_t  surface_height;

static LiteWindowEvent  s_events_queue[LITE_EVENT_QUEUE_SIZE];
static int32_t          s_events_queue_head = 0;
static int32_t          s_events_queue_tail = 0;

static LiteArena*       s_events_arena      = nullptr;
static LiteArenaTemp    s_events_arena_temp = { 0 };

static void lite_push_event(LiteWindowEvent event)
{
    s_events_queue[s_events_queue_tail] = event;
    s_events_queue_tail = (s_events_queue_tail + 1) % LITE_EVENT_QUEUE_SIZE;
}

static LiteWindowEvent lite_pop_event(void)
{
    if (s_events_queue_head == s_events_queue_tail)
    {
        if (s_events_arena_temp.arena != nullptr)
        {
            lite_arena_end_temp(s_events_arena_temp);
            s_events_arena_temp.arena = nullptr;
        }

        return (LiteWindowEvent){
            .type = LiteWindowEventType_None
        };
    }

    LiteWindowEvent event = s_events_queue[s_events_queue_head];
    s_events_queue_head = (s_events_queue_head + 1) % LITE_EVENT_QUEUE_SIZE;
    return event;
}

static LiteStringView lite_get_key_name(WORD scanCode, WORD extended)
{
    if (extended)
    {
        if (scanCode != 0x45)
        {
            scanCode |= 0xE000;
        }
    }
    else
    {
        if (scanCode == 0x45)
        {
            scanCode = 0xE11D45;
        }
        else if (scanCode == 0x54)
        {
            scanCode = 0xE037;
        }
    }

    LONG lParam = 0;
    if (extended)
    {
        if (extended == 0xE11D00)
        {
            lParam = 0x45 << 16;
        }
        else
        {
            lParam = (0x100 | (scanCode & 0xff)) << 16;
        }
    }
    else
    {
        lParam = scanCode << 16;

        if (scanCode == 0x45)
        {
            lParam |= (0x1 << 24);
        }
    }

    LiteArena* frame_arena = s_events_arena;
    const uint32_t length = 16;
    char* text = (char*)lite_arena_acquire(frame_arena, length);
    int text_length = GetKeyNameTextA(lParam, text, length);
    for (int i = 0; i < text_length; i++)
    {
        text[i] = tolower(text[i]);
    }

    return lite_string_view(text, (uint32_t)text_length, 0);
}

static LRESULT WINAPI lite_win32_window_proc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    static int32_t prev_mouse_x = -1;
    static int32_t prev_mouse_y = -1;

    if (s_events_arena == nullptr)
    {
        s_events_arena = lite_arena_create(1024, 1024 * 1024, 1);
    }

    if (s_events_arena_temp.arena == nullptr)
    {
        s_events_arena_temp = lite_arena_begin_temp(s_events_arena);
    }

    LiteArena* frame_arena = s_events_arena;

    switch (msg)
    {
    case WM_SIZE:
    {
        //ReleaseDC(window, hSurface);
        UINT width = LOWORD(lParam);
        UINT height = HIWORD(lParam);

        DeleteBitmap(hSurfaceBitmap);
        DeleteDC(hSurface);

        surface_width = (int32_t)width;
        surface_height = (int32_t)height;

        hSurface = CreateCompatibleDC(hDC);

        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = (LONG)width;
        bmi.bmiHeader.biHeight = -(LONG)height; // top-down
        bmi.bmiHeader.biSizeImage = (DWORD)(surface_width * surface_height * sizeof(LiteColor));
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        hSurfaceBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)(&surface_pixels), nullptr, 0);
        SelectObject(hSurface, hSurfaceBitmap);
        
        lite_push_event((LiteWindowEvent){
            .type = LiteWindowEventType_Resized,
            .resized.width = (int32_t)width,
            .resized.height = (int32_t)height,
        });
        return 0;
    }

    //case WM_EXPOSED:
    //    return 0;

    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_SETFOCUS:
        break;

    case WM_QUIT:
        lite_push_event((LiteWindowEvent){ .type = LiteWindowEventType_Quit });
        return 0;

    case WM_DROPFILES:
        break;

    case WM_KEYUP:
    {
        WORD scanCode = (lParam >> 16) & 0xff;
        WORD extended = (lParam >> 24) & 0x1;

        lite_push_event((LiteWindowEvent){
            .type = LiteWindowEventType_KeyUp,
            .key_up.key_name = lite_get_key_name(scanCode, extended),
        });
        return 0;
    }

    case WM_KEYDOWN:
    {
        WORD scanCode = (lParam >> 16) & 0xff;
        WORD extended = (lParam >> 24) & 0x1;

        lite_push_event((LiteWindowEvent){
            .type = LiteWindowEventType_KeyDown,
            .key_down.key_name = lite_get_key_name(scanCode, extended),
        });
        return 0;
    }

    case WM_INPUT:
    {
        UINT uTextLength = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, 0, sizeof(RAWINPUTHEADER));
        char* text = (char*)lite_arena_acquire(frame_arena, (size_t)uTextLength + 1);
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, text, &uTextLength, sizeof(RAWINPUTHEADER));
        text[uTextLength] = 0;

        lite_push_event((LiteWindowEvent){
            .type = LiteWindowEventType_TextInput,
            .text_input.text = lite_string_view(text, (uint32_t)uTextLength, 0),
        });
        return 0;
    }

    case WM_LBUTTONUP:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        lite_push_event((LiteWindowEvent){
            .type = LiteWindowEventType_MouseUp,
            .mouse_up.button_name = lite_string_lit("left"),
            .mouse_up.x = (int32_t)x,
            .mouse_up.y = (int32_t)y,
            .mouse_up.clicks = 1,
        });
        return 0;
    }

    case WM_RBUTTONUP:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        lite_push_event((LiteWindowEvent){
            .type = LiteWindowEventType_MouseUp,
            .mouse_up.button_name = lite_string_lit("right"),
            .mouse_up.x = (int32_t)x,
            .mouse_up.y = (int32_t)y,
            .mouse_up.clicks = 1,
        });
        return 0;
    }

    case WM_MBUTTONUP:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        lite_push_event((LiteWindowEvent){
            .type = LiteWindowEventType_MouseUp,
            .mouse_up.button_name = lite_string_lit("middle"),
            .mouse_up.x = (int32_t)x,
            .mouse_up.y = (int32_t)y,
            .mouse_up.clicks = 1,
        });
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        lite_push_event((LiteWindowEvent){
            .type = LiteWindowEventType_MouseDown,
            .mouse_up.button_name = lite_string_lit("left"),
            .mouse_up.x = (int32_t)x,
            .mouse_up.y = (int32_t)y,
            .mouse_up.clicks = 1,
        });
        return 0;
    }

    case WM_RBUTTONDOWN:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        lite_push_event((LiteWindowEvent){
            .type = LiteWindowEventType_MouseDown,
            .mouse_up.button_name = lite_string_lit("right"),
            .mouse_up.x = (int32_t)x,
            .mouse_up.y = (int32_t)y,
            .mouse_up.clicks = 1,
        });
        return 0;
    }

    case WM_MBUTTONDOWN:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        lite_push_event((LiteWindowEvent){
            .type = LiteWindowEventType_MouseDown,
            .mouse_up.button_name = lite_string_lit("middle"),
            .mouse_up.x = (int32_t)x,
            .mouse_up.y = (int32_t)y,
            .mouse_up.clicks = 1,
        });
        return 0;
    }

    case WM_MOVE:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        LiteWindowEvent event;
        event.type = LiteWindowEventType_MouseMove;
        event.mouse_move.x = (int32_t)x;
        event.mouse_move.y = (int32_t)y;

        if (prev_mouse_x < 0)
        {
            event.mouse_move.dx = 0;
        }
        else
        {
            event.mouse_move.dx = event.mouse_move.x - prev_mouse_x;
        }

        if (prev_mouse_y < 0)
        {
            event.mouse_move.dy = 0;
        }
        else
        {
            event.mouse_move.dy = event.mouse_move.y - prev_mouse_y;
        }

        prev_mouse_x = x;
        prev_mouse_y = y;

        lite_push_event(event);
        return 0;
    }

    case WM_MOUSEWHEEL:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        lite_push_event((LiteWindowEvent){
            .type = LiteWindowEventType_MouseWheel,
            .mouse_wheel.x = (int32_t)x,
            .mouse_wheel.y = (int32_t)y,
        });
        return 0;
    }

    default:
        break;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

void lite_sleep(uint64_t ms)
{
    Sleep((DWORD)ms);
}

void lite_usleep(uint64_t us)
{
    /* 'NTSTATUS NTAPI NtDelayExecution(BOOL Alerted, PLARGE_INTEGER time);' */
    /* 'typedef LONG NTSTATUS;' =)) */
    /* '#define NTAPI __stdcall' =)) */
    typedef LONG(__stdcall* NtDelayExecutionFN)(BOOL, PLARGE_INTEGER);

    static bool done_finding;
    static NtDelayExecutionFN NtDelayExecution;

    if (!NtDelayExecution && !done_finding)
    {
        done_finding = 1;
        HMODULE module = GetModuleHandleA("ntdll.dll");
        const char* func = "NtDelayExecution";
        NtDelayExecution = (NtDelayExecutionFN)GetProcAddress(module, func);
    }

    if (NtDelayExecution)
    {
        LARGE_INTEGER times;
        times.QuadPart = (LONGLONG)(-us * 10); // Timer is precise as 100 nanoseconds
        (void)NtDelayExecution(FALSE, &times);
    }
    else
    {
        Sleep((DWORD)(us / 1000));
    }
}

uint64_t lite_cpu_ticks(void)
{
    LARGE_INTEGER liCounter;
    QueryPerformanceCounter(&liCounter);
    return (uint64_t)liCounter.QuadPart;
}

uint64_t lite_cpu_frequency(void)
{
    LARGE_INTEGER liFrequency;
    QueryPerformanceFrequency(&liFrequency);
    return (uint64_t)liFrequency.QuadPart;
}

LiteStringView lite_clipboard_get(void)
{
    if (!IsClipboardFormatAvailable(CF_TEXT))
    {
        return lite_string_lit("");
    }

    if (!OpenClipboard(window))
    {
        return lite_string_lit("");
    }

    LiteStringView result = lite_string_lit("");
    HGLOBAL hGlobal = GetClipboardData(CF_TEXT);
    if (hGlobal != nullptr)
    {
        const char* lpStr = (const char*)GlobalLock(hGlobal);
        if (lpStr != nullptr)
        {
            result.buffer = lpStr;
            result.length = lite_string_count(lpStr);
            GlobalUnlock(hGlobal);
        }
    }

    CloseClipboard();
    return result;
}

bool lite_clipboard_set(LiteStringView text)
{
    if (!OpenClipboard(window))
    {
        return false;
    }

    HGLOBAL hGlobalCopy = GlobalAlloc(GMEM_MOVEABLE, text.length + 1);
    if (hGlobalCopy == nullptr)
    {
        CloseClipboard();
        return false;
    }

    void* lpStr = GlobalLock(hGlobalCopy);
    memcpy(lpStr, text.buffer, text.length + 1);

    bool result = SetClipboardData(CF_TEXT, hGlobalCopy) != nullptr;

    GlobalUnlock(hGlobalCopy);
    CloseClipboard();

    return result;
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

    // @maihd(maihd): convert this to win32
    //SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    //atexit(SDL_Quit);

    // Acquire focus when mouse click
    // @todo(maihd): convert this to win32
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

    DWORD window_titlebar_size = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME);
    DWORD window_border_size = GetSystemMetrics(SM_CXFRAME);

    DWORD window_width = (DWORD)((float)monitor_width * 0.8f) + window_border_size;
    DWORD window_height = (DWORD)((float)monitor_height * 0.8f) + window_titlebar_size;

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

    hDC = GetDC(window);

    ShowWindow(window, true);
}

void lite_window_close(void)
{
    DeleteBitmap(hSurfaceBitmap);
    DeleteDC(hSurface);

    ReleaseDC(window, hDC);
    DestroyWindow(window);
    window = nullptr;
}

void* lite_window_handle(void)
{
    return (void*)(uintptr_t)window;
}

void* lite_window_surface(int32_t* width, int32_t* height)
{
    *width = surface_width;
    *height = surface_height;
    return surface_pixels;
}

void lite_window_show(void)
{
    ShowWindow(window, true);
}

void lite_window_hide(void)
{
    ShowWindow(window, false);
}

void lite_window_set_mode(LiteWindowMode mode)
{
    switch (mode)
    {
    case LiteWindowMode_Normal:
        PostMessageA(window, WM_SYSCOMMAND, SC_RESTORE, 0);
        break;

    case LiteWindowMode_Maximized:
        PostMessageA(window, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        break;

    case LiteWindowMode_FullScreen:
        break;
    }
}

void lite_window_set_title(const char* title)
{
    SetWindowTextA(window, title);
}

void lite_window_set_cursor(LiteCursor cursor)
{
    static LPWSTR cursor_names[LiteCursor_COUNT] = {
        nullptr,
        IDC_HAND,
        IDC_ARROW,
        IDC_IBEAM,
        IDC_SIZEWE,
        IDC_SIZENS,
    };
    static HCURSOR cursor_caches[LiteCursor_COUNT];

    HCURSOR hCursor = cursor_caches[cursor];
    if (hCursor == nullptr)
    {
        hCursor = LoadCursor(nullptr, cursor_names[cursor]);
        cursor_caches[cursor] = hCursor;
    }
    SetCursor(hCursor);
}

float lite_window_dpi(void)
{
    return (float)GetDpiForWindow(window);
}

bool lite_window_has_focus(void)
{
    bool result = GetFocus() == window;
    return result;
}

void lite_window_update_rects(struct LiteRect* rects, uint32_t count)
{
    //for (uint32_t i = 0; i < count; i++)
    //{
    //    const LiteRect rect = rects[i];
    //    BitBlt(hDC, rect.x, rect.y, rect.width, rect.height, hSurface, rect.x, rect.y, SRCCOPY);
    //}

    BitBlt(hDC, 0, 0, surface_width, surface_height, hSurface, 0, 0, SRCCOPY);
}

void lite_window_message_box(const char* title, const char* message)
{
    MessageBoxA(window, message, title, MB_OK);
}

bool lite_window_confirm_dialog(const char* title, const char* message)
{
    return MessageBoxA(window, message, title, MB_YESNO) == IDYES;
}

LiteWindowEvent lite_window_poll_event(void)
{
    MSG msg;
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return lite_pop_event();
}

bool lite_window_wait_event(uint64_t time_us)
{
    MSG msg;
    uint64_t time_remain_us = time_us;
    uint64_t time_step_us = 1000 * 1000 / 60;
    while (!PeekMessageA(&msg, nullptr, 0, 0, PM_NOREMOVE))
    {
        if (time_remain_us == 0)
        {
            return false;
        }
        else if (time_step_us > time_remain_us)
        {
            lite_usleep(time_remain_us);
            time_remain_us = 0;
        }
        else
        {
            lite_usleep(time_step_us);
            time_remain_us -= time_step_us;
        }
    }

    return true;
}

//! EOF

