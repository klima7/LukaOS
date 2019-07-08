#ifndef __GDT_H__
#define __GDT_H__

#include <stddef.h>
#include <stdint.h>

#define GDT_SIZE 4

//Struktura opisująca sektor
struct GDT_ENTRY_STRUCT
{
    uint32_t base;
    uint32_t limit; 
    int8_t type;
};

//Struktury opisujące sektory
struct GDT_ENTRY_STRUCT GDT_structs[GDT_SIZE];

//Globalna tablica deskryptorów
uint64_t GDT[GDT_SIZE];

//Prototypy
void GDT_initialize(void);
extern void set_GDT(uint32_t a, uint16_t b);
extern void reload_segments(void);
void encode_GDT_entry(uint8_t *target, struct GDT_ENTRY_STRUCT source);
void create_GDT(void);

#endif