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

void lite_sleep(uint64_t ms)
{
    SDL_Delay((Uint32)ms);
}

void lite_usleep(uint64_t us)
{
    SDL_Delay((Uint32)(us / 1000));
}

uint64_t lite_cpu_ticks(void)
{
    return SDL_GetPerformanceCounter();
}

uint64_t lite_cpu_frequency(void)
{
    return SDL_GetPerformanceFrequency();
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

void* lite_window_surface(int32_t* width, int32_t* height)
{
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    if (width)  *width   = (int32_t)surface->w;
    if (height) *height  = (int32_t)surface->h;
    return surface->pixels;
}

void lite_window_show(void)
{
    SDL_ShowWindow(window);
}

void lite_window_hide(void)
{
    SDL_HideWindow(window);
}

void lite_window_set_mode(LiteWindowMode mode)
{

}

void lite_window_set_title(const char* title)
{

}

void lite_window_set_cursor(LiteCursor cursor)
{

}

float lite_window_dpi(void)
{
    float dpi;
    SDL_GetDisplayDPI(0, NULL, &dpi, NULL);
    return dpi;
}

bool lite_window_has_focus(void)
{
    return false;
}

void lite_window_update_rects(struct LiteRect* rects, uint32_t count)
{
    SDL_UpdateWindowSurfaceRects(window, (const SDL_Rect*)rects, (int)count);
}

void lite_window_message_box(const char* title, const char* message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, window);
}

bool lite_window_confirm_dialog(const char* title, const char* message)
{
    return false;
}

static const char* lite_button_name(int button)
{
    switch (button)
    {
        case 1: return "left";
        case 2: return "middle";
        case 3: return "right";
        default: return "?";
    }
}

static char* lite_key_name(char* dst, int sym)
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

LiteWindowEvent lite_window_poll_event(void)
{
    // @todo(maihd): convert to frame arena memory
    static char      buf[16];
    int       mx, my, wx, wy;
    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            return (LiteWindowEvent){
                .type = LiteWindowEventType_Quit
            };

        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                return (LiteWindowEvent){
                    .type = LiteWindowEventType_Resized,
                    .resized = {
                        .width = e.window.data1,
                        .height = e.window.data2,
                    }
                };
            }

            if (e.window.event == SDL_WINDOWEVENT_EXPOSED)
            {
                return (LiteWindowEvent){
                    .type = LiteWindowEventType_Exposed
                };
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
            return (LiteWindowEvent){
                .type = LiteWindowEventType_DropFile,
                .drop_file = {
                    .file_path = e.drop.file, // @note(maihd): may leak,
                    .x = mx - wx,
                    .y = my - wy
                }
            };
//             SDL_free(e.drop.file);

        case SDL_KEYDOWN:
            return (LiteWindowEvent){
                .type = LiteWindowEventType_KeyDown,
                .key_down = {
                    .key_name = lite_key_name(buf, e.key.keysym.sym)
                }
            };

        case SDL_KEYUP:
            return (LiteWindowEvent){
                .type = LiteWindowEventType_KeyUp,
                .key_up = {
                    .key_name = lite_key_name(buf, e.key.keysym.sym)
                }
            };

        case SDL_TEXTINPUT:
            return (LiteWindowEvent){
                .type = LiteWindowEventType_TextInput,
                .text_input = {
                    .text = e.text.text
                }
            };

        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT)
            {
                SDL_CaptureMouse(true);
            }

            return (LiteWindowEvent){
                .type = LiteWindowEventType_MouseDown,
                .mouse_down = {
                    .button_name = lite_button_name(e.button.button),
                    .x = e.button.x,
                    .y = e.button.y,
                    .clicks = e.button.clicks,
                }
            };

        case SDL_MOUSEBUTTONUP:
            if (e.button.button == SDL_BUTTON_LEFT)
            {
                SDL_CaptureMouse(false);
            }

            return (LiteWindowEvent){
                .type = LiteWindowEventType_MouseUp,
                .mouse_up = {
                    .button_name = lite_button_name(e.button.button),
                    .x = e.button.x,
                    .y = e.button.y,
                    .clicks = e.button.clicks,
                }
            };

        case SDL_MOUSEMOTION:
            return (LiteWindowEvent){
                .type = LiteWindowEventType_MouseMove,
                .mouse_move = {
                    .x = e.motion.x,
                    .y = e.motion.y,
                    .dx = e.motion.xrel,
                    .dy = e.motion.yrel,
                }
            };

        case SDL_MOUSEWHEEL:
            return (LiteWindowEvent){
                .type = LiteWindowEventType_MouseWheel,
                .mouse_wheel = {
                    .x = e.wheel.x,
                    .y = e.wheel.y,
                }
            };

        default:
            break;
        }
    }

    return (LiteWindowEvent){
        .type = LiteWindowEventType_None
    };
}

bool lite_window_wait_event(uint64_t time_us)
{
    return SDL_WaitEventTimeout(nullptr, (int)(time_us / 1000000));
}

//! EOF

