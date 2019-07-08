#ifndef __PORTS_H__
#define __PORTS_H__

extern unsigned char inportb(unsigned char port);
extern void outportb(unsigned char port, unsigned char value);

#endif