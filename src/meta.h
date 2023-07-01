#pragma once

/// constexpr attributes
/// @note(maihd): constexpr help constants more meaning
#if !defined(__cplusplus) && !defined(constexpr)
#define constexpr static const
#endif

/// comptime attributes
/// @note(maihd):
///    Instead of use constexpr for functions (C++)
///    Use comptime (Zig) to mean that
///        function can be execute add compile-time (clearer mean)
#if defined(__cplusplus)
#define __comptime constexpr
#else
#define __comptime (static_assert(0, "Only C++14 and above support comptime function"), (void)0)
#endif

/// Theses primitive types should be use as language-level, not standard-level
/// @note(maihd): And this only worth using, useful standard libraries
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h> // @note(maihd): should be manually define bool for faster compile

//! new empty line, required by GCC

