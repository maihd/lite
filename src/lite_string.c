#include "lite_string.h"
#include "lite_memory.h"
#include <string.h>

LiteStringView lite_string_temp(const char* string)
{
    uint32_t length = lite_string_count(string);
    char*    buffer =
        (char*)lite_arena_acquire(lite_frame_arena_get(), (size_t)length + 1);
    memcpy(buffer, string, (size_t)length + 1);
    return (LiteStringView){.hash = 0, .buffer = buffer, .length = length};
}

// @funcdef(lite_string_count)
uint32_t lite_string_count(const char* string)
{
    // @todo(maihd): apply string length calculate with
    //     simd intrinsics
    uint32_t count = 0;
    while (*string++)
    {
        count++;
    }

    return count;
}

//! EOF
