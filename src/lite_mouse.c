#include "lite_mouse.h"

static int32_t                  s_mouse_x;
static int32_t                  s_mouse_y;
static uint32_t                 s_mouse_flags;
static LiteMouseButtonState     s_mouse_states[LiteMouseButton_COUNT];


uint32_t lite_get_mouse_state(int32_t* x, int32_t* y)
{
    if (x) *x = s_mouse_x;
    if (y) *y = s_mouse_y;

    return s_mouse_flags;
}


void lite_set_mouse_state(uint32_t flags, int32_t x, int32_t y)
{
    s_mouse_x = x;
    s_mouse_y = y;
    s_mouse_flags = flags;
}


void lite_set_mouse_position(int32_t x, int32_t y)
{
    s_mouse_x = x;
    s_mouse_y = y;
}


LiteMouseButtonState* lite_get_mouse_button_state(LiteMouseButton button)
{
    return &s_mouse_states[(int32_t)button];
}

//! EOF

