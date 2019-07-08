#ifndef __IDT_H__
#define __IDT_H__

#include <stddef.h>
#include <stdint.h>

#define CODE_SELECTOR 0x08
// TYPE = 1000 1110 ( present | type )
// present = 1000 (valid)
// type = 1110 (interrupt gate)
#define TYPE 0x8E

// Pojedyńczy wpis w tablicy deskryptorów przerwań
struct IDT_entry{
	uint16_t offset_lowerbits;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_higherbits;
};

// Tablica deskryptorów przerwań
struct IDT_entry IDT[256];

extern void set_IDT(uint32_t a, uint16_t b);

void IDT_initialize(void);
void IRQ_end_notify(unsigned int nr);

void enable_interrupts(void);
void disable_interrupts(void);

// Procedury obsługi przerwań assemblera
// Potrzebujemy ich adresów
extern int IRQ00(void);
extern int IRQ01(void);
extern int IRQ02(void);
extern int IRQ03(void);
extern int IRQ04(void);
extern int IRQ05(void);
extern int IRQ06(void);
extern int IRQ07(void);
extern int IRQ08(void);
extern int IRQ09(void);
extern int IRQ10(void);
extern int IRQ11(void);
extern int IRQ12(void);
extern int IRQ13(void);
extern int IRQ14(void);
extern int IRQ15(void);

void IRQ00_handler(void);
void IRQ01_handler(void);
void IRQ02_handler(void);
void IRQ03_handler(void);
void IRQ04_handler(void);
void IRQ05_handler(void);
void IRQ06_handler(void);
void IRQ07_handler(void);
void IRQ08_handler(void);
void IRQ09_handler(void);
void IRQ10_handler(void);
void IRQ11_handler(void);
void IRQ12_handler(void);
void IRQ13_handler(void);
void IRQ14_handler(void);
void IRQ15_handler(void);

#endif