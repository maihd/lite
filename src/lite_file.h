#pragma once

#include "lite_meta.h"
#include "lite_string.h"

bool lite_is_binary_file(LiteStringView path);
uint64_t lite_file_write_time(LiteStringView string);

//! EOF
