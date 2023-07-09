#pragma once

#include "lite_meta.h"
#include "lite_string.h"
#include "lite_memory.h"

typedef struct LiteReadDirState LiteReadDirState;

bool                lite_is_binary_file(LiteStringView path);
uint64_t            lite_file_write_time(LiteStringView string);

LiteReadDirState*   lite_read_dir(LiteArena* arena, LiteStringView string);
LiteStringView      lite_read_next_file(LiteReadDirState* read_dir_state);

//! EOF

