#ifndef __PAGES_H__
#define __PAGES_H__

#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_COUNT 1048576

#define PAGE_FREE 0
#define PAGE_USED 1
#define PAGE_SYS  2
#define PAGE_NONE 3

#define HEAP_START 0x400000


void pagemap_initialize(void);
void page_set(uint32_t addr, uint32_t state);
void page_set_range(uint32_t addr_start, uint32_t len, uint32_t state);
uint32_t page_get(uint32_t addr);
uint32_t page_claim(uint32_t count);
void debug_display_used_pages(void);

#endif
