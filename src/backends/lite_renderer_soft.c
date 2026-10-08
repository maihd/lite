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


enum { MAX_NUM_GLYPHSET = 256, NUM_GLYPHSET_CHARS = 256 };


struct LiteImage
{
    LiteColor*          pixels;
    int32_t             width, height;
};


typedef struct LiteGlyphSet
{
    LiteImage*          image;
    stbtt_packedchar    glyphs[NUM_GLYPHSET_CHARS];
} LiteGlyphSet;


struct LiteFont
{
    void*               data;
    stbtt_fontinfo      stbfont;

    float               size;
    int32_t             height;
    int32_t             sample;

    bool                is_monospace;
    float               monospace_width;

    LiteGlyphSet*       sets[MAX_NUM_GLYPHSET];
};


static LiteImage        g_surface;
static LiteArena*       g_img_arena;
static LiteArena*       g_font_arena;


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


static LiteStringView utf8_to_codepoint(LiteStringView p, uint32_t* dst)
{
    assert(p.buffer != nullptr);
    assert(p.length > 0);

    uint32_t res, n;
    switch (*p.buffer & 0xf0)
    {
    case 0xf0:
        res = *p.buffer & 0x07;
        n   = 3;
        break;

    case 0xe0:
        res = *p.buffer & 0x0f;
        n   = 2;
        break;

    case 0xd0:
    case 0xc0:
        res = *p.buffer & 0x1f;
        n   = 1;
        break;

    default:
        res = *p.buffer;
        n   = 0;
        break;
    }

    while (n--)
    {
        p.buffer += 1;
        p.length -= 1;

        res = (res << 6) | (*p.buffer & 0x3f);
    }

    *dst = res;

    p.buffer += 1;
    p.length -= 1;
    return p;
}


void lite_renderer_init(void)
{
    g_surface.pixels = (LiteColor*)lite_window_surface(
        &g_surface.width, &g_surface.height
    );

    lite_renderer_set_clip_rect(
        (LiteRect){
            .x      = 0,
            .y      = 0,
            .width  = g_surface.width,
            .height = g_surface.height
        }
    );

    g_img_arena = lite_arena_create(10 * 1024 * 1024, 100 * 1024 * 1024, alignof(LiteColor));
    g_font_arena = lite_arena_create(1 * 1024 * 1024, 20 * 1024 * 1024, alignof(LiteGlyphSet));
}


void lite_renderer_deinit(void)
{
    lite_arena_destroy(g_font_arena);
    lite_arena_destroy(g_img_arena);
    g_font_arena = nullptr;
    g_img_arena = nullptr;

    assert(g_img_arena == nullptr && "Leak arena in renderer");
    assert(g_font_arena == nullptr && "Leak arena in renderer");
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

    size_t      image_size  = sizeof(LiteImage) + width * height * sizeof(LiteColor);
    LiteImage*  image       = (LiteImage*)lite_arena_acquire(g_img_arena, image_size);
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


static LiteGlyphSet* load_glyphset(LiteFont* font, int32_t idx)
{
    LiteGlyphSet* set = check_alloc(calloc(1, sizeof(LiteGlyphSet)));

    // init image
    int32_t width  = 128;
    int32_t height = 128;

    for (;;)
    {
        LiteArenaTemp temp = lite_arena_begin_temp(g_img_arena);

        set->image = lite_new_image(width, height);

        // load glyphs
        float scale = stbtt_ScaleForMappingEmToPixels(&font->stbfont, 1) / stbtt_ScaleForPixelHeight(&font->stbfont, 1);
        // float scale = stbtt_ScaleForPixelHeight(&font->stbfont, 1);

        int32_t first_char  = idx * NUM_GLYPHSET_CHARS;
        int32_t num_chars   = NUM_GLYPHSET_CHARS;

        stbtt_pack_context pack_context;
        stbtt_PackBegin(&pack_context, (unsigned char*)set->image->pixels, set->image->width, set->image->height, 0, 4, nullptr);
        stbtt_PackSetOversampling(&pack_context, font->sample, font->sample);
        int32_t res = stbtt_PackFontRange(&pack_context, font->data, 0, font->size * scale, first_char, num_chars, set->glyphs);
        stbtt_PackEnd(&pack_context);

        /* retry with a larger image buffer if the buffer wasn't large enough */
        if (res == 0)
        {
            width   *= 2;
            height  *= 2;

            lite_arena_end_temp(temp);
            continue;
        }

        break;
    }

    /* adjust glyph yoffsets and xadvance */
    int32_t ascent, descent, linegap;
    stbtt_GetFontVMetrics(&font->stbfont, &ascent, &descent, &linegap);

    float   scale           = stbtt_ScaleForMappingEmToPixels(&font->stbfont, font->size);
    int32_t scaled_ascent   = (int32_t)(ascent * scale + 0.5f);

    for (int32_t i = 0; i < NUM_GLYPHSET_CHARS; i++)
    {
        set->glyphs[i].xadvance = ceilf(set->glyphs[i].xadvance);

        set->glyphs[i].xoff     = ceilf(set->glyphs[i].xoff);
        set->glyphs[i].xoff2    = ceilf(set->glyphs[i].xoff2);

        set->glyphs[i].yoff     = floorf(set->glyphs[i].yoff + scaled_ascent);
        set->glyphs[i].yoff2    = floorf(set->glyphs[i].yoff2 + scaled_ascent + 0.5f);
    }

    // if (font->is_monospace)
    // {
    //     for (int32_t i = 0; i < NUM_GLYPHSET_CHARS; i++)
    //     {
    //         stbtt_packedchar* g = &set->glyphs[i];
    //         int32_t width = g->x1 - g->x0;
    //         if (width % 2 == 1)
    //         {
    //             // quick_exit(0);
    //             g->x0 -= 1;
    //             // g->x1 += 2;
    //         }
    //     }
    // }

    // convert 8bit data to 32bit
    // @note(maihd): why must be pre-convert? -> stb_truetype bitmap only contains alpha
    // @note(maihd): why must be in reverted-order loop? -> this will help make sure the pixels data is not overlapped
    for (int32_t i = width * height - 1; i >= 0; i--)
    {
        uint8_t n = ((uint8_t*)set->image->pixels)[i];
        set->image->pixels[i] = (LiteColor){.r = 255, .g = 255, .b = 255, .a = n};
    }

    return set;
}

static stbtt_packedchar* get_glyph(LiteFont* font, int32_t codepoint);

static LiteGlyphSet* get_glyphset(LiteFont* font, int32_t codepoint)
{
    int32_t idx = (codepoint >> 8) % MAX_NUM_GLYPHSET;
    if (font->sets[idx] == NULL)
    {
        LiteGlyphSet* set = load_glyphset(font, idx);
        font->sets[idx] = set;
    }

    return font->sets[idx];
}


static stbtt_packedchar* get_glyph(LiteFont* font, int32_t codepoint)
{
    return &get_glyphset(font, codepoint)->glyphs[codepoint];
}


LiteFont* lite_load_font(LiteStringView filename, float size)
{
    LiteArenaTemp arena_temp = lite_arena_begin_temp(g_font_arena);

    // init font
    LiteFont* font = check_alloc(lite_arena_acquire(g_font_arena, sizeof(LiteFont)));
    memset(font, 0, sizeof(*font));

    font->size      = size;
    font->sample    = 1;

    // @todo(maihd): use better IO operations
    // load font into buffer
    FILE* fp = fopen(filename.buffer, "rb");
    if (!fp)
    {
        return nullptr;
    }

    // get size
    fseek(fp, 0, SEEK_END);
    int32_t buf_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // load
    font->data = check_alloc(lite_arena_acquire(g_font_arena, buf_size));
    size_t  _  = fread(font->data, 1, buf_size, fp);
    (void)_;
    fclose(fp);
    fp = nullptr;

    // init stbfont
    int32_t ok = stbtt_InitFont(&font->stbfont, font->data, 0);
    if (!ok)
    {
        lite_arena_end_temp(arena_temp);
        return nullptr;
    }

    // get height and scale
    int32_t ascent, descent, linegap;
    stbtt_GetFontVMetrics(&font->stbfont, &ascent, &descent, &linegap);

    float scale  = stbtt_ScaleForMappingEmToPixels(&font->stbfont, size);
    font->height = (int32_t)((ascent - descent + linegap) * scale + 0.5f);

    // Check mono font and set uniform advance for all chars
    int unscaled_mono_advance, lsb;
    stbtt_GetCodepointHMetrics(&font->stbfont, 'M', &unscaled_mono_advance, &lsb);

    bool is_monospace = true;
    for (int i = 0; i < NUM_GLYPHSET_CHARS; i += 1)
    {
        int xadvance, _;
        stbtt_GetCodepointHMetrics(&font->stbfont, i, &xadvance, &_);
        if (xadvance == unscaled_mono_advance)
        {
            is_monospace = false;
            break;
        }
    }

    font->is_monospace = is_monospace;
    if (is_monospace)
    {
        font->monospace_width = floorf((float)unscaled_mono_advance * stbtt_ScaleForMappingEmToPixels(&font->stbfont, size));
        if ((int)(font->monospace_width) % 2 == 1)
        {
            font->monospace_width -= 1;
        }
    }

    // make tab and newline glyphs invisible
    stbtt_packedchar* g = get_glyphset(font, '\n')->glyphs;
    g['\t'].x1 = g['\t'].x0;
    g['\n'].x1 = g['\n'].x0;
    g['\r'].x1 = g['\r'].x0;

    // center operators (just -> now)
    // ASCII offsets: '-' is 45, '>' is 62 (Adjust if using a custom unicode range array)
    bool center_hyphen = false;
    if (center_hyphen)
    {
        stbtt_packedchar* hyphen = get_glyph(font, '-');
        stbtt_packedchar* gt     = get_glyph(font, '>');

        float gt_height = gt->yoff2 - gt->yoff;
        float gt_center = gt->yoff + (gt_height / 2.0f);

        float hyphen_height = hyphen->yoff2 - hyphen->yoff;
        hyphen->yoff  = gt_center - (hyphen_height / 2.0f);
        hyphen->yoff2 = gt_center + (hyphen_height / 2.0f);
    }

    // @note(maihd): tricks to fix the 'k' padding
    if (is_monospace) 
    {
        stbtt_packedchar* k = get_glyph(font, 'k');
        k->x0 -= 1;
        k->x1 -= 1;
    }

    return font;
}


void lite_free_font(LiteFont* font)
{
    for (int32_t i = 0; i < MAX_NUM_GLYPHSET; i++)
    {
        LiteGlyphSet* set = font->sets[i];
        if (set)
        {
            lite_free_image(set->image);
            free(set);
        }
    }

    // @note(maihd): font now use LiteArena, so there no need to free
    //free(font->data);
    //free(font);
}


void lite_set_font_tab_width(LiteFont* font, int32_t n)
{
    LiteGlyphSet* set = get_glyphset(font, '\t');
    set->glyphs['\t'].xadvance = (float)n;
}


int32_t lite_get_font_tab_width(LiteFont* font)
{
    LiteGlyphSet* set = get_glyphset(font, '\t');
    return (int32_t)set->glyphs['\t'].xadvance;
}


int32_t lite_get_font_width(LiteFont* font, LiteStringView text)
{
    int32_t         x = 0;
    LiteStringView  p = text;
    unsigned        codepoint;
    while (p.length > 0)
    {
        p                       = utf8_to_codepoint(p, &codepoint);
        LiteGlyphSet*     set   = get_glyphset(font, codepoint);
        stbtt_packedchar* g     = &set->glyphs[codepoint & 0xff];

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


void lite_draw_rect(LiteRect rect, LiteColor color)
{
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
    g_surface.pixels = (LiteColor*)lite_window_surface(
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

#undef rect_draw_loop
}


void lite_draw_image(LiteImage* image, LiteRect sub, int32_t x, int32_t y, LiteColor color)
{
    if (color.a == 0)
    {
        return;
    }

    /* clip */
    int32_t n;
    if ((n = clip.left - x) > 0)
    {
        sub.width -= n;
        sub.x += n;
        x += n;
    }
    if ((n = clip.top - y) > 0)
    {
        sub.height -= n;
        sub.y += n;
        y += n;
    }
    if ((n = x + sub.width - clip.right) > 0)
    {
        sub.width -= n;
    }
    if ((n = y + sub.height - clip.bottom) > 0)
    {
        sub.height -= n;
    }

    if (sub.width <= 0 || sub.height <= 0)
    {
        return;
    }

    // @note(maihd): trick, need to handle resize event instead
    g_surface.pixels = (LiteColor*)lite_window_surface(
        &g_surface.width, &g_surface.height
    );

    // draw (by copy pixels from image to screen)
    LiteColor* s = image->pixels;
    LiteColor* d = g_surface.pixels;

    int32_t sr = image->width;
    int32_t dr = g_surface.width;

    for (int32_t j = 0; j < sub.height; j++)
    {
        int32_t dst_y = y + j;
        int32_t src_y = sub.y + j;

        for (int32_t i = 0; i < sub.width; i++)
        {
            int32_t dst_x = x + i;
            int32_t src_x = sub.x + i;

            int32_t dst_idx = dst_y * dr + dst_x;
            int32_t src_idx = src_y * sr + src_x;

            LiteColor color1 = d[dst_idx];
            LiteColor color2 = s[src_idx];

            d[dst_idx] = blend_pixel2(color1, color2, color);
        }
    }
}


void lite_draw_image_subpixel(LiteImage* image, LiteRect src, LiteRect dst, LiteColor color)
{
    if (color.a == 0)
    {
        return;
    }

    float scale_x = (float)src.width / (float)dst.width;
    float scale_y = (float)src.height / (float)dst.height;

    // clip src rect
    int32_t n;
    if ((n = clip.left - dst.x) > 0)
    {
        src.width -= (int32_t)(n * scale_x);
        dst.width -= n;

        src.x += n;
        dst.x += (n * scale_x);
    }
    if ((n = clip.top - dst.y) > 0)
    {
        src.height -= (int32_t)(n * scale_y);
        dst.height -= n;

        src.y += (int32_t)(n * scale_y);
        dst.y += n;
    }
    if ((n = dst.x + dst.width - clip.right) > 0)
    {
        src.width -= (int32_t)(n * scale_x);
        dst.width -= n;
    }
    if ((n = dst.y + dst.height - clip.bottom) > 0)
    {
        src.height -= (int32_t)(n * scale_y);
        dst.height -= n;
    }

    if (src.width <= 0 || src.height <= 0 || dst.width <= 0 || dst.height <= 0)
    {
        return;
    }

    // @note(maihd): trick, need to handle resize event instead
    g_surface.pixels = (LiteColor*)lite_window_surface(
        &g_surface.width, &g_surface.height
    );

    // draw (by copy pixels from image to screen)
    LiteColor* s = image->pixels;
    LiteColor* d = g_surface.pixels;

    int32_t sr = image->width;
    int32_t dr = g_surface.width;

    for (int32_t j = 0; j < dst.height; j++)
    {
        int32_t dst_y = dst.y + j;
        int32_t src_y = src.y + (j * scale_y);

        for (int32_t i = 0; i < dst.width; i++)
        {
            int32_t dst_x = dst.x + i;
            int32_t src_x = src.x + (i * scale_x);

            int32_t dst_idx = dst_y * dr + dst_x;
            int32_t src_idx = src_y * sr + src_x;

            LiteColor color1 = d[dst_idx];
            LiteColor color2 = s[src_idx];

            d[dst_idx] = blend_pixel2(color1, color2, color);
        }
    }
}


int32_t lite_draw_text(LiteFont* font, LiteStringView text, int32_t x, int32_t y, LiteColor color)
{
    if (font->is_monospace)
    {
        LiteStringView p = text;
        while (p.length > 0)
        {
            uint32_t codepoint;
            p = utf8_to_codepoint(p, &codepoint);

            LiteGlyphSet*       set = get_glyphset(font, codepoint);
            stbtt_packedchar*   g   = &set->glyphs[codepoint & 0xff];

            LiteRect src = {
                .x      = g->x0,
                .y      = g->y0,
                .width  = g->x1 - g->x0,
                .height = g->y1 - g->y0,
            };

            // Calculate target rectangle on screen using xoff2 and yoff2
            LiteRect dst = {
                .x      = (int32_t)(x + (font->monospace_width - (float)src.width) * 0.5f),
                .y      = y + (int32_t)g->yoff,
                .width  = src.width / font->sample,     // Accurate subpixel width
                .height = src.height / font->sample,    // Accurate subpixel height
            };

            lite_draw_image(set->image, src, dst.x, dst.y, color);
            // lite_draw_image_subpixel(set->image, src, dst, color); // Software rendering does not support AA

            x += (int32_t)font->monospace_width;
        }

        return x;
    }

    LiteStringView p = text;
    while (p.length > 0)
    {
        uint32_t codepoint;
        p = utf8_to_codepoint(p, &codepoint);

        LiteGlyphSet*       set = get_glyphset(font, codepoint);
        stbtt_packedchar*   g   = &set->glyphs[codepoint & 0xff];

        LiteRect src = {
            .x      = g->x0,
            .y      = g->y0,
            .width  = g->x1 - g->x0,
            .height = g->y1 - g->y0,
        };

        // Calculate target rectangle on screen using xoff2 and yoff2
        LiteRect dst = {
            .x      = x + (int32_t)g->xoff,
            .y      = y + (int32_t)g->yoff,
            .width  = src.width / font->sample,     // Accurate subpixel width
            .height = src.height / font->sample,    // Accurate subpixel height
        };

        lite_draw_image(set->image, src, dst.x, dst.y, color);
        // lite_draw_image_subpixel(set->image, src, dst, color); // Software rendering does not support AA

        x += (int32_t)g->xadvance;
    }

    return x;
}

//! Leave an empty newline here, required by GCC
