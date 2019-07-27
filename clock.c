#include <stddef.h>
#include <stdint.h>
#include "clock.h"
#include "idt.h"
#include "shell.h"
#include "clib/stdio.h"

//Funkcje statyczne
static void set_frequency(unsigned int f);
static void clock_command_timesys(const char* tokens, uint32_t tokens_count);

volatile unsigned long long ticks = 0;
volatile double system_time_ms = 0;
double IRQ0_interval_ms = 0;
double  IRQ0_frequency = 0;

// Inicjuje zegar
void clock_initialize(void)
{
    set_frequency(FREQUENCY);
    interrupt_register(32, clock_interrupt_handler);
    register_command("timesys", "Display time in ms since system start", clock_command_timesys);

    printf("Clock Ticking\n");
}

// Ustala zadaną częstotliwość
static void set_frequency(unsigned int f)
{
    unsigned short divider = (unsigned short)(1193182.0 / f);
    IRQ0_frequency = 1193182.0 / divider;
    IRQ0_interval_ms = 1000.0/IRQ0_frequency;
    set_PIT_count(divider);
}

// Przerywa działanie programu na podany okres
void sleep(unsigned long long t)
{
    unsigned long long end = ticks + t;
    while(ticks < end) continue;
}

// Zwraca liczbe uderzeń zegara od uruchomienia programu
unsigned long long clock(void)
{
    return ticks;
}

// Zwraca czas w ms od uruchomienia programu
double time(void)
{
    return system_time_ms;
}

// Obsługa przerwania zegara
void clock_interrupt_handler(void)
{
    system_time_ms += IRQ0_interval_ms;
    ticks += 1;
}

// Komenda timesys
static void clock_command_timesys(const char* tokens, uint32_t tokens_count)
{
    terminal_setcolor(VGA_COLOR_WHITE);
    printf("Time in ms since start: ");
    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("%ull\n", ticks);
}