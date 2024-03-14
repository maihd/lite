#pragma once

#include "lite_meta.h"

/// StringView
/// Data structure contain string characters and it length
/// Utf8 support
/// @note(maihd):
///     the size/alignment need to fit one register, StringView commonly be using as parameter
typedef struct alignas(16) LiteStringView
{
    size_t      length;
    const char* buffer;
} LiteStringView;


/// StringBuffer
/// Data structure contain mutable string
/// Utf8 support
/// This type is pointer type (maybe another term help clear this section)
/// @sample(maihd):
///     StringBuffer* - right
///     StringBuffer  - wrong (compiler error)
typedef struct LiteStringBuffer
{
    uint32_t    mark;
    uint32_t    hash;
    uint32_t    flags;
    uint32_t    length;
    uint32_t    capacity;
    char        data[];
} LiteStringBuffer;


/// Create string view, memory from frame buffer
LiteStringView  lite_string_temp(const char* string);

/// Calculate string length (utf8 support)
size_t          lite_string_count(const char* string);

/// Find last index of character
int32_t         lite_last_index_of_char(LiteStringView string, char c);

// --------------------------------------------------------------------------------
// Utils
// --------------------------------------------------------------------------------

/// Create StringView
__forceinline
LiteStringView lite_string_view(const char* string, size_t length)
{
    LiteStringView string_view;
    string_view.length = length;
    string_view.buffer = string;
    return string_view;
}


/// Create string view, calculate length
__forceinline
LiteStringView lite_string_view_cstr(const char* string)
{
    return lite_string_view(string, lite_string_count(string));
}


/// Create StringView from string literal
#define lite_string_lit(string) lite_string_view(string, sizeof(string) - 1)

//! EOF

