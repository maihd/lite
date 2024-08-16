#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

#include "lite_log.h"
#include "lite_window.h"


SDL_Window*     s_window;
LiteWindowMode  s_current_window_mode;

int32_t         s_window_x_before_maximize;
int32_t         s_window_y_before_maximize;

int32_t         s_window_width_before_maximize;
int32_t         s_window_height_before_maximize;


static void lite_window_load_icon(void)
{
#if !defined(_WIN32) && !defined(__MINGW32__)
#include "../res/icon.inl"
    (void)icon_rgba_len; /* unused */
    SDL_Surface* surf =
        SDL_CreateRGBSurfaceFrom(icon_rgba, 64, 64, 32, 64 * 4, 0x000000ff,
                                 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_SetWindowIcon(s_window, surf);
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


LiteStringView lite_clipboard_get(void)
{
    char* sdl_text = SDL_GetClipboardText();
    LiteStringView text = lite_string_temp(sdl_text);
    SDL_free(sdl_text);
    return text;
}


bool lite_clipboard_set(LiteStringView text)
{
    return SDL_SetClipboardText(text.buffer) == 0;
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

    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    SDL_SetHint(SDL_HINT_MOUSE_DOUBLE_CLICK_TIME, "175");
	SDL_SetHint("SDL_MOUSE_DOUBLE_CLICK_RADIUS", "4");

	// This hint tells SDL to respect borderless window as a normal window.
	// For example, the window will sit right on top of the taskbar instead
	// of obscuring it.
	SDL_SetHint("SDL_BORDERLESS_WINDOWED_STYLE", "1");

	// This hint tells SDL to allow the user to resize a borderless window.
	// It also enables aero-snap on Windows apparently.
	SDL_SetHint("SDL_BORDERLESS_RESIZABLE_STYLE", "1");


    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);

    Uint32 window_flags = SDL_WINDOW_RESIZABLE
                        | SDL_WINDOW_ALLOW_HIGHDPI
                        | SDL_WINDOW_HIDDEN;

    s_window = SDL_CreateWindow("",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              (int)(dm.w * 0.8), (int)(dm.h * 0.8),
                              window_flags);
    if (s_window)
    {
        // @todo(maihd): handle error
    }


    lite_window_load_icon();
}


void lite_window_close(void)
{
    SDL_Event ev;
    ev.type = SDL_QUIT;
    SDL_PushEvent(&ev);

    //SDL_DestroyWindow(s_window);
    //s_window = nullptr;
}


void* lite_window_handle(void)
{
    return s_window;
}


void* lite_window_surface(int32_t* width, int32_t* height)
{
    SDL_Surface* surface = SDL_GetWindowSurface(s_window);
    if (surface)
    {
        if (width)  *width    = (int32_t)surface->w;
        if (height) *height = (int32_t)surface->h;
        return surface->pixels;
    }

    if (width)  *width    = 0;
    if (height) *height = 0;
    return nullptr;
}


void lite_window_show(void)
{
    SDL_ShowWindow(s_window);
}


void lite_window_hide(void)
{
    SDL_HideWindow(s_window);
}


typedef struct LiteHitTestInfo
{
    int32_t title_height;
    int32_t controls_width;
    int32_t resize_border;
} LiteHitTestInfo;

static SDL_HitTestResult SDLCALL lite_window_hit_test(SDL_Window* window, const SDL_Point* pt, void* data)
{
    const LiteHitTestInfo*  hit_info        = (LiteHitTestInfo*)data;
    const int32_t           resize_border   = hit_info->resize_border;
    const int32_t           controls_width  = hit_info->controls_width;

    int32_t w, h;
    SDL_GetWindowSize(window, &w, &h);

    if (pt->y < hit_info->title_height 
		&& pt->y > hit_info->resize_border 
		&& pt->x > resize_border 
		&& pt->x < w - controls_width)
    {
        return SDL_HITTEST_DRAGGABLE;
    }

    #define REPORT_RESIZE_HIT(name) return SDL_HITTEST_RESIZE_##name

    if (pt->x < resize_border && pt->y < resize_border)
    {
        REPORT_RESIZE_HIT(TOPLEFT);
    }
    else if (pt->x > resize_border && pt->x < w - controls_width && pt->y < resize_border)
    {
        REPORT_RESIZE_HIT(TOP);
    }
    else if (pt->x > w - resize_border && pt->y < resize_border)
    {
        REPORT_RESIZE_HIT(TOPRIGHT);
    }
    else if (pt->x > w - resize_border && pt->y > resize_border && pt->y < h - resize_border)
    {
        REPORT_RESIZE_HIT(RIGHT);
    }
    else if (pt->x > w - resize_border && pt->y > h - resize_border)
    {
        REPORT_RESIZE_HIT(BOTTOMRIGHT);
    }
    else if (pt->x < w - resize_border && pt->x > resize_border && pt->y > h - resize_border)
    {
        REPORT_RESIZE_HIT(BOTTOM);
    }
    else if (pt->x < resize_border && pt->y > h - resize_border)
    {
        REPORT_RESIZE_HIT(BOTTOMLEFT);
    }
    else if (pt->x < resize_border && pt->y < h - resize_border && pt->y > resize_border)
    {
        REPORT_RESIZE_HIT(LEFT);
    }

    return SDL_HITTEST_NORMAL;
}


void lite_window_show_titlebar(void)
{
    SDL_SetWindowBordered(s_window, SDL_TRUE);
    SDL_SetWindowHitTest(s_window, nullptr, nullptr);
}


void lite_window_hide_titlebar(void)
{
    SDL_SetWindowBordered(s_window, SDL_FALSE);
}


void lite_window_config_hit_test(int32_t title_height, int32_t controls_width, int32_t resize_border)
{
    static LiteHitTestInfo window_hit_info;
    window_hit_info.title_height   = title_height;
    window_hit_info.controls_width = controls_width;
    window_hit_info.resize_border  = resize_border;
    SDL_SetWindowHitTest(s_window, lite_window_hit_test, &window_hit_info);
}


void lite_window_set_position(int32_t x, int32_t y)
{
    SDL_SetWindowPosition(s_window, x, y);
}


void lite_window_get_position(int32_t* x, int32_t* y)
{
    SDL_GetWindowPosition(s_window, x, y);
}


void lite_window_minimize(void)
{
    SDL_MinimizeWindow(s_window);
}


void lite_window_maximize(void)
{
    if (lite_window_get_mode() == LiteWindowMode_Normal)
    {
//         lite_window_get_position(&s_window_x_before_maximize, &s_window_y_before_maximize);
//         lite_window_get_size(&s_window_width_before_maximize, &s_window_height_before_maximize);
//         s_current_window_mode = LiteWindowMode_Maximized;

//         SDL_Rect display_bounds;
//         if (SDL_GetDisplayUsableBounds(SDL_GetWindowDisplayIndex(s_window), &display_bounds) == 0)
//         {
//             lite_window_set_position(display_bounds.x, display_bounds.y);
// //             lite_window_set_size(display_bounds.w, display_bounds.h);
//             SDL_SetWindowSize(s_window, display_bounds.w, display_bounds.h);
//         }
//         else
//         {
//             SDL_MaximizeWindow(s_window);
//         }
        SDL_MaximizeWindow(s_window);
    }
}


bool lite_window_is_maximized(void)
{
    //return s_current_window_mode == LiteWindowMode_Maximized;
    return SDL_GetWindowFlags(s_window) & SDL_WINDOW_MAXIMIZED;
}


void lite_window_toggle_maximize(void)
{
    if (lite_window_is_maximized())
    {
        lite_window_restore_maximize();
    }
    else
    {
        lite_window_maximize();
    }
}


void lite_window_restore_maximize(void)
{
    SDL_RestoreWindow(s_window);
//     if (lite_window_is_maximized())
//     {
//         s_current_window_mode = LiteWindowMode_Normal;
//         lite_window_set_position(s_window_x_before_maximize, s_window_y_before_maximize);
//         lite_window_set_size(s_window_width_before_maximize, s_window_height_before_maximize);

//         SDL_SetWindowSize(s_window, s_window_width_before_maximize, s_window_height_before_maximize);
//     }
}


LiteWindowMode lite_window_get_mode(void)
{
    Uint32 flags = SDL_GetWindowFlags(s_window);
    if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
    {
        return LiteWindowMode_FullScreen;
    }

//     return s_current_window_mode;
    if (flags & SDL_WINDOW_MAXIMIZED)
    {
        return LiteWindowMode_Maximized;
    }

    return LiteWindowMode_Normal;
}


void lite_window_set_mode(LiteWindowMode mode)
{
    // @note(maihd):
    //      when mode != fullscreen,
    //      must be remove SDL_WINDOW_FULLSCREEN_DESKTOP flag from window
    SDL_SetWindowFullscreen(s_window,
        mode == LiteWindowMode_FullScreen
        ? SDL_WINDOW_FULLSCREEN_DESKTOP
        : 0);

    if (mode == LiteWindowMode_Normal)
    {
        SDL_RestoreWindow(s_window);
    }

    if (mode == LiteWindowMode_Maximized)
    {
        SDL_MaximizeWindow(s_window);
    }
}


void lite_window_set_title(const char* title)
{
    SDL_SetWindowTitle(s_window, title);
}


void lite_window_set_cursor(LiteCursor cursor)
{
    static SDL_Cursor* sdl_cursor_cache[16];
    static const int sdl_cursor_enums[] = {
        0,
        SDL_SYSTEM_CURSOR_HAND,
        SDL_SYSTEM_CURSOR_ARROW,
        SDL_SYSTEM_CURSOR_IBEAM,
        SDL_SYSTEM_CURSOR_SIZEWE,
        SDL_SYSTEM_CURSOR_SIZENS,
    };

    int         n = sdl_cursor_enums[(uint32_t)cursor];
    SDL_Cursor* sdl_cursor = sdl_cursor_cache[n];
    if (!sdl_cursor)
    {
        sdl_cursor = SDL_CreateSystemCursor(n);
        sdl_cursor_cache[n] = sdl_cursor;
    }
    SDL_SetCursor(sdl_cursor);
}


void lite_window_get_mouse_position(int32_t* x, int32_t* y)
{
    SDL_GetMouseState(x, y);
}


void lite_window_get_global_mouse_position(int32_t* x, int32_t* y)
{
    SDL_GetGlobalMouseState(x, y);
}


float lite_window_get_opacity(void)
{
    float opacity = 1.0f;
    SDL_GetWindowOpacity(s_window, &opacity);
    return opacity;
}


void lite_window_set_opacity(float opacity)
{
    SDL_SetWindowOpacity(s_window, opacity);
}


void lite_window_get_size(int32_t* width, int32_t* height)
{
    SDL_GetWindowSize(s_window, width, height);
}


float lite_window_dpi(void)
{
    float dpi;
    SDL_GetDisplayDPI(0, NULL, &dpi, NULL);
    return dpi;
}


bool lite_window_has_focus(void)
{
    Uint32 flags = SDL_GetWindowFlags(s_window);
    return flags & SDL_WINDOW_INPUT_FOCUS;
}


void lite_window_update_rects(struct LiteRect* rects, uint32_t count)
{
    SDL_UpdateWindowSurfaceRects(s_window, (const SDL_Rect*)rects, (int)count);
}


void lite_window_message_box(const char* title, const char* message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, s_window);
}


bool lite_window_confirm_dialog(const char* title, const char* message)
{
    SDL_MessageBoxButtonData buttons[] = {
        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes"},
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No" },
    };

    SDL_MessageBoxData data = {
        .title = title,
        .message = message,
        .numbuttons = 2,
        .buttons = buttons,
    };

    int button_id;
    SDL_ShowMessageBox(&data, &button_id);

    return button_id == 1;
}


static LiteStringView lite_button_name(Uint8 button)
{
    const LiteStringView button_names[] = {
        lite_string_lit("?"),
        lite_string_lit("left"),
        lite_string_lit("middle"),
        lite_string_lit("right"),
        lite_string_lit("x1"),
        lite_string_lit("x2"),
    };

    return button_names[button];
}


static LiteStringView lite_key_name(SDL_Keycode sym)
{
    LiteStringView key_name = lite_string_temp(SDL_GetKeyName(sym));
    char* p = (char*)key_name.buffer;
    while (*p)
    {
        *p = tolower(*p);
        p++;
    }
    return key_name;
}


LiteEvent lite_window_poll_event(void)
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            return (LiteEvent){
                .type = LiteEventType_Quit
            };

        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED
				|| e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                return (LiteEvent){
                    .type = LiteEventType_Resized,
                    .resized = {
                        .width = e.window.data1,
                        .height = e.window.data2,
                    }
                };
            }

            if (e.window.event == SDL_WINDOWEVENT_EXPOSED)
            {
                return (LiteEvent){
                    .type = LiteEventType_Exposed
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
        {
            int mx, my, wx, wy;
            SDL_GetGlobalMouseState(&mx, &my);
            SDL_GetWindowPosition(s_window, &wx, &wy);
            LiteStringView text = lite_string_temp(e.drop.file);
            SDL_free(e.drop.file);
            return (LiteEvent){
                .type = LiteEventType_DropFile,
                .drop_file = {
                    .file_path = text, // @note(maihd): may leak,
                    .x = mx - wx,
                    .y = my - wy
                }
            };
        }

        case SDL_KEYDOWN:
            lite_log_debug("SDL_KEYDOWN: %s\n", lite_key_name(e.key.keysym.sym).buffer);
            return (LiteEvent){
                .type = LiteEventType_KeyDown,
                .key_down = {
                    .key_name = lite_key_name(e.key.keysym.sym)
                }
            };

        case SDL_KEYUP:
            lite_log_debug("SDL_KEYUP: %s\n", lite_key_name(e.key.keysym.sym).buffer);
            return (LiteEvent){
                .type = LiteEventType_KeyUp,
                .key_up = {
                    .key_name = lite_key_name(e.key.keysym.sym)
                }
            };

        case SDL_TEXTINPUT:
            lite_log_debug("SDL_TextInput: %s\n", e.text.text);
            return (LiteEvent){
                .type = LiteEventType_TextInput,
                .text_input = {
                    .text = lite_string_temp(e.text.text)
                }
            };

        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT)
            {
                SDL_CaptureMouse(true);
            }

            return (LiteEvent){
                .type = LiteEventType_MouseDown,
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

            return (LiteEvent){
                .type = LiteEventType_MouseUp,
                .mouse_up = {
                    .button_name = lite_button_name(e.button.button),
                    .x = e.button.x,
                    .y = e.button.y,
                    .clicks = e.button.clicks,
                }
            };

        case SDL_MOUSEMOTION:
            return (LiteEvent){
                .type = LiteEventType_MouseMove,
                .mouse_move = {
                    .x = e.motion.x,
                    .y = e.motion.y,
                    .dx = e.motion.xrel,
                    .dy = e.motion.yrel,
                }
            };

        case SDL_MOUSEWHEEL:
            return (LiteEvent){
                .type = LiteEventType_MouseWheel,
                .mouse_wheel = {
                    .x = e.wheel.x,
                    .y = e.wheel.y,
                }
            };

        default:
            break;
        }
    }

    return (LiteEvent){
        .type = LiteEventType_None
    };
}


bool lite_window_wait_event(uint64_t time_us)
{
    return SDL_WaitEventTimeout(nullptr, (int)(time_us / 1000000));
}

//! EOF

