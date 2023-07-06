#pragma once

#include "lite_meta.h"

typedef struct LiteArena LiteArena;

struct LiteArena
{
    LiteArena*  prev;       // For delete
    LiteArena*  next;       // For fast remove one arena and its children only

    LiteArena*  current;    // Current arena have free memory

    size_t      commit;     // Store pre-commit to create new arena when out of capacity
    size_t      capacity;   // Reversed capacity of virtual memory this arena have register

    size_t      position;   // Current use of memory cursor
    size_t      committed;  // Number of bytes memory that committed (<= capacity)

    size_t      alignment;  // Alignment of memory block that arena use (and affect memory block that are acquired by user)
};

constexpr size_t LITE_ARENA_DEFAULT_COMMIT      =    1 * 1024 * 1024;
constexpr size_t LITE_ARENA_DEFAULT_REVERSED    = 1024 * 1024 * 1024;

LiteArena*  lite_arena_create_default(void);
LiteArena*  lite_arena_create(size_t commit, size_t reserved);
void        lite_arena_destroy(LiteArena* arena);

uint8_t*    lite_arena_acquire(LiteArena* lite_arena, size_t size, size_t align);
//void        lite_arena_collect()

//! EOF

