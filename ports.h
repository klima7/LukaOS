#ifndef __PORTS_H__
#define __PORTS_H__

#include <stddef.h>
#include <stdint.h>

extern uint8_t inportb(uint16_t port);
extern void outportb(uint16_t port, uint8_t value);

#endif