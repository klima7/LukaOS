#include <stddef.h>
#include <stdint.h>
#include "multiboot.h"
#include "clib/stdio.h"
#include "clib/string.h"

struct multiboot_info *multiboot_info;

// Odczytuje adres struktury multiboot_info i upewnia się, że nie ma problemu z pamięcią
void multiboot_initialize(void)
{
    printf("Multiboot Initialization\n");

    uint32_t address = get_multibot_info();
    multiboot_info = (struct multiboot_info*)address;

    // Jeżeli bit jest ustawiony to mapa jest poprawna
    uint32_t flags = multiboot_info->flags;
    if((flags & (1u << 6)) == 0)
    {
        // Mamy problem
        printf("Memory Detection Failed\n");
    }
}

// Zwraca wskaźnika na strukturę multiboot info
struct multiboot_info *multiboot_get_struct(void)
{
    return multiboot_info;
}

// Wyświetla informacje o dostępnej pamięci
void multiboot_debug_report_memory(void)
{
    unsigned int count = multiboot_info->mmap_length / sizeof(struct multiboot_mmap_entry);
    struct multiboot_mmap_entry *entry = (struct multiboot_mmap_entry *)multiboot_info->mmap_addr;
    printf("MEM Begin      MEM End        MEM Type\n");
    for(unsigned int i=0; i<count; i++)
    {
        int count = printf("%llu", entry->addr);
        for(; count<15; count++) printf(" ");
        count = printf("%llu", entry->addr + entry->len);
        for(; count<15; count++) printf(" ");
        unsigned int type = entry->type;
        if(type==MULTIBOOT_MEMORY_AVAILABLE) printf("Available\n");
        else if(type==MULTIBOOT_MEMORY_RESERVED) printf("Reserved\n");
        else if(type==MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) printf("ACPI\n");
        else if(type==MULTIBOOT_MEMORY_NVS) printf("NVS\n");
        else if(type==MULTIBOOT_MEMORY_BADRAM) printf("Badram\n");
        entry++;
    }
}

