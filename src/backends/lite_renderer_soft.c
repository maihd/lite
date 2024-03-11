#include <assert.h>

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lib/stb/stb_truetype.h"

#include "lite_meta.h"
#include "lite_memory.h"
#include "lite_window.h"
#include "lite_renderer.h"


#define MAX_GLYPHSET 256


struct LiteImage
{
    LiteColor*  pixels;
    int32_t     width, height;
};


typedef struct
{
    LiteImage*          image;
    stbtt_bakedchar     glyphs[256];
} GlyphSet;


struct LiteFont
{
    void*               data;
    stbtt_fontinfo      stbfont;
    float               size;
    int32_t             height;
    GlyphSet*           sets[MAX_GLYPHSET];
};


static LiteImage    g_surface;
static LiteArena*   g_img_arena;
static LiteArena*   g_font_arena;


static struct
{
    int32_t left, top, right, bottom;
} clip;


// @todo: replace with assert
static void* check_alloc(void* ptr)
{
    if (!ptr)
    {
        // @todo: maybe need to show messagebox here, instead of printing
        fprintf(stderr, "Fatal error: memory allocation failed\n");
        exit(-1);
    }

    return ptr;
}


static const char* utf8_to_codepoint(const char* p, uint32_t* dst)
{
    uint32_t res, n;
    switch (*p & 0xf0)
    {
    case 0xf0:
        res = *p & 0x07;
        n   = 3;
        break;

    case 0xe0:
        res = *p & 0x0f;
        n   = 2;
        break;

    case 0xd0:
    case 0xc0:
        res = *p & 0x1f;
        n   = 1;
        break;

    default:
        res = *p;
        n   = 0;
        break;
    }

    while (n--)
    {
        res = (res << 6) | (*(++p) & 0x3f);
    }

    *dst = res;
    return p + 1;
}


void lite_renderer_init(void)
{
    g_surface.pixels  = (LiteColor*)lite_window_surface(
        &g_surface.width, &g_surface.height
    );

    lite_renderer_set_clip_rect((LiteRect){
                                    .x = 0,
                                    .y = 0,
                                    .width = g_surface.width,
                                    .height = g_surface.height
                                });

    g_img_arena = lite_arena_create(1 * 1024 * 1024, 20 * 1024 * 1024, alignof(LiteColor));
    g_font_arena = lite_arena_create(1 * 1024 * 1024, 20 * 1024 * 1024, alignof(GlyphSet));
}


void lite_renderer_deinit(void)
{
    lite_arena_destroy(g_font_arena);
    lite_arena_destroy(g_img_arena);
    g_font_arena = nullptr;
    g_img_arena = nullptr;

    assert(g_img_arena && "Leak arena in renderer");
    assert(g_font_arena && "Leak arena in renderer");
}


void lite_renderer_update_rects(LiteRect* rects, int32_t count)
{
    lite_window_update_rects(rects, (uint32_t)count);

    static bool initial_frame = true;
    if (initial_frame)
    {
        lite_window_show();
        initial_frame = false;
    }
}


void lite_renderer_set_clip_rect(LiteRect rect)
{
    clip.left   = rect.x;
    clip.top    = rect.y;
    clip.right  = rect.x + rect.width;
    clip.bottom = rect.y + rect.height;
}


void lite_renderer_get_size(int32_t* x, int32_t* y)
{
    assert(x);
    assert(y);

    lite_window_surface(x, y);
}


LiteImage* lite_new_image(int32_t width, int32_t height)
{
    assert(width > 0 && height > 0);

    // @todo(maihd): use Arena instead of malloc
    LiteImage* image =
        (LiteImage*)lite_arena_acquire(g_img_arena, sizeof(LiteImage) + width * height * sizeof(LiteColor));
    check_alloc(image);
    image->pixels = (LiteColor*)(image + 1);
    image->width  = width;
    image->height = height;
    return image;
}


void lite_free_image(LiteImage* image)
{
//     free(image);
}


static GlyphSet* load_glyphset(LiteFont* font, int32_t idx)
{
    GlyphSet* set = check_alloc(calloc(1, sizeof(GlyphSet)));

    /* init image */
    int32_t width  = 128;
    int32_t height = 128;

    for (;;)
    {
        LiteArenaTemp temp = lite_arena_begin_temp(g_img_arena);
        set->image = lite_new_image(width, height);

        /* load glyphs */
        float s = stbtt_ScaleForMappingEmToPixels(&font->stbfont, 1) / stbtt_ScaleForPixelHeight(&font->stbfont, 1);
        int32_t res = stbtt_BakeFontBitmap(font->data, 0, font->size * s,
                                           (void*)set->image->pixels, width,
                                           height, idx * 256, 256, set->glyphs);

        /* retry with a larger image buffer if the buffer wasn't large enough */
        if (res < 0)
        {
            width *= 2;
            height *= 2;

            lite_arena_end_temp(temp);
            continue;
        }

        break;
    }

    /* adjust glyph yoffsets and xadvance */
    int32_t ascent, descent, linegap;
    stbtt_GetFontVMetrics(&font->stbfont, &ascent, &descent, &linegap);
    float   scale = stbtt_ScaleForMappingEmToPixels(&font->stbfont, font->size);
    int32_t scaled_ascent = (int32_t)(ascent * scale + 0.5f);
    for (int32_t i = 0; i < 256; i++)
    {
        set->glyphs[i].yoff += scaled_ascent;
        set->glyphs[i].xadvance = floorf(set->glyphs[i].xadvance);
    }

    // convert 8bit data to 32bit
    // @todo: why must be pre-convert?
    // @todo: why must be in reverted-order loop?
    for (int32_t i = width * height - 1; i >= 0; i--)
    {
        uint8_t n = ((uint8_t*)set->image->pixels)[i];
        set->image->pixels[i] =
            (LiteColor){.r = 255, .g = 255, .b = 255, .a = n};
    }

    return set;
}


static GlyphSet* get_glyphset(LiteFont* font, int32_t codepoint)
{
    int32_t idx = (codepoint >> 8) % MAX_GLYPHSET;
    if (font->sets[idx] == NULL)
    {
        font->sets[idx] = load_glyphset(font, idx);
    }

    return font->sets[idx];
}


LiteFont* lite_load_font(const char* filename, float size)
{
    LiteArenaTemp arena_temp = lite_arena_begin_temp(g_font_arena);

    LiteFont* font = nullptr;
    FILE*    fp   = nullptr;

    /* init font */
    font       = check_alloc(lite_arena_acquire(g_font_arena, sizeof(LiteFont)));
    memset(font, 0, sizeof(*font));
    font->size = size;

    /* load font into buffer */
    fp = fopen(filename, "rb");
    if (!fp)
    {
        return nullptr;
    }

    /* get size */
    fseek(fp, 0, SEEK_END);
    int32_t buf_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    /* load */
    font->data = check_alloc(lite_arena_acquire(g_font_arena, buf_size));
    size_t  _  = fread(font->data, 1, buf_size, fp);
    (void)_;
    fclose(fp);
    fp = nullptr;

    /* init stbfont */
    int32_t ok = stbtt_InitFont(&font->stbfont, font->data, 0);
    if (!ok)
    {
        lite_arena_end_temp(arena_temp);
        return nullptr;
    }

    /* get height and scale */
    int32_t ascent, descent, linegap;
    stbtt_GetFontVMetrics(&font->stbfont, &ascent, &descent, &linegap);
    float scale  = stbtt_ScaleForMappingEmToPixels(&font->stbfont, size);
    font->height = (int32_t)((ascent - descent + linegap) * scale + 0.5f);

    /* make tab and newline glyphs invisible */
    stbtt_bakedchar* g = get_glyphset(font, '\n')->glyphs;
    g['\t'].x1         = g['\t'].x0;
    g['\n'].x1         = g['\n'].x0;

    return font;
}


void lite_free_font(LiteFont* font)
{
    for (int32_t i = 0; i < MAX_GLYPHSET; i++)
    {
        GlyphSet* set = font->sets[i];
        if (set)
        {
            lite_free_image(set->image);
            free(set);
        }
    }
    free(font->data);
    free(font);
}


void lite_set_font_tab_width(LiteFont* font, int32_t n)
{
    GlyphSet* set              = get_glyphset(font, '\t');
    set->glyphs['\t'].xadvance = (float)n;
}


int32_t lite_get_font_tab_width(LiteFont* font)
{
    GlyphSet* set = get_glyphset(font, '\t');
    return (int32_t)set->glyphs['\t'].xadvance;
}


int32_t lite_get_font_width(LiteFont* font, const char* text)
{
    int32_t     x = 0;
    const char* p = text;
    unsigned    codepoint;
    while (*p)
    {
        p                    = utf8_to_codepoint(p, &codepoint);
        GlyphSet*        set = get_glyphset(font, codepoint);
        stbtt_bakedchar* g   = &set->glyphs[codepoint & 0xff];

        x += (int32_t)g->xadvance;
    }
    return x;
}


int32_t lite_get_font_height(LiteFont* font)
{
    return font->height;
}


static inline LiteColor blend_pixel(LiteColor dst, LiteColor src)
{
    int32_t ia = 0xff - src.a;
    dst.r      = ((src.r * src.a) + (dst.r * ia)) >> 8;
    dst.g      = ((src.g * src.a) + (dst.g * ia)) >> 8;
    dst.b      = ((src.b * src.a) + (dst.b * ia)) >> 8;
    return dst;
}


static inline LiteColor blend_pixel2(LiteColor dst, LiteColor src, LiteColor color)
{
    src.a      = (src.a * color.a) >> 8;
    uint8_t ia = 0xff - src.a;
    dst.r      = ((src.r * color.r * src.a) >> 16) + ((dst.r * ia) >> 8);
    dst.g      = ((src.g * color.g * src.a) >> 16) + ((dst.g * ia) >> 8);
    dst.b      = ((src.b * color.b * src.a) >> 16) + ((dst.b * ia) >> 8);
    return dst;
}


#define rect_draw_loop(expr)                                                   \
    for (int32_t j = y1; j < y2; j++)                                          \
    {                                                                          \
        for (int32_t i = x1; i < x2; i++)                                      \
        {                                                                      \
            *d = expr;                                                         \
            d++;                                                               \
        }                                                                      \
        d += dr;                                                               \
    }


void lite_draw_rect(LiteRect rect, LiteColor color)
{
    if (color.a == 0)
    {
        return;
    }

    int32_t x1 = rect.x < clip.left ? clip.left : rect.x;
    int32_t y1 = rect.y < clip.top ? clip.top : rect.y;
    int32_t x2 = rect.x + rect.width;
    int32_t y2 = rect.y + rect.height;
    x2         = x2 > clip.right ? clip.right : x2;
    y2         = y2 > clip.bottom ? clip.bottom : y2;

    // @note(maihd): trick, need to handle resize event instead
    g_surface.pixels  = (LiteColor*)lite_window_surface(
        &g_surface.width, &g_surface.height
    );

    LiteColor* d = g_surface.pixels;
    d += x1 + y1 * g_surface.width;
    int32_t dr = g_surface.width - (x2 - x1);

    if (color.a == 0xff)
    {
        rect_draw_loop(color);
    }
    else
    {
        rect_draw_loop(blend_pixel(*d, color));
    }
}


void lite_draw_image(LiteImage* image, LiteRect* sub, int32_t x, int32_t y, LiteColor color)
{
    if (color.a == 0)
    {
        return;
    }

    /* clip */
    int32_t n;
    if ((n = clip.left - x) > 0)
    {
        sub->width -= n;
        sub->x += n;
        x += n;
    }
    if ((n = clip.top - y) > 0)
    {
        sub->height -= n;
        sub->y += n;
        y += n;
    }
    if ((n = x + sub->width - clip.right) > 0)
    {
        sub->width -= n;
    }
    if ((n = y + sub->height - clip.bottom) > 0)
    {
        sub->height -= n;
    }

    if (sub->width <= 0 || sub->height <= 0)
    {
        return;
    }

    // @note(maihd): trick, need to handle resize event instead
    g_surface.pixels  = (LiteColor*)lite_window_surface(
        &g_surface.width, &g_surface.height
    );

    /* draw */
    LiteColor*    s    = image->pixels;
    LiteColor*    d    = g_surface.pixels;
    s += sub->x + sub->y * image->width;
    d += x + y * g_surface.width;
    int32_t sr = image->width - sub->width;
    int32_t dr = g_surface.width - sub->width;

    for (int32_t j = 0; j < sub->height; j++)
    {
        for (int32_t i = 0; i < sub->width; i++)
        {
            *d = blend_pixel2(*d, *s, color);
            d++;
            s++;
        }
        d += dr;
        s += sr;
    }
}


int32_t lite_draw_text(LiteFont* font, const char* text, int32_t x, int32_t y, LiteColor color)
{
    LiteRect rect;
    const char* p = text;
    uint32_t    codepoint;
    while (*p)
    {
        p                    = utf8_to_codepoint(p, &codepoint);
        GlyphSet*        set = get_glyphset(font, codepoint);
        stbtt_bakedchar* g   = &set->glyphs[codepoint & 0xff];
        rect.x               = g->x0;
        rect.y               = g->y0;
        rect.width           = g->x1 - g->x0;
        rect.height          = g->y1 - g->y0;
        lite_draw_image(set->image, &rect, x + (int32_t)g->xoff, y + (int32_t)g->yoff, color);

        x += (int32_t)g->xadvance;
    }
    return x;
}

//! Leave an empty newline here, required by GCC
