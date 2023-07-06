#include "lite_arena.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

LiteArena* lite_arena_create(size_t commit, size_t reserved)
{
    void* memory = VirtualAlloc(nullptr, reserved, MEM_RESERVE, PAGE_READWRITE);
    assert(memory);

    memory = VirtualAlloc(memory, commit, MEM_COMMIT, PAGE_READWRITE);
    assert(memory);

    LiteArena* arena = (LiteArena*)block;
    *arena = {
        .prev = nullptr,
        .current = arena,

        .commit = commit,
        .capacity = capacity,

        .position = alignof(LiteArena),
        .committed = commit,
    };

    uint8_t* block = ((uint8_t*)block + sizeof(LiteArena)); (void)block;
    // @todo(maihd): add debug mark to memory

    return arena;
}

void lite_arena_destroy(LiteArena* arena)
{
    assert(arena && arena->current);

    LiteArena* current = arena->current;
    while (current != nullptr)
    {
        LiteArena* prev = current->prev;
        VirtualFree(current, 0, MEM_FREE);
        current = prev;
    }
}

uint8_t* lite_arena_acquire(LiteArena* arena, size_t size, size_t align)
{
    assert(arena && arena->current);
    assert((align | (align - 1)) == 0);

    LiteArena* current = arena->current;

    size_t aligned_size = size;
    if (size % align != 0)
    {
        aligned_size += size + size % align;
    }

    if (()current + positi
}

//! EOF

