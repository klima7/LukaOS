#ifndef __BEEPER_H__
#define __BEEPER_H__

#include <stddef.h>
#include <stdint.h>

// Definicje
#define BEEP_PORT 0x61
#define BEEP_ON 0x3

#define BEEP_FREQ 500
#define BEEP_LENGTH 200

// Prototypy
void beeper_initialize(void);
void beep(void);
void beep_start(uint32_t frequency);
void silent(void);

#endif