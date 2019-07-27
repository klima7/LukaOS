#ifndef __PORTS_H__
#define __PORTS_H__

#include <stddef.h>
#include <stdint.h>

// Pobiera/zapisuje jeben bajt do portu - byte
extern uint8_t inportb(uint16_t port);
extern void outportb(uint16_t port, uint8_t value);

// Pobiera/zapisuje dwa bajty do portu - word
extern uint16_t inportw(uint16_t port);
extern void outportw(uint16_t port, uint16_t value);

// Pobiera/zapisuje cztery bajty do portu - double word
extern uint32_t inportdw(uint16_t port);
extern void outportdw(uint16_t port, uint32_t value);

#endif