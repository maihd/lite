/* date = July 2nd 2023 5:50 am */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/// StringView
/// Data structure contain string characters and it length
/// Utf8 support
typedef struct StringView
{
    size_t size;
    const char* data;
} StringView;

/// Calculate string length (utf8 support)
const size_t string_count(const char* string);

//! new empty line, required by GCC

