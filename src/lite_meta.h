// -----------------------------------------------------------------
// This file for compiler compat and extensions
// -----------------------------------------------------------------

#pragma once

/// nullptr liternal
/// @note(maihd): make one way only to present null value
#if !defined(__cplusplus)
#define nullptr ((void*)0)
#endif

/// static_assert expression
/// @note(maihd): simple define, no need to include <assert.h>
/// @todo(maihd): as fallback implementation for other C/C++ dialect
#if !defined(__cplusplus) && !defined(static_assert)
#   if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#       define static_assert(cond, msg) _Static_assert(cond, msg)
#   else
#       define static_assert(cond, msg) struct { char __[(cond) ? 1 : -1]; }
#   endif
#endif

/// thread_local attribute
/// @note(maihd): simple define, no need to include <threads.h>
/// @todo(maihd): as fallback implementation for other C/C++ dialect
#if !defined(__cplusplus) && !defined(thread_local)
#define thread_local _Thread_local
#endif

/// constexpr attribute
/// @note(maihd): constexpr help constants more meaning
#if !defined(__cplusplus) && !defined(constexpr)
#define constexpr static const
#endif

/// comptime attribute
/// @note(maihd):
///    Instead of use constexpr for functions (C++)
///    Use comptime (Zig) to mean that
///        function can be execute add compile-time (clearer mean)
#if defined(__cplusplus) && __cplusplus >= 201402L
#define __comptime constexpr
#else
#define __comptime                                                             \
    static_assert(0, "Only C++14 and above support comptime function"); // @note(maihd):
                                                                        // add
                                                                        // ';' here because
                                                                        // we donot know is
                                                                        // static_assert a
                                                                        // expression or not
                                                                        // base on compilers
                                                                        // implementation
#endif

/// alignas attribute
/// @note(maihd): simple define, no need to include <stdalign.h>
/// @todo(maihd): as fallback implementation for other C/C++ dialect
#if !defined(__cplusplus)
#   if defined(_MSC_VER)
#       define alignas(x) __declspec(align(x))
#   else
#       define alignas(x) _Alignas(x)
#   endif
#endif

/// alignof operator
/// @note(maihd): simple define, no need to include <stdalign.h>
/// @todo(maihd): as fallback implementation for other C/C++ dialect
#if !defined(__cplusplus)
#   if defined(_MSC_VER)
#       define alignof(x) __alignof(x)
// #   elif defined(__GNUC__)
// #       define alignof(x) __alignof__(x)
#   else
#       define alignof(x) _Alignof(x)
#   endif
#endif

/// __forceinline attribute
/// @note(maihd): polyfill for multiple compiler to make sure will inline even
/// on optimize off (unsure)
#if !defined(_MSC_VER) && !defined(__forceinline)
#   if defined(__GNUC__)
#       define __forceinline __attribute__((always_inline))
#   else
#       define __forceinline inline
#   endif
#endif

/// __enum_vtype attribute
/// @note(maihd): the placeholder for C enum's members value type, that also work for C++
#if !defined(__enum_vtype)
#   if defined(__cplusplus)
#       define __enum_vtype(T) : T 
#   else
#       define __enum_vtype(T)
#   endif
#endif


/// __count_of operator
/// @note(maihd):
#ifndef __count_of
// #define __count_of(array)                               \
//     (static_assert(sizeof(array) >= sizeof(array[0]), "__count_of only work on C Array"),  \
//      (sizeof(array) / sizeof(array[0])))
#define __count_of(array) (sizeof(array) / sizeof((array)[0]))
#endif

/// Theses primitive types should be use as language-level, not standard-level
/// @note(maihd): And theses is only worth using, useful standard libraries
#include <stdbool.h> // @note(maihd): should be manually define bool for faster compile
#include <stddef.h>
#include <stdint.h>

//! new empty line, required by GCC