#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <stddef.h>
#include <stdint.h>

// Czy blok pamięci jest wolny, czy zajęty
enum mem_state { FREE, USED, END_OF_MEMORY };

// Nagłówek znajdujący się przed każdym blokiem pamięci
struct mem_header
{
    uint32_t size;
    enum mem_state state;
    struct mem_header *next_block;
    struct mem_header *prev_block;
};

typedef struct mem_header HEADER;

#define HSIZE sizeof(HEADER)

// Prototypy
void allocator_initialize(void);
void *kmalloc(uint32_t size);
void *kcalloc(uint32_t size);
void *krealloc(void *old_ptr, uint32_t new_size);
void kfree(void *address);
HEADER *create_header_at(uint32_t address, struct mem_header *prev, struct mem_header *next, uint32_t size, int state);
HEADER *memory_split(HEADER *block, uint32_t size);
HEADER *memory_join(HEADER *block);
void debug_display_memory(void);

#endif