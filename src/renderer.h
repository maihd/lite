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

void ren_init(void* win_handle);
void ren_deinit(void);

void ren_update_rects(RenRect* rects, int32_t count);
void ren_set_clip_rect(RenRect rect);
void ren_get_size(int32_t* x, int32_t* y);

RenImage* ren_new_image(int32_t width, int32_t height);
void      ren_free_image(RenImage* image);

RenFont* ren_load_font(const char* filename, float size);
void     ren_free_font(RenFont* font);
void     ren_set_font_tab_width(RenFont* font, int32_t n);
int      ren_get_font_tab_width(RenFont* font);
int      ren_get_font_width(RenFont* font, const char* text);
int      ren_get_font_height(RenFont* font);

void ren_draw_rect(RenRect rect, RenColor color);
void ren_draw_image(RenImage* image, RenRect* sub, int32_t x, int32_t y,
                    RenColor color);
int  ren_draw_text(RenFont* font, const char* text, int32_t x, int32_t y,
                   RenColor color);
