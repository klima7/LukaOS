#include "gdt.h"
#include "kernel.h"
#include "clib/stdio.h"
#include <stddef.h>
#include <stdint.h>


void GDT_initialize(void)
{
    // Inicjuje struktury reprezentujące segmenty
    printf("Creating GDT Table\n");
	GDT_structs[0] = (struct GDT_ENTRY_STRUCT){.base=0, .limit=0, .type=0};                     // Niedostępny fragmęt pamięci, NULL
	GDT_structs[1] = (struct GDT_ENTRY_STRUCT){.base=0, .limit=0xffffffff, .type=0x9A};         // Obszar pamięci na kod, wykonywalny, nie można zapisywać
	GDT_structs[2] = (struct GDT_ENTRY_STRUCT){.base=0, .limit=0xffffffff, .type=0x92};         // Obszar pamięci na dane, nie można wykonywać, możne zapisywać i odczytywać
	GDT_structs[3] = (struct GDT_ENTRY_STRUCT){.base=0, .limit=0, .type=0}; 				    // Obszar na przyszlość, dla wielozadaniowości

    // Tworzy Globalną tablice deskryptorów w oparciu o segmenty
    for(int i=0; i<GDT_SIZE; i++)
    {
        uint8_t *gdt_entry_start = (uint8_t*)(GDT+i);
        encode_GDT_entry(gdt_entry_start, GDT_structs[i]);
    }

    // Ładuje selektory do rejestrów segmentowych
    printf("Loading Selectors into Segment Registers\n");
    reload_segments();

    // Ładuje tablice deskryptorów do rejestru GDR
    printf("Setting GDTR Register\n");
    set_GDT((uint32_t)GDT, sizeof(GDT)-1);
}

//Zamienia strukture sektora na wpis w tablicy GDT
void encode_GDT_entry(uint8_t *target, struct GDT_ENTRY_STRUCT source)
{
    //Sprawdzenie czy struktura może być skonwertowana
    if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF)) 
    {
        //Ewentualny obsluga błędu
    }

    //Dopasuj ziarnistość 1B/4kB
    if (source.limit > 65536) {
        source.limit = source.limit >> 12;
        target[6] = 0xC0;
    } else {
        target[6] = 0x40;
    }
 
    //Zakodowanie limitu sektora
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] |= (source.limit >> 16) & 0xF;
 
    //Zakodowanie offsetu sektora
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;
 
    //Zakodowanie typu sektora
    target[5] = source.type;
}
