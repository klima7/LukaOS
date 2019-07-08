#include "kernel.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "buffer.h"
#include "terminal.h"
#include "clock.h"
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
	printf("Kernel ready\n");

	//Demonstracja zegara
	sleep(1000);
	printf("\nOdliczanie... ");
	sleep(1000);
	printf("3 ");
	sleep(1000);
	printf("2 ");
	sleep(1000);
	printf("1 \n");
	sleep(1000);

	//Wyświetlanie wpisywanych znaków
	printf("\nYou can write >> ");
	while(1)
	{
		if(!buffer_isempty(&keyboard_buffer))
			putchar(buffer_get(&keyboard_buffer));
	}
}