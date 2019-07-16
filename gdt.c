#include <stddef.h>
#include <stdint.h>
#include "gdt.h"
#include "kernel.h"
#include "clib/stdio.h"

void GDT_initialize(void)
{
    // Inicjuje struktury reprezentujące segmenty
    printf("Creating GDT Table\n");

    // Segment niewykorzystywany
	segments[0] = (struct segment_t) 
    { 
        .base = 0, .limit = 0,          
        .access = 0,                                                                                         
        .flags = FLAGS_32BIT_MODE         
    };  

    // Segment kodu
	segments[1] = (struct segment_t) 
    { .base = 0, 
    .limit = 0xFFFFFFFF, 
    .access = ACCESS_PRESENT | ACCESS_RING0 | ACCESS_CD_SEGMENT | ACCESS_EXECUTABLE | ACCESS_READ_WRITE, 
    .flags = FLAGS_32BIT_MODE | FLAGS_4KB_GRANULARITY 
    };  

    // Segment danych
	segments[2] = (struct segment_t) 
    { 
        .base = 0, 
        .limit = 0xFFFFFFFF, 
        .access = ACCESS_PRESENT | ACCESS_RING0 | ACCESS_CD_SEGMENT | ACCESS_EXECUTABLE | ACCESS_READ_WRITE, 
        .flags = FLAGS_32BIT_MODE | FLAGS_4KB_GRANULARITY 
    }; 

    // TSS w przyszłości
	segments[3] = (struct segment_t) 
    { 
        .base = 0, 
        .limit = 0,          
        .access = 0,                                                                                         
        .flags = FLAGS_32BIT_MODE                         
    }; 

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
void encode_GDT_entry(struct gdt_entry_t *target, struct segment_t *source)
{
    // Ustawienie flag
    target->flags = source->flags;
 
    //Zakodowanie limitu sektora
    target->limit_low  = source->limit & 0x0000FFFF;
    target->limit_high = (source->limit & 0x000F0000) >> 16;
 
    //Zakodowanie offsetu sektora
    target->base_low = source->base & 0x0000FFFF;
    target->base_middle = (source->base & 0x00FF0000) >> 16;
    target->base_high = (source->base & 0xFF000000) >> 24;
 
    //Zakodowanie praw dostępu sektora
    target->access = source->access;
}
