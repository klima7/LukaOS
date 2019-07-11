#ifndef __GDT_H__
#define __GDT_H__

#include <stddef.h>
#include <stdint.h>

#define GDT_SIZE 4

// Struktura opisująca sektor
struct segment
{
    uint32_t base;
    uint32_t limit; 
    uint8_t access;
    uint8_t flags;
};

// Pojedyńczy wpis w tablicy GDT
struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t limit_high : 4;
    uint8_t flags : 4;
    uint8_t base_high;
};

// Struktury opisujące sektory
struct segment segments[GDT_SIZE];

// Globalna tablica deskryptorów
struct gdt_entry GDT[GDT_SIZE];

// Prototypy
void GDT_initialize(void);
extern void set_GDT(uint32_t a, uint16_t b);
extern void reload_segments(void);
void encode_GDT_entry(struct gdt_entry *target, struct segment *source);
void create_GDT(void);

#endif