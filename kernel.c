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
#include "threads.h"
#include "sys.h"
#include "cmos.h"
#include "time.h"
#include "rng.h"
#include "pci.h"
#include "shell.h"
#include "launcher.h"
#include "network/RTL8139.h"
#include "network/ethernet.h"
#include "network/arp.h"
#include "network/ipv4.h"
#include "network/icmp.h"
#include "network/udp.h"
#include "network/utils.h"
#include "network/network_ports.h"
#include "network/network.h"
#include "additions/talk.h"
#include "clib/stdio.h"
#include "clib/string.h"
#include "clib/stdlib.h"
#include "beeper.h"
#include "timers.h"

// Wszystkie procedury inicjujące 
static void kernel_init(void)
{
	terminal_initialize();
	multiboot_initialize();
	memorymap_initialize();
	heap_initialize();
	shell_initialize();

	heap_full_initialize();
	terminal_full_initialize();
	multiboot_full_initialize();

    GDT_initialize();
	IDT_initialize();
	acpi_initialize();
	ps2_initialize();
	keyboard_initialize();
	mouse_initialize();
	clock_initialize();
	threads_initialize();
	pci_initialize();
	time_initialize();
	network_initialize();
	RTL8139_initialize();
	ports_initialize();
	arp_initialize();
	icmp_initialize();
	launcher_initialize();
	timer_initialize();
	debug_display_heap();
	beeper_initialize();
	
	printf("Kernel ready\n");

	talk_initialize();
}

// Kernel
void kernel_main(void) 
{
	kernel_init();
	beep_start(261);
	silent();
	shell_main();
}

