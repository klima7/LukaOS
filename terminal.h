#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//Stała wielkość terminalu
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define SCROLL_HEIGHT 256

// Adres bufora terminalu
#define TERMINAL_ADDRESS 0xB8000

// Liczba lini przewijana przy jednym obrocie kółka
#define TERMINAL_SCROLL_SPEED 2

// Porty wykrzystywane do sterowanie kursorem
#define CURSOR_PORT_1 0x3D4
#define CURSOR_PORT_2 0x3D5

// Kształt kursora
#define CURSOR_SCANLINE_START 13
#define CURSOR_SCANLINE_END 15

// Stale kolorow 
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

//Prototypy
void terminal_initialize(void);
void terminal_full_initialize(void);
void terminal_setcolor(uint8_t color);
uint8_t terminal_getcolor(void);
void terminal_putchar(char c);
void terminal_writestring(const char* data);
void terminal_clear(void);

void terminal_disable_cursor();
void terminal_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void terminal_update_cursor(int x, int y);
uint16_t terminal_get_cursor_position(void);
void terminal_cursor_apply(void);

#endif