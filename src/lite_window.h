#pragma once

#include "lite_meta.h"
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

typedef enum LiteWindowEventType
{
    LiteWindowEventType_None,
    LiteWindowEventType_Quit,

    LiteWindowEventType_Resized,
    LiteWindowEventType_Exposed,
    LiteWindowEventType_DropFile,

    LiteWindowEventType_KeyUp,
    LiteWindowEventType_KeyDown,
    LiteWindowEventType_TextInput,

    LiteWindowEventType_MouseUp,
    LiteWindowEventType_MouseDown,
    LiteWindowEventType_MouseMove,
    LiteWindowEventType_MouseWheel,
} LiteWindowEventType;

typedef struct LiteWindowEvent
{
    LiteWindowEventType type;
    union
    {
        struct
        {
            int32_t width;
            int32_t height;
        } resized;

        struct
        {
            LiteStringView file_path;
            int32_t        x;
            int32_t        y;
        } drop_file;

        struct
        {
            LiteStringView key_name;
        } key_up, key_down;

        struct
        {
            LiteStringView text;
        } text_input;

        struct
        {
            LiteStringView button_name;
            int32_t        x;
            int32_t        y;
            int32_t        clicks;
        } mouse_up, mouse_down;

        struct
        {
            int32_t x;
            int32_t y;
            int32_t dx;
            int32_t dy;
        } mouse_move;

        struct
        {
            int32_t x;
            int32_t y;
        } mouse_wheel;
    };
} LiteWindowEvent;

void lite_sleep(uint64_t ms);
void lite_usleep(uint64_t us);

uint64_t lite_cpu_ticks(void);
uint64_t lite_cpu_frequency(void);

LiteStringView lite_clipboard_get(void);
bool           lite_clipboard_set(LiteStringView text);

void lite_console_open(void);
void lite_console_close(void);

void lite_window_open(void);
void lite_window_close(void);

void* lite_window_handle(void);
void* lite_window_surface(int32_t* width, int32_t* height);

void lite_window_show(void);
void lite_window_hide(void);

void lite_window_set_mode(LiteWindowMode mode);
void lite_window_set_title(const char* title);
void lite_window_set_cursor(LiteCursor cursor);

float lite_window_dpi(void);
bool  lite_window_has_focus(void);

void lite_window_update_rects(struct LiteRect* rects, uint32_t count);

void lite_window_message_box(const char* title, const char* message);
bool lite_window_confirm_dialog(const char* title, const char* message);

LiteWindowEvent lite_window_poll_event(void);
bool            lite_window_wait_event(uint64_t time_us);

//! EOF
