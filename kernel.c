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
#include "pagemap.h"
#include "clib/stdio.h"
#include "clib/string.h"

void kernel_main(void) 
{
	//Inicjalizacja
	terminal_initialize();
    GDT_initialize();
	IDT_initialize();
	keyboard_initialize();
	clock_initialize();
	multiboot_initialize();
	pagemap_initialize();
	heap_initialize();
	printf("Kernel ready\n");

	printf("\n");
	debug_display_heap();

	int *temp1 = (int*)kmalloc(2000);
	int *temp2 = (int*)kmalloc(2000);

	printf("\n");
	debug_display_heap();

	int *temp3 = (int*)krealloc(temp1, 10000);

	printf("\n");
	debug_display_heap();

	int *temp4 = (int*)kcalloc(2000);

	printf("\n");
	debug_display_heap();

	kfree(temp2);
	kfree(temp3);
	kfree(temp4);

	printf("\n");
	debug_display_heap();
	printf("\n");
}