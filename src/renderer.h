#pragma once

#include "meta.h"

typedef struct RenImage RenImage;
typedef struct RenFont  RenFont;

typedef struct RenColor
{
    uint8_t b, g, r, a;
} RenColor;

typedef struct RenRect
{
    int32_t x, y, width, height;
} RenRect;

void lite_renderer_init(void* win_handle);
void lite_renderer_deinit(void);

void lite_renderer_update_rects(RenRect* rects, int32_t count);
void lite_renderer_set_clip_rect(RenRect rect);
void lite_renderer_get_size(int32_t* x, int32_t* y);

RenImage* lite_new_image(int32_t width, int32_t height);
void      lite_free_image(RenImage* image);

RenFont* lite_load_font(const char* filename, float size);
void     lite_free_font(RenFont* font);
void     lite_set_font_tab_width(RenFont* font, int32_t n);
int      lite_get_font_tab_width(RenFont* font);
int      lite_get_font_width(RenFont* font, const char* text);
int      lite_get_font_height(RenFont* font);

void lite_draw_rect(RenRect rect, RenColor color);
void lite_draw_image(RenImage* image, RenRect* sub, int32_t x, int32_t y,
                     RenColor color);
int  lite_draw_text(RenFont* font, const char* text, int32_t x, int32_t y,
                    RenColor color);
