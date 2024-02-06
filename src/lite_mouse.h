#pragma once

#include <stdint.h>
#include "lite_meta.h"


#define LITE_MOUSE_BUTTON(X) (1 << ((X) - 1))


typedef enum LiteMouseButton __enum_vtype(uint32_t)
{
    LiteMouseButton_None     = 0,
    LiteMouseButton_Left     = 1,
    LiteMouseButton_Middle   = 2,
    LiteMouseButton_Right    = 3,
    LiteMouseButton_X1       = 4,
    LiteMouseButton_X2       = 5,
    LiteMouseButton_COUNT    = 6
} LiteMouseButton;


typedef struct LiteMouseButtonState
{
    int32_t     clicks;
    int32_t     last_x;
    int32_t     last_y;
    uint64_t    last_timestamp;
} LiteMouseButtonState;


uint32_t                lite_get_mouse_state(int32_t* x, int32_t* y);
void                    lite_set_mouse_state(uint32_t flags, int32_t x, int32_t y);
void                    lite_set_mouse_position(int32_t x, int32_t y);

LiteMouseButtonState*   lite_get_mouse_button_state(LiteMouseButton button);

// void        lite_send_mouse_event();

//! EOF
