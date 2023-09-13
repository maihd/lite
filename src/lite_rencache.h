#pragma once

#include "lite_meta.h"
#include "lite_renderer.h"

void lite_rencache_init(void);
void lite_rencache_deinit(void);

void    lite_rencache_show_debug(bool enable);
void    lite_rencache_free_font(LiteFont* font);
void    lite_rencache_set_clip_rect(LiteRect rect);
void    lite_rencache_draw_rect(LiteRect rect, LiteColor color);
int32_t lite_rencache_draw_text(LiteFont* font, const char* text, int32_t x,
                                int32_t y, LiteColor color);

void lite_rencache_invalidate(void);
void lite_rencache_begin_frame(void);
void lite_rencache_end_frame(void);
