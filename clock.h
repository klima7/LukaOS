#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <stddef.h>
#include <stdint.h>

// Częstotliwość zegara
#define FREQUENCY 1000

// Prototypy
void clock_initialize(void);
void set_frequency(unsigned int f);
void clock_interrupt_handler(void);
void sleep(unsigned long long t);
unsigned long long clock(void);
double time(void);

// Procedury assemblerowe
extern unsigned short read_PIT_count(void);
extern void set_PIT_count(unsigned short count);

#endif