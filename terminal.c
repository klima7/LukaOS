#include "terminal.h"
#include "kernel.h"
#include "clib/string.h"
#include "clib/stdio.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//Funkcje statyczne
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
static inline uint16_t vga_entry(unsigned char uc, uint8_t color);
static void terminal_autoscroll();

//Tworzy wpis koloru 4B kolor 4B tlo
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
//Tworzy wpis do buforu tekstowego 8B znak 8B kolor 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

//Czyści terminal
void terminal_clear(void)
{
    terminal_row = 0;
	terminal_column = 0;

	terminal_buffer = (uint16_t*) 0xB8000;                                             
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

//Inicjalizuje terminal
void terminal_initialize(void) 
{
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_clear();
	printf("Terminal initialization\n");
}
 
//Ustawia bierzący kolor tla i tekstu
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
//Umieszcza znak na danej pozycji
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
//Dodaje nowa linie
void terminal_putnewline(void)
{
	terminal_column = VGA_WIDTH-1;
}

//Drukuje jeden znak
void terminal_putchar(char c) 
{
	if(c=='\n')
		terminal_putnewline();
	else
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_autoscroll();
	}
}

//Przesówa bufor konsoli o linie do góry
static void terminal_autoscroll()
{
    for(size_t y=1; y<=VGA_HEIGHT-1; y++)
    {
        for(size_t x=0; x<VGA_WIDTH; x++)
        {
            const size_t index_source = y * VGA_WIDTH + x;
            const size_t index_destination = (y-1) * VGA_WIDTH + x;
            *(terminal_buffer+index_destination) = *(terminal_buffer+index_source);
        }
    }
    //Czyści ostatnia linie
    for(size_t x=0; x<VGA_WIDTH; x++)
    {
        const size_t index = (VGA_HEIGHT-1) * VGA_WIDTH + x;
        *(terminal_buffer+index) = vga_entry(' ', terminal_color);
    }
    terminal_row--;
}
 
//Drukuje napis o podanej danej dlugosci
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
//Drukuje napis bez podawanie dlugosci
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}