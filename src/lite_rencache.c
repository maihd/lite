#include "lite_rencache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* a cache over the software renderer -- all drawing operations are stored as
** commands when issued. At the end of the frame we write the commands to a grid
** of hash values, take the cells that have changed since the previous frame,
** merge them into dirty rectangles and redraw only those regions */

#define CELLS_X          80
#define CELLS_Y          50
#define CELL_SIZE        96
#define COMMAND_BUF_SIZE (1024 * 512)

enum
{
    FREE_FONT,
    SET_CLIP,
    DRAW_TEXT,
    DRAW_RECT
};

typedef struct
{
    uint32_t    type;
    int32_t     size;
    LiteRect    rect;
    LiteColor   color;
    LiteFont*   font;
    int32_t     tab_width;
    char        text[0];
} Command;

static uint32_t     cells_buf1[CELLS_X * CELLS_Y];
static uint32_t     cells_buf2[CELLS_X * CELLS_Y];
static uint32_t*    cells_prev = cells_buf1;
static uint32_t*    cells      = cells_buf2;

static LiteRect     rect_buf[CELLS_X * CELLS_Y / 2];
static uint8_t      command_buf[COMMAND_BUF_SIZE];
static uint32_t     command_buf_idx;

static LiteRect     screen_rect;
static bool         show_debug;

#ifndef _WIN32
static inline int32_t min(int32_t a, int32_t b)
{
    return a < b ? a : b;
}

static inline int32_t max(int32_t a, int32_t b)
{
    return a > b ? a : b;
}
#endif

/* 32bit fnv-1a hash */
#define HASH_INITIAL 2166136261

static void hash(uint32_t* h, const void* data, int32_t size)
{
    const uint8_t* p = data;
    while (size--)
    {
        *h = (*h ^ *p++) * 16777619;
    }
}

static inline int32_t cell_idx(int32_t x, int32_t y)
{
    return x + y * CELLS_X;
}

static inline bool rects_overlap(LiteRect a, LiteRect b)
{
    return b.x + b.width >= a.x && b.x <= a.x + a.width
        && b.y + b.height >= a.y && b.y <= a.y + a.height;
}

static LiteRect intersect_rects(LiteRect a, LiteRect b)
{
    int32_t x1 = max(a.x, b.x);
    int32_t y1 = max(a.y, b.y);
    int32_t x2 = min(a.x + a.width, b.x + b.width);
    int32_t y2 = min(a.y + a.height, b.y + b.height);
    return (LiteRect){
        .x = x1,
        .y = y1,
        .width = max(0, x2 - x1),
        .height = max(0, y2 - y1)
    };
}

static LiteRect merge_rects(LiteRect a, LiteRect b)
{
    int32_t x1 = min(a.x, b.x);
    int32_t y1 = min(a.y, b.y);
    int32_t x2 = max(a.x + a.width, b.x + b.width);
    int32_t y2 = max(a.y + a.height, b.y + b.height);
    return (LiteRect){
        .x = x1,
        .y = y1,
        .width = x2 - x1,
        .height = y2 - y1
    };
}

static Command* push_command(int32_t type, int32_t size)
{
    Command* cmd = (Command*)(command_buf + command_buf_idx);
    int32_t  n   = command_buf_idx + size;
    if (n > COMMAND_BUF_SIZE)
    {
        fprintf(stderr, "Warning: (" __FILE__ "): exhausted command buffer\n");
        return NULL;
    }

    command_buf_idx = n;
    memset(cmd, 0, sizeof(Command));
    cmd->type = type;
    cmd->size = size;
    return cmd;
}

static bool next_command(Command** prev)
{
    if (*prev == NULL)
    {
        *prev = (Command*)command_buf;
    }
    else
    {
        *prev = (Command*)(((uint8_t*)*prev) + (*prev)->size);
    }
    return *prev != ((Command*)(command_buf + command_buf_idx));
}

void lite_rencache_show_debug(bool enable)
{
    show_debug = enable;
}

void lite_rencache_free_font(LiteFont* font)
{
    Command* cmd = push_command(FREE_FONT, sizeof(Command));
    if (cmd)
    {
        cmd->font = font;
    }
}

void lite_rencache_set_clip_rect(LiteRect rect)
{
    Command* cmd = push_command(SET_CLIP, sizeof(Command));
    if (cmd)
    {
        cmd->rect = intersect_rects(rect, screen_rect);
    }
}

void lite_rencache_draw_rect(LiteRect rect, LiteColor color)
{
    if (!rects_overlap(screen_rect, rect))
    {
        return;
    }

    Command* cmd = push_command(DRAW_RECT, sizeof(Command));
    if (cmd)
    {
        cmd->rect  = rect;
        cmd->color = color;
    }
}

int32_t lite_rencache_draw_text(LiteFont* font, const char* text,
                                int32_t x, int32_t y,
                                LiteColor color)
{
    LiteRect rect;
    rect.x      = x;
    rect.y      = y;
    rect.width  = lite_get_font_width(font, text);
    rect.height = lite_get_font_height(font);

    if (rects_overlap(screen_rect, rect))
    {
        int      sz  = strlen(text) + 1;
        Command* cmd = push_command(DRAW_TEXT, sizeof(Command) + sz);
        if (cmd)
        {
            memcpy(cmd->text, text, sz);
            cmd->color     = color;
            cmd->font      = font;
            cmd->rect      = rect;
            cmd->tab_width = lite_get_font_tab_width(font);
        }
    }

    return x + rect.width;
}

void lite_rencache_invalidate(void)
{
    memset(cells_prev, 0xff, sizeof(cells_buf1));
}

void lite_rencache_begin_frame(void)
{
    /* reset all cells if the screen width/height has changed */
    int32_t w, h;
    lite_renderer_get_size(&w, &h);
    if (screen_rect.width != w || h != screen_rect.height)
    {
        screen_rect.width  = w;
        screen_rect.height = h;
        lite_rencache_invalidate();
    }
}

static void update_overlapping_cells(LiteRect r, uint32_t h)
{
    int32_t x1 = r.x / CELL_SIZE;
    int32_t y1 = r.y / CELL_SIZE;
    int32_t x2 = (r.x + r.width) / CELL_SIZE;
    int32_t y2 = (r.y + r.height) / CELL_SIZE;

    for (int32_t y = y1; y <= y2; y++)
    {
        for (int32_t x = x1; x <= x2; x++)
        {
            int32_t idx = cell_idx(x, y);
            hash(&cells[idx], &h, sizeof(h));
        }
    }
}

static void push_rect(LiteRect r, int32_t* count)
{
    /* try to merge with existing rectangle */
    for (int i = *count - 1; i >= 0; i--)
    {
        LiteRect* rp = &rect_buf[i];
        if (rects_overlap(*rp, r))
        {
            *rp = merge_rects(*rp, r);
            return;
        }
    }

    /* couldn't merge with previous rectangle: push */
    rect_buf[(*count)++] = r;
}

void lite_rencache_end_frame(void)
{
    /* update cells from commands */
    Command*  cmd = nullptr;
    LiteRect  cr  = screen_rect;
    while (next_command(&cmd))
    {
        if (cmd->type == SET_CLIP)
        {
            cr = cmd->rect;
        }

        LiteRect r = intersect_rects(cmd->rect, cr);
        if (r.width == 0 || r.height == 0)
        {
            continue;
        }

        uint32_t h = HASH_INITIAL;
        hash(&h, cmd, cmd->size);
        update_overlapping_cells(r, h);
    }

    /* push rects for all cells changed from last frame, reset cells */
    int32_t rect_count = 0;
    int32_t max_x      = screen_rect.width / CELL_SIZE + 1;
    int32_t max_y      = screen_rect.height / CELL_SIZE + 1;
    for (int32_t y = 0; y < max_y; y++)
    {
        for (int32_t x = 0; x < max_x; x++)
        {
            /* compare previous and current cell for change */
            int32_t idx = cell_idx(x, y);
            if (cells[idx] != cells_prev[idx])
            {
                push_rect((LiteRect){x, y, 1, 1}, &rect_count);
            }
            cells_prev[idx] = HASH_INITIAL;
        }
    }

    /* expand rects from cells to pixels */
    for (int32_t i = 0; i < rect_count; i++)
    {
        LiteRect* r = &rect_buf[i];
        r->x *= CELL_SIZE;
        r->y *= CELL_SIZE;
        r->width *= CELL_SIZE;
        r->height *= CELL_SIZE;
        *r = intersect_rects(*r, screen_rect);
    }

    /* redraw updated regions */
    bool has_free_commands = false;
    for (int32_t i = 0; i < rect_count; i++)
    {
        /* draw */
        LiteRect r = rect_buf[i];
        lite_renderer_set_clip_rect(r);

        cmd = NULL;
        while (next_command(&cmd))
        {
            switch (cmd->type)
            {
            case FREE_FONT:
                has_free_commands = true;
                break;

            case SET_CLIP:
                lite_renderer_set_clip_rect(intersect_rects(cmd->rect, r));
                break;

            case DRAW_RECT:
                lite_draw_rect(cmd->rect, cmd->color);
                break;

            case DRAW_TEXT:
                lite_set_font_tab_width(cmd->font, cmd->tab_width);
                lite_draw_text(cmd->font,
                               cmd->text,
                               cmd->rect.x, cmd->rect.y,
                               cmd->color);
                break;
            }
        }

        if (show_debug)
        {
            LiteColor color = { .r = rand(), .g = rand(), .b = rand(), .a = 50 };
            lite_draw_rect(r, color);
        }
    }

    /* update dirty rects */
    if (rect_count > 0)
    {
        lite_renderer_update_rects(rect_buf, rect_count);
    }

    /* free fonts */
    if (has_free_commands)
    {
        cmd = NULL;
        while (next_command(&cmd))
        {
            if (cmd->type == FREE_FONT)
            {
                lite_free_font(cmd->font);
            }
        }
    }

    /* swap cell buffer and reset */
    uint32_t* tmp   = cells;
    cells           = cells_prev;
    cells_prev      = tmp;
    command_buf_idx = 0;
}

//! EOF

