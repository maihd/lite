#pragma once

#include <stdint.h>
#include <lite_string.h>

typedef enum LiteEventType
{
    LiteEventType_None,
    LiteEventType_Quit,

    LiteEventType_Resized,
    LiteEventType_Exposed,
    LiteEventType_DropFile,

    LiteEventType_KeyUp,
    LiteEventType_KeyDown,
    LiteEventType_TextInput,

    LiteEventType_MouseUp,
    LiteEventType_MouseDown,
    LiteEventType_MouseMove,
    LiteEventType_MouseWheel,
} LiteEventType;


typedef enum LiteEventAction
{
    LiteEventAction_None,
    LiteEventAction_Pressed,
    LiteEventAction_Released
} LiteEventAction;


typedef struct LiteEvent
{
    LiteEventType type;
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
} LiteEvent;



//! EOF
