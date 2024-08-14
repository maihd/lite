#pragma once

#include "lite_meta.h"
#include "lite_event.h"
#include "lite_string.h"

struct LiteRect;

typedef enum LiteCursor
{
    LiteCursor_None,
    LiteCursor_Hand,
    LiteCursor_Arrow,
    LiteCursor_Ibeam,
    LiteCursor_SizeH,
    LiteCursor_SizeV,
    LiteCursor_COUNT
} LiteCursor;


typedef enum LiteWindowMode
{
    LiteWindowMode_Normal,
    LiteWindowMode_Maximized,
    LiteWindowMode_FullScreen,
} LiteWindowMode;


void            lite_sleep(uint64_t ms);
void            lite_usleep(uint64_t us);

uint64_t        lite_cpu_ticks(void);
uint64_t        lite_cpu_frequency(void);

LiteStringView  lite_clipboard_get(void);
bool            lite_clipboard_set(LiteStringView text);

void            lite_console_open(void);
void            lite_console_close(void);

void            lite_window_open(void);
void            lite_window_close(void);

void*           lite_window_handle(void);
void*           lite_window_surface(int32_t* width, int32_t* height);

void            lite_window_show(void);
void            lite_window_hide(void);

void            lite_window_show_titlebar(void);
void            lite_window_hide_titlebar(void);

void            lite_window_set_position(int32_t x, int32_t y);
void            lite_window_get_position(int32_t* x, int32_t* y);

void            lite_window_minimize(void);
void            lite_window_maximize(void);
void            lite_window_toggle_maximize(void);
void            lite_window_restore_maximize(void);

void            lite_window_set_mode(LiteWindowMode mode);
void            lite_window_set_title(const char* title);
void            lite_window_set_cursor(LiteCursor cursor);

void            lite_window_get_mouse_position(int32_t* x, int32_t* y);
void            lite_window_get_global_mouse_position(int32_t* x, int32_t* y);

float           lite_window_get_opacity(void);                  // Opacity in range [0.0f, 1.0f]
void            lite_window_set_opacity(float opacity);         // Opacity in range [0.0f, 1.0f]

void            lite_window_get_size(int32_t* width, int32_t* height);

float           lite_window_dpi(void);
bool            lite_window_has_focus(void);

void            lite_window_update_rects(struct LiteRect* rects, uint32_t count);

void            lite_window_message_box(const char* title, const char* message);
bool            lite_window_confirm_dialog(const char* title, const char* message);

LiteEvent       lite_window_poll_event(void);
bool            lite_window_wait_event(uint64_t time_us);

//! EOF
