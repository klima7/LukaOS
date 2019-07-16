#ifndef __GDT_H__
#define __GDT_H__

#include <stddef.h>
#include <stdint.h>

#define GDT_SIZE 4

// Możliwe wartości bitów flagi
#define FLAGS_32BIT_MODE 0x4
#define FLAGS_4KB_GRANULARITY 0x8

// Możliwe wartości bitów flagi dostępu
#define ACCESS_PRESENT 0x80
#define ACCESS_RING0 0x00
#define ACCESS_RING1 0x20
#define ACCESS_RING2 0x40
#define ACCESS_RING3 0xC0
#define ACCESS_CD_SEGMENT 0x10
#define ACCESS_EXECUTABLE 0x08
#define ACCESS_DIRECTION 0x04
#define ACCESS_READ_WRITE 0x02
#define ACCESS_ACCESSED 0x01


// Struktura opisująca sektor
struct segment_t
{
    uint32_t base;
    uint32_t limit; 
    uint8_t access;
    uint8_t flags;
};

// Pojedyńczy wpis w tablicy GDT
struct gdt_entry_t
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
struct segment_t segments[GDT_SIZE];

// Globalna tablica deskryptorów
struct gdt_entry_t GDT[GDT_SIZE];

// Prototypy
void GDT_initialize(void);
extern void set_GDT(uint32_t a, uint16_t b);
extern void reload_segments(void);
void encode_GDT_entry(struct gdt_entry_t *target, struct segment_t *source);
void create_GDT(void);

#endif