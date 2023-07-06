#pragma once

#include "lite_meta.h"

typedef struct LiteArena LiteArena;

alignas(64)
struct LiteArena
{
    LiteArena*  prev;
    LiteArena*  current;

    size_t      commit;     // Cache pre-commit to create new arena when out of capacity
    size_t      capacity;

    size_t      position;
    size_t      committed;
};

constexpr size_t LITE_ARENA_DEFAULT_COMMIT      =    1 * 1024 * 1024;
constexpr size_t LITE_ARENA_DEFAULT_REVERSED    = 1024 * 1024 * 1024;

LiteArena*  lite_arena_create(size_t commit, size_t reserved);
void        lite_arena_destroy(LiteArena* arena);

uint8_t*    lite_arena_acquire(LiteArena* lite_arena, size_t size, size_t align);
//void        lite_arena_collect()

//! EOF

