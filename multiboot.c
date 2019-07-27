#include <stddef.h>
#include <stdint.h>
#include "multiboot.h"
#include "sys.h"
#include "terminal.h"
#include "shell.h"
#include "clib/stdio.h"
#include "clib/string.h"

// Funkcje statyczne
static void multiboot_command_meminfo(const char* tokens, uint32_t tokens_count);

struct multiboot_info *multiboot_info;

// Odczytuje adres struktury multiboot_info i upewnia się, że nie ma problemu z pamięcią
void multiboot_initialize(void)
{
    printf("Multiboot Initialization\n");

    uint32_t address = get_multibot_info();
    multiboot_info = (struct multiboot_info*)address;

    // Jeżeli bit jest ustawiony to mapa jest poprawna
    uint32_t flags = multiboot_info->flags;
    if((flags & (1u << 6)) == 0) kernel_panic("Memory Detection Failed\n");

    multiboot_debug_report_memory();
}

// Kończy inicjalizacje - wymaga aby stos i powłoka zostały zainicjowane wcześniej
void multiboot_full_initialize(void)
{
    register_command("meminfo", "Display information about memory placement", multiboot_command_meminfo);
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
    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("| MEM Begin      | MEM End        | MEM Type\n");
    terminal_setcolor(VGA_COLOR_WHITE);
    for(unsigned int i=0; i<count; i++)
    {
        int count = printf("| %llu", entry->addr);
        for(; count<17; count++) printf(" ");
        count = printf("| %llu", entry->addr + entry->len);
        for(; count<17; count++) printf(" ");
        unsigned int type = entry->type;
        if(type==MULTIBOOT_MEMORY_AVAILABLE) printf("| Available\n");
        else if(type==MULTIBOOT_MEMORY_RESERVED) printf("| Reserved\n");
        else if(type==MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) printf("| ACPI\n");
        else if(type==MULTIBOOT_MEMORY_NVS) printf("| NVS\n");
        else if(type==MULTIBOOT_MEMORY_BADRAM) printf("| Badram\n");
        entry++;
    }
}

// Komenda meminfo
static void multiboot_command_meminfo(const char* tokens, uint32_t tokens_count)
{
    multiboot_debug_report_memory();
}
