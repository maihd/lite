#include "lite_renderer.h"
#include "lite_api.h"
#include "lite_rencache.h"


static LiteColor luaL_checkcolor(lua_State* L, int idx, int def)
{
    LiteColor color;
    if (lua_isnoneornil(L, idx))
    {
        return (LiteColor){def, def, def, 255};
    }

    lua_rawgeti(L, idx, 1);
    lua_rawgeti(L, idx, 2);
    lua_rawgeti(L, idx, 3);
    lua_rawgeti(L, idx, 4);
    color.r = (uint8_t)luaL_checknumber(L, -4);
    color.g = (uint8_t)luaL_checknumber(L, -3);
    color.b = (uint8_t)luaL_checknumber(L, -2);
    color.a = (uint8_t)luaL_optnumber(L, -1, 255);
    lua_pop(L, 4);
    return color;
}


static int f_show_debug(lua_State* L)
{
    luaL_checkany(L, 1);
    lite_rencache_show_debug(lua_toboolean(L, 1));
    return 0;
}


static int f_get_size(lua_State* L)
{
    int w, h;
    lite_renderer_get_size(&w, &h);
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    return 2;
}


static int f_begin_frame(lua_State* L)
{
    lite_rencache_begin_frame();
    return 0;
}


static int f_end_frame(lua_State* L)
{
    lite_rencache_end_frame();
    return 0;
}


static int f_set_clip_rect(lua_State* L)
{
    LiteRect rect;
    rect.x      = (int32_t)luaL_checknumber(L, 1);
    rect.y      = (int32_t)luaL_checknumber(L, 2);
    rect.width  = (int32_t)luaL_checknumber(L, 3);
    rect.height = (int32_t)luaL_checknumber(L, 4);
    lite_rencache_set_clip_rect(rect);
    return 0;
}


static int f_draw_rect(lua_State* L)
{
    LiteRect rect;
    rect.x          = (int32_t)luaL_checknumber(L, 1);
    rect.y          = (int32_t)luaL_checknumber(L, 2);
    rect.width      = (int32_t)luaL_checknumber(L, 3);
    rect.height     = (int32_t)luaL_checknumber(L, 4);
    LiteColor color = luaL_checkcolor(L, 5, 255);
    lite_rencache_draw_rect(rect, color);
    return 0;
}


static int f_draw_text(lua_State* L)
{
    LiteFont**        font   = luaL_checkudata(L, 1, API_TYPE_FONT);
    LiteStringView    text   = lua_checkstringview(L, 2);
    int32_t           x      = (int32_t)luaL_checknumber(L, 3);
    int32_t           y      = (int32_t)luaL_checknumber(L, 4);
    LiteColor         color  = luaL_checkcolor(L, 5, 255);
    int32_t           next_x = lite_rencache_draw_text(*font, text, x, y, color);
    lua_pushnumber(L, next_x);
    return 1;
}


static const luaL_Reg lib[] = {
    {"show_debug",    f_show_debug   },
    {"get_size",      f_get_size     },
    {"begin_frame",   f_begin_frame  },
    {"end_frame",     f_end_frame    },
    {"set_clip_rect", f_set_clip_rect},
    {"draw_rect",     f_draw_rect    },
    {"draw_text",     f_draw_text    },
    {NULL,            NULL           }
};


int luaopen_renderer(lua_State* L)
{
    extern int luaopen_renderer_font(lua_State* L);
    
    luaL_newlib(L, lib);
    luaopen_renderer_font(L);
    lua_setfield(L, -2, "font");
    return 1;
}


//! EOF
