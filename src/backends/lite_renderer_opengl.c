#include <stdio.h>

#include "renderer.h"

// typedef struct RenImage RenImage;
// typedef struct LiteFont  LiteFont;

// typedef struct LiteColor
// {
//     uint8_t b, g, r, a;
// } LiteColor;

// typedef struct LiteRect
// {
//     int x, y, width, height;
// } LiteRect;

void ren_init(void* win_handle);
void ren_update_rects(LiteRect* rects, int count);
void ren_set_clip_rect(LiteRect rect);
void ren_get_size(int* x, int* y);

RenImage* ren_new_image(int width, int height);
void      ren_free_image(RenImage* image);

LiteFont* ren_load_font(const char* filename, float size);
void     ren_free_font(LiteFont* font);
void     ren_set_font_tab_width(LiteFont* font, int n);
int      ren_get_font_tab_width(LiteFont* font);
int      ren_get_font_width(LiteFont* font, const char* text);
int      ren_get_font_height(LiteFont* font);

void ren_draw_rect(LiteRect rect, LiteColor color);
void ren_draw_image(RenImage* image, LiteRect* sub, int x, int y,
                    LiteColor color);
int  ren_draw_text(LiteFont* font, const char* text, int x, int y,
                   LiteColor color);
