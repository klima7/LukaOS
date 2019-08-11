#include <stddef.h>
#include <stdint.h>
#include "idt.h"
#include "ports.h"
#include "clock.h"
#include "keyboard.h"
#include "mouse.h"
#include "pic.h"
#include "clib/stdio.h"

// Funkcje statyczne
static void set_IDT_entry(struct IDT_entry *entry, uint32_t offset, uint16_t selector, uint8_t type);

// Tablica deskryptorów przerwań
struct IDT_entry IDT[256];

struct handlers_list handlers[256];

//Inicjuje obsługe przerwań
void IDT_initialize(void)
{
    // Przemapowuje PIC - tak aby przerwania nie pokrywały się z wyjątkami
    PIC_remap(32, 40);
    
    printf("Creating IDT Table\n");

    // Tablica pomocnicza ułatwiająca stworzenie tablicy IDT
    void (*ints[INTS_COUNT])(void) = { 
        int0,  int1,  int2,   int3,  int4, int5,  int6,  int7,  int8,  int9,  int10, int11, int12, int13, int14, int15, 
        int16, int17, int18, int19, int20, int21, int22, int23, int24, int25, int26, int27, int28, int29, int30, int31, 
        int32, int33, int34, int35, int36, int37, int38, int39, int40, int41, int42, int43, int44, int45, int46, int47, 
    };

    // Tworzy tablice IDT
    for(uint32_t i=0; i<INTS_COUNT; i++)
        set_IDT_entry(IDT+i, (uint32_t)*(ints+i), CODE_SELECTOR, TYPE);

    // Ustawia rejestr IDTR
    printf("Setting IDTR Register\n");
    uint32_t addres = (uint32_t)IDT;
    uint16_t size = (uint16_t)(sizeof(IDT)*256-1);
    set_IDT(addres, size);
}

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

// Kojarzy przekazaną funkcję z przerwaniem o podanym numerze
void interrupt_register(uint32_t interrupt_nr, HANDLER_FUN fun)
{
    uint32_t count = handlers[interrupt_nr].count;
    if(count == MAX_HANDLERS_COUNT) return;

    handlers[interrupt_nr].funs[count] = fun;
    handlers[interrupt_nr].count++;
}

// Funkcja wywołuje wszystkie funkcje skojarzone z danym przerwaniem
void interrupt_handler(uint32_t nr)
{
    if(nr!=32 && nr!=44 && nr!=33 && nr!=43)
        printf("[INT %u]\n", nr);

    uint32_t count = handlers[nr].count;

    for(uint32_t i=0; i<count; i++)
    {
        HANDLER_FUN handler = handlers[nr].funs[i];
        handler();
    }

    PIC_end_notify(nr);
}
