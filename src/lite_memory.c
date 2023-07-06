#include "lite_memory.h"
#include <assert.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

LiteArena* lite_arena_create_default(void)
{
    return lite_arena_create(LITE_ARENA_DEFAULT_COMMIT, LITE_ARENA_DEFAULT_COMMIT);
}

LiteArena* lite_arena_create(size_t commit, size_t reserved)
{
    void* memory = VirtualAlloc(nullptr, reserved, MEM_RESERVE, PAGE_READWRITE);
    assert(memory);

    memory = VirtualAlloc(memory, commit, MEM_COMMIT, PAGE_READWRITE);
    assert(memory);

    LiteArena* arena = (LiteArena*)memory;
    *arena = (LiteArena){
        .prev = nullptr,
        .current = arena,

        .commit = commit,
        .capacity = reserved,

        .position = sizeof(LiteArena),
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
    assert((align & (align - 1)) == 0);

    LiteArena* current = arena->current;

    size_t aligned_size = size;
    if (size % align != 0)
    {
        aligned_size += size + size % align;
    }

    if (current->position + aligned_size > current->capacity)
    {
        assert(aligned_size <= current->capacity);

        LiteArena* new_arena = lite_arena_create(current->commit, current->capacity);
        assert(new_arena);

        new_arena->prev = current;
        current->current = new_arena;
        current = new_arena;
    }

    if (current->position + aligned_size > current->committed)
    {
        size_t remain_size = current->committed - current->position;
        size_t commit_size = ((aligned_size - remain_size) / current->commit + 1) * current->commit;
        void* commited_block = VirtualAlloc((uint8_t*)current + current->committed, commit_size, MEM_COMMIT, PAGE_READWRITE);
        assert(commited_block); (void)commited_block;
        current->committed += commit_size;
    }

    uintptr_t address = (uintptr_t)current + current->position;
    current->position += aligned_size;

    if (address % align != 0)
    {
        address += address - address % align;
    }

    assert(address % align == 0);
    return (uint8_t*)address;
}

//! EOF

