#include "meta.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "lib/stb/stb_truetype.h"
#include <SDL2/SDL.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "renderer.h"

#define MAX_GLYPHSET 256

struct RenImage
{
    RenColor* pixels;
    int32_t   width, height;
};

typedef struct
{
    RenImage*       image;
    stbtt_bakedchar glyphs[256];
} GlyphSet;

struct RenFont
{
    void*          data;
    stbtt_fontinfo stbfont;
    GlyphSet*      sets[MAX_GLYPHSET];
    float          size;
    int32_t        height;
};

static SDL_Window* window;
static RenImage g_surface;

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

void lite_renderer_init(void* win_handle)
{
    assert(win);
    
    window            = (SDL_Window*)win_handle;
    SDL_Surface* surf = SDL_GetWindowSurface(window);
    g_surface = (RenImage){
        .width = (int32_t)surf->w,
        .height = (int32_t)surf->h,
        .pixels = (RenColor*)surf->pixels
    };
    
    lite_renderer_set_clip_rect((RenRect){
                                    .x = 0,
                                    .y = 0,
                                    .width = (int32_t)surf->w,
                                    .height = (int32_t)surf->h
                                });
}

void lite_renderer_deinit(void)
{
    if (window != nullptr)
    {
        // @todo: delete surface here
    }
}

#if 0
// @note(maihd): copy from SDL2 src
struct SDL_Window
{
    const void *magic;
    SDL_WindowID id;
    char *title;
    SDL_Surface *icon;
    int x, y;
    int w, h;
    int min_w, min_h;
    int max_w, max_h;
    int last_pixel_w, last_pixel_h;
    Uint32 flags;
    Uint32 pending_flags;
    float display_scale;
    SDL_bool fullscreen_exclusive;  /* The window is currently fullscreen exclusive */
    SDL_DisplayID last_fullscreen_exclusive_display;  /* The last fullscreen_exclusive display */
    SDL_DisplayID last_displayID;
    
    /* Stored position and size for windowed mode */
    SDL_Rect windowed;
    
    /* Whether or not the intial position was defined */
    SDL_bool undefined_x;
    SDL_bool undefined_y;
    
    SDL_DisplayMode requested_fullscreen_mode;
    SDL_DisplayMode current_fullscreen_mode;
    
    float opacity;
    
    SDL_Surface *surface;
    SDL_bool surface_valid;
    
    SDL_bool is_hiding;
    SDL_bool restore_on_show; /* Child was hidden recursively by the parent, restore when shown. */
    SDL_bool is_destroying;
    SDL_bool is_dropping; /* drag/drop in progress, expecting SDL_SendDropComplete(). */
    
    SDL_Rect mouse_rect;
    
    SDL_WindowShaper *shaper;
    
    SDL_HitTest hit_test;
    void *hit_test_data;
    
    SDL_WindowUserData *data;
    
    SDL_WindowData *driverdata;
    
    SDL_Window *prev;
    SDL_Window *next;
    
    SDL_Window *parent;
    SDL_Window *first_child;
    SDL_Window *prev_sibling;
    SDL_Window *next_sibling;
};

// @note(maihd): copy from SDL2 src
struct SDL_WindowData
{
    SDL_Window *window;
    HWND hwnd;
    HWND parent;
    HDC hdc;
    HDC mdc;
#if 0
    HINSTANCE hinstance;
    HBITMAP hbm;
    WNDPROC wndproc;
    HHOOK keyboard_hook;
    SDL_bool created;
    WPARAM mouse_button_flags;
    LPARAM last_pointer_update;
    WCHAR high_surrogate;
    SDL_bool initializing;
    SDL_bool expected_resize;
    SDL_bool in_border_change;
    SDL_bool in_title_click;
    Uint8 focus_click_pending;
    SDL_bool skip_update_clipcursor;
    Uint64 last_updated_clipcursor;
    SDL_bool mouse_relative_mode_center;
    SDL_bool windowed_mode_was_maximized;
    SDL_bool in_window_deactivation;
    RECT cursor_clipped_rect;
    SDL_Point last_raw_mouse_position;
    SDL_bool mouse_tracked;
    SDL_bool destroy_parent_with_window;
    SDL_DisplayID last_displayID;
    WCHAR *ICMFileName;
    SDL_Window *keyboard_focus;
    struct SDL_VideoData *videodata;
#ifdef SDL_VIDEO_OPENGL_EGL
    EGLSurface egl_surface;
#endif
    
    /* Whether we retain the content of the window when changing state */
    UINT copybits_flag;
#endif
};
#endif

void lite_renderer_update_rects(RenRect* rects, int32_t count)
{
    // @note(maihd): this algorithm from SDL2 src
    //struct SDL_WindowData* data = window->driverdata;
    //for (int32_t i = 0; i < count; ++i)
    //{
    //BitBlt(data->hdc, rects[i].x, rects[i].y, rects[i].width, rects[i].height,
    //data->mdc, rects[i].x, rects[i].y, SRCCOPY);
    //}
    SDL_UpdateWindowSurfaceRects(window, (const SDL_Rect*)rects, count);
    
    static bool initial_frame = true;
    if (initial_frame)
    {
        SDL_ShowWindow(window);
        initial_frame = false;
    }
}

void lite_renderer_set_clip_rect(RenRect rect)
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
    
    SDL_Surface* surf = SDL_GetWindowSurface(window);
    *x                = (int32_t)surf->w;
    *y                = (int32_t)surf->h;
}

RenImage* lite_new_image(int32_t width, int32_t height)
{
    assert(width > 0 && height > 0);
    
    // @todo(maihd): use Arena instead of malloc
    RenImage* image =
        malloc(sizeof(RenImage) + width * height * sizeof(RenColor));
    check_alloc(image);
    image->pixels = (void*)(image + 1);
    image->width  = width;
    image->height = height;
    return image;
}

void lite_free_image(RenImage* image)
{
    free(image);
}

static GlyphSet* load_glyphset(RenFont* font, int32_t idx)
{
    GlyphSet* set = check_alloc(calloc(1, sizeof(GlyphSet)));
    
    /* init image */
    int32_t width  = 1024;
    int32_t height = 1024;
    
    for (;;)
    {
        set->image = lite_new_image(width, height);
        
        /* load glyphs */
        float s = stbtt_ScaleForMappingEmToPixels(&font->stbfont, 1) /
            stbtt_ScaleForPixelHeight(&font->stbfont, 1);
        int32_t res = stbtt_BakeFontBitmap(font->data, 0, font->size * s,
                                           (void*)set->image->pixels, width,
                                           height, idx * 256, 256, set->glyphs);
        
        /* retry with a larger image buffer if the buffer wasn't large enough */
        if (res < 0)
        {
            width *= 2;
            height *= 2;
            lite_free_image(set->image);
            continue;
        }
        
        break;
    }
    
    /* adjust glyph yoffsets and xadvance */
    int32_t ascent, descent, linegap;
    stbtt_GetFontVMetrics(&font->stbfont, &ascent, &descent, &linegap);
    float   scale = stbtt_ScaleForMappingEmToPixels(&font->stbfont, font->size);
    int32_t scaled_ascent = ascent * scale + 0.5;
    for (int32_t i = 0; i < 256; i++)
    {
        set->glyphs[i].yoff += scaled_ascent;
        set->glyphs[i].xadvance = floor(set->glyphs[i].xadvance);
    }
    
    // convert 8bit data to 32bit
    // @todo: why must be pre-convert?
    // @todo: why must be in reverted-order loop?
    for (int32_t i = width * height - 1; i >= 0; i--)
    {
        uint8_t n = ((uint8_t*)set->image->pixels)[i];
        set->image->pixels[i] =
        (RenColor){.r = 255, .g = 255, .b = 255, .a = n};
    }
    
    return set;
}

static GlyphSet* get_glyphset(RenFont* font, int32_t codepoint)
{
    int32_t idx = (codepoint >> 8) % MAX_GLYPHSET;
    if (font->sets[idx] == NULL)
    {
        font->sets[idx] = load_glyphset(font, idx);
    }
    
    return font->sets[idx];
}

RenFont* lite_load_font(const char* filename, float size)
{
    RenFont* font = nullptr;
    FILE*    fp   = nullptr;
    
    /* init font */
    font       = check_alloc(calloc(1, sizeof(RenFont)));
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
    font->data = check_alloc(malloc(buf_size));
    int32_t _  = fread(font->data, 1, buf_size, fp);
    (void)_;
    fclose(fp);
    fp = nullptr;
    
    /* init stbfont */
    int32_t ok = stbtt_InitFont(&font->stbfont, font->data, 0);
    if (!ok)
    {
        free(font->data);
        return nullptr;
    }
    
    /* get height and scale */
    int32_t ascent, descent, linegap;
    stbtt_GetFontVMetrics(&font->stbfont, &ascent, &descent, &linegap);
    float scale  = stbtt_ScaleForMappingEmToPixels(&font->stbfont, size);
    font->height = (ascent - descent + linegap) * scale + 0.5;
    
    /* make tab and newline glyphs invisible */
    stbtt_bakedchar* g = get_glyphset(font, '\n')->glyphs;
    g['\t'].x1         = g['\t'].x0;
    g['\n'].x1         = g['\n'].x0;
    
    return font;
}

void lite_free_font(RenFont* font)
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

void lite_set_font_tab_width(RenFont* font, int32_t n)
{
    GlyphSet* set              = get_glyphset(font, '\t');
    set->glyphs['\t'].xadvance = n;
}

int32_t lite_get_font_tab_width(RenFont* font)
{
    GlyphSet* set = get_glyphset(font, '\t');
    return set->glyphs['\t'].xadvance;
}

int32_t lite_get_font_width(RenFont* font, const char* text)
{
    int32_t     x = 0;
    const char* p = text;
    unsigned    codepoint;
    while (*p)
    {
        p                    = utf8_to_codepoint(p, &codepoint);
        GlyphSet*        set = get_glyphset(font, codepoint);
        stbtt_bakedchar* g   = &set->glyphs[codepoint & 0xff];
        x += g->xadvance;
    }
    return x;
}

int32_t lite_get_font_height(RenFont* font)
{
    return font->height;
}

static inline RenColor blend_pixel(RenColor dst, RenColor src)
{
    int32_t ia = 0xff - src.a;
    dst.r      = ((src.r * src.a) + (dst.r * ia)) >> 8;
    dst.g      = ((src.g * src.a) + (dst.g * ia)) >> 8;
    dst.b      = ((src.b * src.a) + (dst.b * ia)) >> 8;
    return dst;
}

static inline RenColor blend_pixel2(RenColor dst, RenColor src, RenColor color)
{
    src.a      = (src.a * color.a) >> 8;
    int32_t ia = 0xff - src.a;
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

void lite_draw_rect(RenRect rect, RenColor color)
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
    
    RenColor* d = g_surface.pixels;
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

void lite_draw_image(RenImage* image, RenRect* sub, int32_t x, int32_t y, RenColor color)
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
    
    /* draw */
    RenColor*    s    = image->pixels;
    RenColor*    d    = g_surface.pixels;
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

int32_t lite_draw_text(RenFont* font, const char* text, int32_t x, int32_t y, RenColor color)
{
    RenRect     rect;
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
        lite_draw_image(set->image, &rect, x + g->xoff, y + g->yoff, color);
        x += g->xadvance;
    }
    return x;
}

//! Leave an empty newline here, required by GCC