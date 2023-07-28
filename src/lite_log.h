#pragma once

#include <stdlib.h>

#ifndef NDEBUG
#define lite_log_debug(fmt, ...) ((void)0)
#else
#define lite_log_debug(fmt, ...) ((void)printf(fmt, ##__VA_ARGS__))
#endif

//!EOF

