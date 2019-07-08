#include "clock.h"
#include "clib/stdio.h"
#include <stddef.h>
#include <stdint.h>

volatile unsigned long long ticks = 0;
volatile double system_time_ms = 0;
double IRQ0_interval_ms = 0;
double  IRQ0_frequency = 0;

// Inicjuje zegar
void clock_initialize(void)
{
    printf("Clock Initialization\n");
    set_frequency(FREQUENCY);
}

// Ustala zadaną częstotliwość
void set_frequency(unsigned int f)
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