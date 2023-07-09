/* date = July 2nd 2023 5:50 am */

#pragma once

#include "lite_meta.h"

/// StringView
/// Data structure contain string characters and it length
/// Utf8 support
typedef struct LiteStringView
{
    uint32_t    hash;
    uint32_t    length;
    const char* buffer;
} LiteStringView;

/// StringBuffer
/// Data structure contain mutable string
/// Utf8 support
/// This type is pointer type (maybe another term help clear this section)
/// @sample(maihd):
///     StringBuffer* - right
///     StringBuffer  - wrong
typedef struct LiteStringBuffer
{
    uint32_t mark;
    uint32_t hash;
    uint32_t flags;
    uint32_t length;
    uint32_t capacity;
    char     data[];
} LiteStringBuffer;

/// Calculate string length (utf8 support)
uint32_t lite_string_count(const char* string);

/// Create StringView
static __forceinline LiteStringView lite_string_view(const char* string, uint32_t length, uint32_t hash)
{
    LiteStringView string_view;
    string_view.hash = hash;
    string_view.length = length;
    string_view.buffer = string;
    return string_view;
}

//! new empty line, required by GCC
