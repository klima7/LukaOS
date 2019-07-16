#include <stddef.h>
#include <stdint.h>
#include "memory_map.h"
#include "multiboot.h"
#include "clib/stdio.h"

// Funkcje statyczne
static void page_set_from_index(uint32_t nr, uint32_t state);
static uint32_t page_get_from_index(uint32_t nr);

// Mapa pamięci 4GB
uint8_t memory_map[PAGE_COUNT/PAGE_PER_STATUS_BYTE];

// Inicjuje mape stron
void memorymap_initialize(void)
{
    printf("Memory Map Initialization\n");

    // Cała pamięć jest wolna
    page_set_range(0, 0xFFFFFFFF, PAGE_FREE);

    // Zablokowanie nadpisywania pierwszych 4Mb - struktury multiboot, jądro
    page_set_range(0, HEAP_START, PAGE_SYS);

    // Wypełnia resztę mapu pamięci na podstawie danych od multiboot
    struct multiboot_mmap_entry *entry = (struct multiboot_mmap_entry *)multiboot_get_struct()->mmap_addr;
    uint32_t count = multiboot_get_struct()->mmap_length / sizeof(struct multiboot_mmap_entry);
    for(uint32_t i=0; i<count; i++)
    {
        if(entry->type != MULTIBOOT_MEMORY_AVAILABLE) page_set_range((uint32_t)entry->addr, entry->len, PAGE_NONE);
        entry++;
    }
}

// Ustala stan strony zawierającej ponany adres
void page_set(uint32_t addr, uint32_t state)
{
    uint32_t page_nr = addr / PAGE_SIZE;
    page_set_from_index(page_nr, state);
}

// Zwraca stan strony zawierającej podany adres
uint32_t page_get(uint32_t addr)
{
    uint32_t page_nr = addr / PAGE_SIZE;
    uint32_t state = page_get_from_index(page_nr);
    return state;
}

// Ustala stan wszystkim stronom w podanym zakresie adresów, włączając w to krańce
void page_set_range(uint32_t addr_start, uint32_t len, uint32_t state)
{
    uint32_t addr_end = addr_start + len;

    uint32_t first_page_nr = addr_start / PAGE_SIZE;
    uint32_t last_page_nr = addr_end / PAGE_SIZE;

    for(; first_page_nr < last_page_nr; first_page_nr++)
        page_set_from_index(first_page_nr, state);
}

// Znajduje podaną liczbę wolnych stron pod rząd, ustala ich stan na USED i zwraca adres pierwszej
uint32_t page_claim(uint32_t count)
{
    for(uint32_t i=0; i<=PAGE_COUNT-count; i++)
    {
        int ok_flag = 1;
        for(uint32_t j=0; j<count; j++)
        {
            uint32_t state = page_get_from_index(i+j);
            if(state != PAGE_FREE)
            {
                ok_flag = 0;
                break;
            }
        }

        if(ok_flag)
        {
            for(uint32_t j=0; j<count; j++)
                page_set_from_index(i+j, PAGE_USED);

            return i*PAGE_SIZE;
        }
    }

    return 0;
}

// Debugowanie - wyświetl wszystkie storony o stanie USED
void debug_display_used_pages(void)
{
    printf("Used pages: ");
    for(uint32_t i=0; i<PAGE_COUNT; i++)
    {
        if(page_get_from_index(i)==PAGE_USED)
            printf("%u ", i);
    }
    printf("\n");
}

// Ustala stan strony o podanym numerze
static void page_set_from_index(uint32_t nr, uint32_t state)
{
    uint32_t byte = nr / PAGE_PER_STATUS_BYTE;
    uint32_t part = nr % PAGE_PER_STATUS_BYTE;

    uint8_t *page_byte = memory_map + byte;

    uint8_t mask = 0b11000000 >> part*2;
    *page_byte &= ~mask;

    uint8_t new_state = (uint8_t)state;
    new_state <<= (3-part)*2;
    *page_byte |= new_state;
}

// Zwraca stan strony o podanym numerze
static uint32_t page_get_from_index(uint32_t nr)
{
    uint32_t byte = nr / PAGE_PER_STATUS_BYTE;
    uint32_t part = nr % PAGE_PER_STATUS_BYTE;

    uint8_t *page_byte = memory_map + byte;

    uint8_t mask = 0b11000000 >> part*2;
    uint8_t result = *page_byte & mask;
    result >>= (3-part)*2;

    return result;
}


