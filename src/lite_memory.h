#pragma once

#include "lite_meta.h"

typedef struct LiteArena     LiteArena;
typedef struct LiteArenaTemp LiteArenaTemp;

struct LiteArena
{
    LiteArena* prev;    // For delete
    LiteArena* current; // Current arena have free memory

    size_t commit; // Store pre-commit to create new arena when out of capacity
    size_t capacity;  // Reversed capacity of virtual memory this arena have
                      // register

    size_t position;  // Current use of memory cursor
    size_t committed; // Number of bytes memory that committed (<= capacity)

    size_t alignment; // Alignment of memory block that arena use (and affect
                      // memory block that are acquired by user)
    size_t _reversed;
};

struct LiteArenaTemp
{
    LiteArena* arena;
    size_t     mark_position;
};

constexpr size_t LITE_ARENA_DEFAULT_COMMIT    = 1 * 1024 * 1024;
constexpr size_t LITE_ARENA_DEFAULT_REVERSED  = 1024 * 1024 * 1024;
constexpr size_t LITE_ARENA_DEFAULT_ALIGNMENT = 16;

LiteArena* lite_arena_create_default(void);
LiteArena* lite_arena_create(size_t commit, size_t reserved, size_t alignment);
void       lite_arena_destroy(LiteArena* arena);

uint8_t* lite_arena_acquire(LiteArena* lite_arena, size_t size);
// void        lite_arena_collect()

LiteArena* lite_frame_arena_get(void);

void lite_frame_arena_begin(void);
void lite_frame_arena_end(void);

static __forceinline LiteArenaTemp lite_arena_begin_temp(LiteArena* arena)
{
    LiteArenaTemp temp;
    temp.arena         = arena;
    temp.mark_position = arena->current->position;
    return temp;
}

static __forceinline void lite_arena_end_temp(LiteArenaTemp temp)
{
    temp.arena->current->position = temp.mark_position;
}

//! EOF
