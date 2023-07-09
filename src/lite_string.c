#include "lite_string.h"

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

