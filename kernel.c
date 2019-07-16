#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "buffer.h"
#include "terminal.h"
#include "clock.h"
#include "multiboot.h"
#include "heap.h"
#include "memory_map.h"
#include "acpi.h"
#include "mouse.h"
#include "pic.h"
#include "ps2.h"
#include "clib/stdio.h"
#include "clib/string.h"

// Wszystkie procedury inicjujące 
static void kernel_init(void)
{
	terminal_initialize();
    GDT_initialize();
	IDT_initialize();
	acpi_initialize();
	ps2_initialize();
	keyboard_initialize();
	mouse_initialize();
	terminal_full_initialize();
	clock_initialize();
	multiboot_initialize();
	memorymap_initialize();
	heap_initialize();
	printf("Kernel ready\n");
}

// Kernel
void kernel_main(void) 
{
	kernel_init();

	sleep(1000);
	struct buffer_t *keyboard_buffer = keyboard_get_buffer();
	printf("\nYou can write >> ");
	while(1)
	{
		if(!buffer_isempty(keyboard_buffer))
			putchar(buffer_get(keyboard_buffer));
	}
}

