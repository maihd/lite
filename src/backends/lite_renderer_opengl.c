#include <stdio.h>

#include "lite_renderer.h"

// Prefer low-battery GPU, so disable optimus!
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
__declspec(dllexport) DWORD  NvOptimusEnablement                    = 0; // https://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
__declspec(dllexport) int    AmdPowerXpressRequestHighPerformance   = 0; // https://developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
#endif

static void* s_gl_context = nullptr;

void lite_renderer_init(void)
{
    assert(s_gl_context != nullptr && "Attempt to reinit renderer");

#if LITE_SYSTEM_SDL2
    SDL_Window* window = (SDL_Window*)lite_window_handle();

    SDL_GL_SetAttribute(SDL_GL_RED_BITS, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_BITS, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_BITS, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_BITS, 8);
    s_gl_context = SDL_GL_CreateContext(window);
    if (s_gl_context == nullptr)
    {
        // @todo(maihd): Handle error
        // @todo(maihd): Fallback to software renderer
        return;
    }
#else
    // @todo(maihd): create context with on each platforms (WGL, GLX)
#endif

    g_img_arena = lite_arena_create(1 * 1024 * 1024, 20 * 1024 * 1024, alignof(LiteColor));
    g_font_arena = lite_arena_create(1 * 1024 * 1024, 20 * 1024 * 1024, alignof(GlyphSet));
}

void lite_renderer_deinit(void)
{
    if (s_gl_context != nullptr)
    {
    #if LITE_SYSTEM_SDL2
        SDL_GL_DeleteContext(s_gl_context);
        s_gl_context = nullptr;
    #else
        // @todo(maihd): delete OpenGL context for each platforms (WGL, GLX)
    #endif
    }
}

//! EOF

