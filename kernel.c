#include "kernel.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "buffer.h"
#include "terminal.h"
#include "clock.h"
#include "memory.h"
#include "allocator.h"
#include "clib/stdio.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void kernel_main(void) 
{
	//Inicjalizacja
	terminal_initialize();
    GDT_initialize();
	IDT_initialize();
	keyboard_initialize();
	clock_initialize();
	memory_initialize();
	allocator_initialize();
	printf("Kernel ready\n");

	printf("\n");
	report_memory();
	
	printf("\n");
	debug_display_memory();

	printf("\n");
	int *block1 = kcalloc(654304ull);
	printf("kalloc1 = %u\n", (uint32_t)block1);
	int *block2 = kcalloc(100);
	printf("kalloc2 = %u\n", (uint32_t)block2);

	printf("\n");
	debug_display_memory();

	kfree(block1);
	kfree(block2);

	printf("\n");
	debug_display_memory();

	//Wyświetlanie wpisywanych znaków
	printf("\nYou can write >> ");
	while(1)
	{
		if(!buffer_isempty(&keyboard_buffer))
			putchar(buffer_get(&keyboard_buffer));
	}
}