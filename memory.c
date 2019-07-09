#include "memory.h"
#include "multiboot.h"
#include "clib/stdio.h"
#include <stddef.h>
#include <stdint.h>

struct multiboot_info *mb_info = NULL;
struct multiboot_mmap_entry *mb_memory_map = NULL;
unsigned int mb_mm_entries = 0;

// Inicjuje mape pamięci
void memory_initialize(void)
{
    printf("Memory Detection\n");

    // Jeżeli bit jest ustawiony to mapa jest poprawna
    uint32_t flags = mb_info->flags;
    if((flags & (1u << 6)) == 0)
    {
        // Mamy problem
        printf("Memory Detection Failed\n");
    }

    uint32_t address = get_multibot_info();
    mb_info = (struct multiboot_info *)address;
    mb_memory_map = (struct multiboot_mmap_entry *)mb_info->mmap_addr;
    mb_mm_entries = mb_info->mmap_length / sizeof(struct multiboot_mmap_entry);
}

// Wyświetla informacje o dostępnej pamięci
void report_memory(void)
{
    printf("MEM Begin      MEM Len        MEM Type\n");
    for(unsigned int i=0; i<mb_mm_entries; i++)
    {
        int count = printf("%llu", mb_memory_map->addr);
        for(; count<15; count++) printf(" ");
        count = printf("%llu", mb_memory_map->addr + mb_memory_map->len);
        for(; count<15; count++) printf(" ");
        unsigned int type = mb_memory_map->type;
        if(type==MULTIBOOT_MEMORY_AVAILABLE) printf("Available\n");
        else if(type==MULTIBOOT_MEMORY_RESERVED) printf("Reserved\n");
        else if(type==MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) printf("ACPI\n");
        else if(type==MULTIBOOT_MEMORY_NVS) printf("NVS\n");
        else if(type==MULTIBOOT_MEMORY_BADRAM) printf("Badram\n");
        mb_memory_map++;
    }
}

