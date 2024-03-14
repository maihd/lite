#include "lite_string.h"
#include "lite_memory.h"

#include <string.h>


// @funcdef(lite_string_temp)
LiteStringView lite_string_temp(const char* string)
{
    size_t length = lite_string_count(string);
    char*  buffer = (char*)lite_arena_acquire(lite_frame_arena_get(), (size_t)length + 1);
    memcpy(buffer, string, length + 1);
    return (LiteStringView){.buffer = buffer, .length = length};
}


// @funcdef(lite_string_count)
size_t lite_string_count(const char* string)
{
    // @todo(maihd): apply string length calculate with
    //     simd intrinsics
    size_t count = 0;
    while (*string++)
    {
        count++;
    }

    return count;
}


// @funcdef(lite_last_index_of_char)
int32_t lite_last_index_of_char(LiteStringView string, char c)
{
    for (int32_t i = string.length - 1; i > -1; i--)
    {
        if (string.buffer[i] == c)
        {
            return i;
        }
    }

    return -1;
}


//! EOF

