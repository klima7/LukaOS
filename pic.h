#ifndef __PIC_H__
#define __PIC_H__

#include <stddef.h>
#include <stdint.h>

// Adresy portów
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
 
// Możliwe opcje trzeciego bajtu konfiguracyjnego PIC
#define ICW4_8086 0x01		
#define ICW4_AUTO 0x02		
#define ICW4_BUF_SLAVE 0x08		
#define ICW4_BUF_MASTER	0x0C		
#define ICW4_SFNM 0x10		

// Komendy
#define COMMAND_IRQ_END 0x20
#define COMMAND_READ_IRR 0x0A
#define COMMAND_READ_ISR 0x0B

// Prototypy
void PIC_remap(int offset1, int offset2);
void PIC_set_mask(uint8_t irq);
void PIC_clear_mask(uint8_t irq);
void PIC_end_notify(uint8_t irq);
uint16_t PIC_get_isr(void);
uint16_t PIC_get_irr(void);

#endif