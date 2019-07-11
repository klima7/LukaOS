#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <stddef.h>
#include <stdint.h>

#define HSIZE sizeof(HEADER)

// Czy blok pamięci jest wolny, czy zajęty
enum mem_state { FREE, USED };

// Nagłówek znajdujący się przed każdym blokiem pamięci - węzeł listy
struct mem_header
{
    uint32_t size;
    enum mem_state state;
    struct mem_header *next_block;
    struct mem_header *prev_block;

    // Dodatkowe pola, ułatwiające debugowanie
    const char *filename;
    uint32_t line;
};

typedef struct mem_header HEADER;

// Lista zawierająca bloki pamięci
struct mem_list
{
    HEADER *head;
    HEADER *tail;
    uint32_t size;
};

// Makra
#define kmalloc(_size) __kmalloc(_size, __FILE__, __LINE__);
#define kcalloc(_size) __kcalloc(_size, __FILE__, __LINE__);
#define krealloc(_ptr, _size) __krealloc(_ptr, _size, __FILE__, __LINE__);

// Prototypy
void heap_initialize(void);

void *__kmalloc(uint32_t size, const char *filename, uint32_t line);
void *__kcalloc(uint32_t size, const char *filename, uint32_t line);
void *__krealloc(void *old_ptr, uint32_t new_size, const char *filename, uint32_t line);

void kfree(void *ptr);
void debug_display_heap(void);

#endif