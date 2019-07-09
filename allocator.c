#include "allocator.h"
#include "memory.h"
#include "multiboot.h"
#include "clib/stdio.h"
#include <stddef.h>
#include <stdint.h>

// Pierwszy węzeł listy - Początek przestrzeni sterty
struct mem_header *mem_start;



// Inicjalizuje menadżer pamięci
void allocator_initialize(void)
{
    mem_start = NULL;

    struct multiboot_mmap_entry *entry = mb_memory_map;
    unsigned int i = 0;

    HEADER *mem_tail = NULL;

    // Szukamy pierwszego dostępnego obszaru
    while(entry->type != MULTIBOOT_MEMORY_AVAILABLE && i<mb_mm_entries) { entry++; i++; }

    if(i==mb_mm_entries)
    {
        // Mamy problem
        printf("Couldn't Find Any Available Memory Block\n");
    }

    // Dodaje wartownika przed pierwszym obszarem
    HEADER *guard = create_header_at((uint32_t)entry->addr, NULL, NULL, 0, END_OF_MEMORY);
    HEADER *first = create_header_at((uint32_t)(entry->addr+HSIZE), guard, NULL, entry->len-HSIZE, FREE);
    guard->next_block = first;
    mem_start = first;
    mem_tail = first;

    entry++; i++;

    // Szukamy reszty dostępnych obszarów
    for(; i<mb_mm_entries; i++)
    {
        if(entry->type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            HEADER *next = create_header_at((uint32_t)entry->addr, mem_tail, NULL, entry->len, FREE);
            mem_tail->next_block = next;
        }
        entry++;
    }

    // Dodaje wartownika na koniec ostatniego obszaru
    mem_tail->size -= HSIZE;
    uint32_t guard_addr = (uint32_t)mem_tail + HSIZE;
    create_header_at(guard_addr, mem_tail, NULL, 0, END_OF_MEMORY);

    printf("Allocator Ready\n");
}

// Tworzy nagłówek bloku pamięci w danym miejscu o podanych parametrach
HEADER *create_header_at(uint32_t address, struct mem_header *prev, struct mem_header *next, uint32_t size, int state)
{
    struct mem_header *header = (HEADER*)address;
    header->prev_block = prev;
    header->next_block = next;
    header->size = size;
    header->state = state;
    return header;
}

// Wydziela mniejszy blok pamięci z większego i zwraca adres mniejszego
HEADER *memory_split(HEADER *block, uint32_t size)
{
    if(block->size <= HSIZE || block->state != FREE)
        return NULL;

    uint32_t new_header_address = (uint32_t)block + sizeof(struct mem_header) + size;
    HEADER *new_block = create_header_at(new_header_address, block, block->next_block, block->size - size - HSIZE, FREE);

    block->next_block = new_block;
    block->size = size;

    return block;
}

// Łączy podany blok z blokiem następnym i go zwraca
HEADER *memory_join(HEADER *block)
{
    if(block->state != FREE || block->next_block->state != FREE) return NULL;

    // Sąsiednie bloki z listy mogą pochodzić z różnich obszarów pamięci więc trzeba sprawdzić czy na prawde sąsiadują
    if((uint32_t)block + block->size + HSIZE != (uint32_t)block->next_block) return NULL;

    block->size = block->size + block->next_block->size + HSIZE;
    block->next_block = block->next_block->next_block;
    block->next_block->prev_block = block;

    return block;
}

// Alokuje podaną liczbe bajtów
void *kmalloc(uint32_t size)
{
    HEADER *current = mem_start;

    while(current->state != FREE || current->size < size)
    {
        current = current->next_block;

        // Nie udało się zaalokować pamięci
        if(current->state == END_OF_MEMORY) 
            return NULL;
    }

    HEADER *found = current;

    // Jeżeli znaleziony blok jest za duży to podziel go na dwa
    if((int64_t)current->size - (int64_t)size - (int64_t)HSIZE > 0)
        found = memory_split(current, size);

    found->state = USED;

    return found+1;
}

// Alokuje podaną liczbe bajtów i zeruje wszystie bajty
void *kcalloc(uint32_t size)
{
    char *ptr = kmalloc(size);
    if(ptr == NULL) return NULL;

    for(uint32_t i=0; i<size; i++)
        *(ptr + i) = 0;

    return ptr;
}

// Przeprowadza relokacje danego obszaru
void *krealloc(void *old_ptr, uint32_t new_size)
{
    // Przeprowadz zwykłą alokacje
    if(old_ptr == NULL) 
        return kmalloc(new_size);

    HEADER *old_block = (HEADER*)old_ptr;
    old_block--;

    // Nie rób nic, aktualny rozmiar jest wystarczający
    if(old_block->size >= new_size)
        return old_ptr;

    // Przeprowadz relokacje
    char *new_ptr = kmalloc(new_size);
    if(new_ptr == NULL) return NULL;

    // Kopiowanie
    char *old_ptr_char = (char*)old_ptr;
    for(uint32_t i=0; i<old_block->size; i++)
        *(new_ptr+i) = *(old_ptr_char + i);
    
    kfree(old_ptr);
    return new_ptr;
}

// Zwalnia zaalokowany wcześniej obszar
void kfree(void *address)
{
    HEADER *block = (HEADER*)address;
    block--;

    if(block->state != USED) return;

    block->state = FREE;

    // Łączy powstały pusty blok z sąsiednimi
    if(block->next_block->state == FREE) memory_join(block);
    if(block->prev_block->state == FREE) memory_join(block->prev_block);
}

// Debugowanie
void debug_display_memory(void)
{
    HEADER *current = mem_start;

    printf("Block Begin    Block Len      Block Type\n");
    while(current->state != END_OF_MEMORY)
    {
        int count = printf("%u", (unsigned int)(current+1));
        for(; count<15; count++) printf(" ");
        count = printf("%u", current->size);
        for(; count<15; count++) printf(" ");
        if(current->state == FREE) printf("FREE\n");
        if(current->state == USED) printf("USED\n");
        current = current->next_block;
    }
}


