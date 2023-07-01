/* date = July 2nd 2023 5:50 am */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/// StringView
/// Data structure contain string characters and it length
/// Utf8 support
typedef struct StringView
{
    uint32_t hash;
    uint32_t length;
    const char* buffer;
} StringView;

/// StringBuffer
/// Data structure contain mutable string
/// Utf8 support
/// This type is pointer type (maybe another term help clear this section)
/// @sample(maihd):
///     StringBuffer* - right
///     StringBuffer  - wrong
typedef struct StringBuffer
{
    uint32_t mark;
    uint32_t hash;
    uint32_t flags;
    uint32_t length;
    uint32_t capacity;
    char data[];
} StringBuffer;

/// Calculate string length (utf8 support)
const size_t string_count(const char* string);

//! new empty line, required by GCC

