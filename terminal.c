#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "terminal.h"
#include "kernel.h"
#include "idt.h"
#include "mouse.h"
#include "ports.h"
#include "shell.h"
#include "clib/math.h"
#include "clib/string.h"
#include "clib/stdio.h"

//Funkcje statyczne
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
static inline uint16_t vga_entry(unsigned char uc, uint8_t color);
static inline void terminal_put_entry_at(uint16_t*base, char c, uint8_t color, size_t x, size_t y);
static void terminal_putnewline(void);
static void reprint(void);
static void terminal_scroll_handler(void);
static void terminal_move_up();
static void terminal_command_clear(const char* tokens, uint32_t tokens_count);

// Podstawowe zmienne terminalu
int32_t terminal_row = 0;
int32_t terminal_column = 0;
uint8_t terminal_color = 0;
uint16_t* terminal_buffer = (uint16_t*)TERMINAL_ADDRESS;

// Dodatkowy bufor potrzebny dla przewijania
uint16_t terminal_space[VGA_WIDTH * SCROLL_HEIGHT] = {0};

// Aktualna ozycja okna przesównego
int32_t terminal_window_position = 0;

// Przechowujemy ostatnią pozycję kółka myszy by wykryć zmianę
int32_t last_wheel = 0;

// Pierwsza część inicjalizacji terminalu, by od początku móc wyświetlać wiadomości
void terminal_initialize(void) 
{
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_clear();
	terminal_enable_cursor(CURSOR_SCANLINE_START, CURSOR_SCANLINE_END);
	terminal_update_cursor(0, 0);
	printf("Terminal Initialization\n");
}

// Druga część inicjalizacji terminalu, koniecznie po zainicjowaniu IDT oraz myszki
void terminal_full_initialize(void)
{
	interrupt_register(44, terminal_scroll_handler);
	last_wheel = get_mouse_info()->scroll_wheel;
	register_command("clear", "Clear screen", terminal_command_clear);
	printf("Terminal Full Initialization\n");
}

// Czyści terminal
void terminal_clear(void)
{
    terminal_row = 0;
	terminal_column = 0;
	terminal_window_position = 0;
                                           
	for (uint32_t y = 0; y < SCROLL_HEIGHT; y++) {
		for (uint32_t x = 0; x < VGA_WIDTH; x++) {
			uint32_t index = y * VGA_WIDTH + x;
			terminal_space[index] = vga_entry(' ', terminal_color);
		}
	}

	reprint();
}

// Drukuje tyle nowych lini, aby bierzący ekran stał się czysty
void terminal_fake_clear(void)
{
	for(uint32_t i=0; i<VGA_HEIGHT; i++)
		putchar('\n');

	terminal_cursor_apply();
}

//Drukuje jeden znak, trzeba wymyśleć coś szybszego! 
void terminal_putchar(char c) 
{
	// Przenosi wszystko o linie wyżej, gdy w dodatkowym buforze skończyło się miejsce
	// Można by tego uniknąc implementując go tak aby kolejne wiersze były zapisywane cyklicznie
	if(terminal_row == SCROLL_HEIGHT)
	{
		terminal_move_up();
		terminal_row--;
	}

	// Czy trzeba przekopiowywać całe okno?
	int reprint_flag = 0;

	// W czasie pisania konsola ma być przewinięta na sam dół
	if(terminal_window_position != MAX(terminal_row - VGA_HEIGHT + 1, 0))
	{
		terminal_window_position = MAX(terminal_row - VGA_HEIGHT + 1, 0);
		reprint_flag = 1;
	}

	if(c=='\n')
		terminal_putnewline();
	else 
	{
		terminal_put_entry_at(terminal_space, c, terminal_color, terminal_column, terminal_row);
		terminal_put_entry_at(terminal_buffer, c, terminal_color, terminal_column, terminal_row-terminal_window_position);
	}

	terminal_column++;
	if (terminal_column == VGA_WIDTH) 
	{
		terminal_column = 0;
		terminal_row++;

		if(terminal_row >= VGA_HEIGHT)
		{
			terminal_window_position++;
			reprint_flag = 1;
		}
	}

	// W razie konieczności przekopiowuje całe okno
	if(reprint_flag) reprint();

	// Aktualizacja kursora
	if(terminal_row - terminal_window_position > VGA_HEIGHT)
		terminal_disable_cursor();
	else
		terminal_enable_cursor(CURSOR_SCANLINE_START, CURSOR_SCANLINE_END);

	terminal_update_cursor(terminal_column, terminal_row - terminal_window_position);
}

//Przesówa wszystkie linie o jedną do góry
static void terminal_move_up()
{
	// Przesówa
    for(uint32_t y=1; y<=SCROLL_HEIGHT-1; y++)
    {
        for(uint32_t x=0; x<VGA_WIDTH; x++)
        {
            uint32_t index_source = y * VGA_WIDTH + x;
            uint32_t index_destination = (y-1) * VGA_WIDTH + x;
            *(terminal_space+index_destination) = *(terminal_space+index_source);
        }
    }
    // Czyści ostatnia linie
    for(uint32_t x=0; x<VGA_WIDTH; x++)
    {
        uint32_t index = (SCROLL_HEIGHT-1) * VGA_WIDTH + x;
        *(terminal_space+index) = vga_entry(' ', terminal_color);
    }
}

// Drukuje napis w terminalu
void terminal_writestring(const char* data) 
{
	for (; *data != 0; data++)
		terminal_putchar(*data);
}

// Kopiuje/Odciska odpowiedni fragment z pamięci terminala do jego bufora wyjściowego
static void reprint(void)
{
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			uint32_t index_from = (y+terminal_window_position)*VGA_WIDTH+x;
			uint32_t index_to = y*VGA_WIDTH+x;
			terminal_buffer[index_to] = terminal_space[index_from];
		}
	}
}

// Funkcja wywoływana przy akcji myszką
static void terminal_scroll_handler(void)
{
	int32_t wheel = get_mouse_info()->scroll_wheel;

	if(wheel > last_wheel && terminal_window_position + VGA_HEIGHT <= terminal_row)
	{
		terminal_window_position += MIN(TERMINAL_SCROLL_SPEED, terminal_row - terminal_window_position - VGA_HEIGHT + 1);
		reprint();
	}
	else if(wheel < last_wheel && terminal_window_position > 0) 
	{
		terminal_window_position -= MIN(TERMINAL_SCROLL_SPEED, terminal_window_position);
		reprint();
	}	

	last_wheel = wheel;
}
 
// Ustawia bierzący kolor tla i tekstu
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

// Zwraca aktualny kolor tła i tekstu
uint8_t terminal_getcolor(void)
{
	return terminal_color;
}
 
// Umieszcza znak na danej pozycji w danym buforze
static inline void terminal_put_entry_at(uint16_t*base, char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	base[index] = vga_entry(c, color);
}

// Dodaje nową linię
static void terminal_putnewline(void)
{
	terminal_column = VGA_WIDTH-1;
}

 // Tworzy wpis koloru 4B kolor 4B tlo
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
// Tworzy wpis do buforu tekstowego 8B znak 8B kolor 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

// Wyłącza wyświetlanie kursora
void terminal_disable_cursor()
{
	outportb(CURSOR_PORT_1, 0x0A);
	outportb(CURSOR_PORT_2, 0x20);
}

// Włącza wyświetlanie kursora i ustala jego pozycje w obrębie danego pola
void terminal_enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outportb(CURSOR_PORT_1, 0x0A);
	outportb(CURSOR_PORT_2, (inportb(CURSOR_PORT_2) & 0xC0) | cursor_start);
 
	outportb(CURSOR_PORT_1, 0x0B);
	outportb(CURSOR_PORT_2, (inportb(CURSOR_PORT_2) & 0xE0) | cursor_end);
}

// Umieszcza kursor w podanym miejscu
void terminal_update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;
 
	outportb(CURSOR_PORT_1, 0x0F);
	outportb(CURSOR_PORT_2, (uint8_t) (pos & 0xFF));
	outportb(CURSOR_PORT_1, 0x0E);
	outportb(CURSOR_PORT_2, (uint8_t) ((pos >> 8) & 0xFF));
}

// Zwraca aktualną pozycję kursora
uint16_t terminal_get_cursor_position(void)
{
    uint16_t pos = 0;
    outportb(CURSOR_PORT_1, 0x0F);
    pos |= inportb(CURSOR_PORT_2);
    outportb(CURSOR_PORT_1, 0x0E);
    pos |= ((uint16_t)inportb(CURSOR_PORT_2)) << 8;
    return pos;
}

// Aktualizacja kursora, robienie tego po każdym znaku trałoby za długo
void terminal_cursor_apply(void)
{
	if(terminal_row - terminal_window_position > VGA_HEIGHT)
		terminal_disable_cursor();
	else
		terminal_enable_cursor(CURSOR_SCANLINE_START, CURSOR_SCANLINE_END);

	terminal_update_cursor(terminal_column, terminal_row - terminal_window_position);
}

// Usówa ostatnio zapisane znaki
void terminal_undo_char(uint32_t count)
{
	for(uint32_t i=0; i<count; i++)
	{
		// Cofnięcie o jeden znak
		if(terminal_column == 0) 
		{
			terminal_column = VGA_WIDTH-1;
			terminal_row--;
		}
		else terminal_column--;

		putchar(' ');

		// Znowu cofnięcie o jeden znak
		if(terminal_column == 0) 
		{
			terminal_column = VGA_WIDTH-1;
			terminal_row--;
		}
		else terminal_column--;
	}

	terminal_cursor_apply();
}

// Komenda clear
static void terminal_command_clear(const char* tokens, uint32_t tokens_count)
{
	terminal_fake_clear();
}