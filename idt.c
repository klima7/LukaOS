#include <stddef.h>
#include <stdint.h>
#include "idt.h"
#include "ports.h"
#include "clock.h"
#include "keyboard.h"
#include "clib/stdio.h"

// Funkcje statyczne
static void set_IDT_entry(struct IDT_entry *entry, uint32_t offset, uint16_t selector, uint8_t type);

// Funkcja tworzy pojedyńczy wpis w tablicy IDT o podanych parametrach
static void set_IDT_entry(struct IDT_entry *entry, uint32_t offset, uint16_t selector, uint8_t type)
{
    entry->offset_lowerbits = offset & 0xFFFF;
    offset >>= 16;
    entry->offset_higherbits = offset & 0xFFFF;
    entry->zero = 0;
    entry->type_attr = type;
    entry->selector = selector;
}

//Inicjuje obsługe przerwań
void IDT_initialize(void)
{
    // Trzeba przepamować w przyszłości PIC
    
    printf("Creating IDT Table\n");
    set_IDT_entry(IDT +  0, (uint32_t)IRQ00, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT +  1, (uint32_t)IRQ01, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT +  2, (uint32_t)IRQ02, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT +  3, (uint32_t)IRQ03, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT +  4, (uint32_t)IRQ04, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT +  5, (uint32_t)IRQ05, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT +  6, (uint32_t)IRQ06, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT +  7, (uint32_t)IRQ07, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT +  8, (uint32_t)IRQ08, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT +  9, (uint32_t)IRQ09, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT + 10, (uint32_t)IRQ10, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT + 11, (uint32_t)IRQ11, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT + 12, (uint32_t)IRQ12, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT + 13, (uint32_t)IRQ13, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT + 14, (uint32_t)IRQ14, CODE_SELECTOR, TYPE);
    set_IDT_entry(IDT + 15, (uint32_t)IRQ15, CODE_SELECTOR, TYPE);

    printf("Setting IDTR Register\n");
    uint32_t addres = (uint32_t)IDT;
    uint16_t size = (uint16_t)(sizeof(IDT)*256-1);
    set_IDT(addres, size);
}

// Włącza obsługę przerwań
void enable_interrupts(void)
{
    asm("sti");
}

// Wyłącza obsługę przerwań
void disable_interrupts(void)
{
    asm("sti");
}

// 0x20 - Port głównego kontrolera przerwań PIC
// 0xA0 - Port podrzędnego kontrolera przerwań PIC
// Przerwania IRQ 0-7 są obsługiwane w całości przez głowny kontroler
// Przerwanie IRQ 8-15 są obsługiwane przez oba kontrolery (kaskada)
void IRQ_end_notify(unsigned int nr)
{
    if(nr<8)
    {
        outportb(0x20, 0x20);
    }
    else
    {
          outportb(0xA0, 0x20);
          outportb(0x20, 0x20);
    }
}

void IRQ00_handler(void) {
    printf("Przerwanie 0\n");
    IRQ_end_notify(0);
}
 
void IRQ01_handler(void) {
    printf("Przerwanie 1\n");
	IRQ_end_notify(1);
}
 
void IRQ02_handler(void) {
    printf("Przerwanie 2\n");
    IRQ_end_notify(2);
}
 
void IRQ03_handler(void) {
    printf("Przerwanie 3\n");
    IRQ_end_notify(3);
}
 
void IRQ04_handler(void) {
    printf("Przerwanie 4\n");
    IRQ_end_notify(4);
}
 
void IRQ05_handler(void) {
    printf("Przerwanie 5\n");
    IRQ_end_notify(5);
}
 
void IRQ06_handler(void) {
    printf("Przerwanie 6\n");
    IRQ_end_notify(6);
}
 
void IRQ07_handler(void) {
    printf("Przerwanie 7\n");
    IRQ_end_notify(7);
}
 
// Przerwanie zegara
void IRQ08_handler(void) {
    clock_interrupt_handler();
    IRQ_end_notify(8);
}
 
 // Przerwanie klawiatury
void IRQ09_handler(void) {
    keyboard_interrupt_handler();
    IRQ_end_notify(9);
}
 
void IRQ10_handler(void) {
    printf("Przerwanie 10\n");
    IRQ_end_notify(10);
}
 
void IRQ11_handler(void) {
    printf("Przerwanie 11\n");
    IRQ_end_notify(11);
}
 
void IRQ12_handler(void) {
    printf("Przerwanie 12\n");
    IRQ_end_notify(12);
}
 
void IRQ13_handler(void) {
    printf("Przerwanie 13\n");
    IRQ_end_notify(13);
}
 
void IRQ14_handler(void) {
    printf("Przerwanie 14\n");
    IRQ_end_notify(14);
}
 
void IRQ15_handler(void) {
    printf("Przerwanie 15\n");
    IRQ_end_notify(15);
}
