#pragma once

#include "lite_meta.h"

typedef uint32_t LiteStartupFlags;
enum LiteStartupFlags
{
    LiteStartupFlags_None                   = 0,
    LiteStartupFlags_Splash                 = 1 << 0,
    LiteStartupFlags_NoEditor               = 1 << 1,
    LiteStartupFlags_SafeMode               = 1 << 2,
    LiteStartupFlags_SafeModeOnFailure      = 1 << 3,
    LiteStartupFlags_SafeModeAskOnFailure   = 1 << 4,
};

typedef struct LiteStartupParams
{
    uint32_t            argc;
    const char**        argv;

    const char*         title;
    void*               window_handle;

    LiteStartupFlags    flags;
} LiteStartupParams;

void lite_startup(const LiteStartupParams params);

//! EOF
