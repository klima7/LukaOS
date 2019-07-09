#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stddef.h>
#include <stdint.h>

void memory_initialize(void);
void report_memory(void);

// Procedura assemblerowa
extern unsigned int get_multibot_info(void);

// Udostępnienie zmiennych innym plikom
extern struct multiboot_info *mb_info;
extern struct multiboot_mmap_entry *mb_memory_map;
extern unsigned int mb_mm_entries;

#endif