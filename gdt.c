#include <stddef.h>
#include <stdint.h>
#include "gdt.h"
#include "kernel.h"
#include "clib/stdio.h"

void GDT_initialize(void)
{
    // Inicjuje struktury reprezentujące segmenty
    printf("Creating GDT Table\n");
	segments[0] = (struct segment) { .base=0, .limit=0,          .access=0,    .flags=0x4 };         // Niedostępny fragmęt pamięci, NULL
	segments[1] = (struct segment) { .base=0, .limit=0xFFFFFFFF, .access=0x9A, .flags=0xC };         // Obszar pamięci na kod, wykonywalny, nie można zapisywać
	segments[2] = (struct segment) { .base=0, .limit=0xFFFFFFFF, .access=0x92, .flags=0xC };         // Obszar pamięci na dane, nie można wykonywać, możne zapisywać i odczytywać
	segments[3] = (struct segment) { .base=0, .limit=0,          .access=0,    .flags=0x4 }; 	     // Obszar na przyszlość, dla wielozadaniowości

    // Tworzy Globalną tablice deskryptorów w oparciu o segmenty
    for(int i=0; i<GDT_SIZE; i++)
        encode_GDT_entry(GDT+i, segments+i);

    // Ładuje selektory do rejestrów segmentowych
    printf("Loading Selectors into Segment Registers\n");
    reload_segments();

    // Ładuje tablice deskryptorów do rejestru GDR
    printf("Setting GDTR Register\n");
    set_GDT((uint32_t)GDT, sizeof(GDT)-1);
}

//Zamienia strukture segmentu na wpis w tablicy GDT
void encode_GDT_entry(struct gdt_entry *target, struct segment *source)
{
    // Ustawienie flag
    target->flags = source->flags;
 
    //Zakodowanie limitu sektora
    target->limit_low  = source->limit & 0x0000FFFF;
    target->limit_high = source->limit & 0x00FF0000;
 
    //Zakodowanie offsetu sektora
    target->base_low = source->base & 0x0000FFFF;
    target->base_middle = source->base & 0x00FF0000;
    target->base_high = source->base & 0xFF000000;
 
    //Zakodowanie praw dostępu sektora
    target->access = source->access;
}
