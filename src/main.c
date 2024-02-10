#include "lite_rencache.h"
#include "lite_renderer.h"
#include "lite_startup.h"
#include "lite_window.h"

#ifdef _WIN32
#ifdef NDEBUG
#define USE_TERMINAL_CONSOLE 0
#else
#define USE_TERMINAL_CONSOLE 1
#endif
#else
#define USE_TERMINAL_CONSOLE 0
#endif

#ifdef _WIN32
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmd,
                   int nShowCmd)
#else
int main(int argc, char** argv)
#endif
{
#if defined(_WIN32)
    (void)hInstance;
    (void)hPrevInstance;
    (void)pCmd;
    (void)nShowCmd;

    int    argc = __argc;
    char** argv = __argv;
#endif

#if USE_TERMINAL_CONSOLE
    lite_console_open();
#endif

    lite_window_open();
    lite_renderer_init();
    lite_rencache_init();

    const LiteStartupParams startup_params = {
        .argc = (uint32_t)argc,
        .argv = (const char**)argv,
        .title = "lite",
        .window_handle = lite_window_handle(),
        .flags = LiteStartupFlags_None
    };
    lite_startup(startup_params);

    lite_rencache_deinit();
    lite_renderer_deinit();
    lite_window_close();

#if USE_TERMINAL_CONSOLE
    lite_console_close();
#endif

    return EXIT_SUCCESS;
}

//! EOF
