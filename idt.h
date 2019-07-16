#ifndef __IDT_H__
#define __IDT_H__

#include <stddef.h>
#include <stdint.h>

#define CODE_SELECTOR 0x08
#define TYPE 0x8E

#define MAX_HANDLERS_COUNT 16
#define INTS_COUNT 48

// Wskaźnik na funkcję obsługi przerwania
typedef void(*HANDLER_FUN)(void);

// Struktura opisująca zbiór funkcji obsługujących dane przerwanie
struct handlers_list
{
	uint32_t count;
	HANDLER_FUN funs[MAX_HANDLERS_COUNT];
};

// Pojedyńczy wpis w tablicy deskryptorów przerwań
struct IDT_entry{
	uint16_t offset_lowerbits;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_higherbits;
};

// Prototypy
void IDT_initialize(void);
void interrupt_register(uint32_t interrupt_nr, HANDLER_FUN fun);
void interrupt_handler(uint32_t nr);

// Funkcje zewnętrzne
extern void set_IDT(uint32_t a, uint16_t b);

extern void int0(void);  extern void int1(void);  extern void int2(void);  extern void int3(void); 
extern void int4(void);  extern void int5(void);  extern void int6(void);  extern void int7(void); 
extern void int8(void);  extern void int9(void);  extern void int10(void); extern void int11(void); 
extern void int12(void); extern void int13(void); extern void int14(void); extern void int15(void); 
extern void int16(void); extern void int17(void); extern void int18(void); extern void int19(void); 
extern void int20(void); extern void int21(void); extern void int22(void); extern void int23(void); 
extern void int24(void); extern void int25(void); extern void int26(void); extern void int27(void); 
extern void int28(void); extern void int29(void); extern void int30(void); extern void int31(void); 
extern void int32(void); extern void int33(void); extern void int34(void); extern void int35(void); 
extern void int36(void); extern void int37(void); extern void int38(void); extern void int39(void); 
extern void int40(void); extern void int41(void); extern void int42(void); extern void int43(void); 
extern void int44(void); extern void int45(void); extern void int46(void); extern void int47(void); 

#endif