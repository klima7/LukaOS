#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "stdio.h"
#include "../terminal.h"
#include "string.h"
#include "math.h"

//Funkcje statyczne
static int get_int_len(int val);
static int get_ull_len(unsigned long long val);
static int display_int(int val, int limit);
static int display_float(double val);
static int display_ull(unsigned long long val);
static int display_binary(unsigned long long number, int bits);
static int display_hex(unsigned long long number, int bits);

//Drukuje znak na standardowym wyjściu
void putchar(char c)
{
    terminal_putchar(c);
	terminal_cursor_apply();
}

//Drukuje napis na standardowym wyjściu
void puts(const char *s)
{
    terminal_writestring(s);
	terminal_cursor_apply();
}

// Wyświetla napis na środku konsoli
void display_center(const char *str)
{
	uint32_t spaces = VGA_WIDTH/2 - strlen(str)/2;
	for(uint32_t i=0; i<spaces; i++) printf(" ");
	puts(str);
	puts("\n");
}

// Wyświetla napis wyrównany do prawej strony
void display_right(const char *str)
{
	uint32_t spaces = VGA_WIDTH - strlen(str);
	for(uint32_t i=0; i<spaces; i++);
	puts(str);
	puts("\n");
}

//Zwraca dlugosc liczby ze znakiem
static int get_int_len(int val)
{
	if (val == 0) return 1;
	if (val == 10) return 2;

	int len = 0;
	while (val != 0)
	{
		len++;
		val /= 10;
	}
	return len;
}

//Zwraca dlugosc liczby bez znaku
static int get_ull_len(unsigned long long val)
{
	if (val == 0) return 1;
	if (val == 10) return 2;

	int len = 0;
	while (val != 0)
	{
		len++;
		val /= 10;
	}
	return len;
}

//Wyswietla liczbe ze znakiem
static int display_int(int val, int limit)
{
	int length = 0;

	int abs = (int)fabs(val);
	int len = get_int_len(abs);

	if (val < 0)
	{
		terminal_putchar('-');
		length++;
	}

	int weight = (int)pow(10, len - 1);

	for(int i=0; i<len; i++)
	{
		int digit = abs / weight;
		abs -= digit * weight;
		terminal_putchar('0' + digit);
		length++;
		weight /= 10;

		if (length == limit) 
			return length;
	}

	return length;
}

//Wyświetla liczbe zmiennoprzecinkową
static int display_float(double val)
{
	int len = 0;

	//Czesc calkowita
	int a = (int)val;
	len += display_int(a, -1);
	terminal_putchar('.');
	len++;

	double r = val - a;
	int b = (int)fabs((int)(r * 1000000));

	//Zera przed
	int len_temp = get_int_len(b);
	int zero_before = 5 - len_temp;
	if (zero_before < 0) zero_before = 0;

	for (int i = 0; i < zero_before; i++)
	{
		terminal_putchar('0');
		len++;
	}

	len_temp = display_int(b, 5);
	int zero_after = 5 - zero_before - len_temp;
	len += len_temp;

	//Zera po
	for (int i = 0; i < zero_after; i++)
	{
		terminal_putchar('0');
		len++;
	}

	return len;
}

//Wyświetla liczbe bez znaku
static int display_ull(unsigned long long val)
{
	int length = 0;

	int len = get_ull_len(val);

	unsigned long long weight = ullpow(10, len - 1);

	for(int i=0; i<len; i++)
	{
		unsigned long long digit = val / weight;
		val -= digit * weight;
		terminal_putchar('0' + digit);
		length++;
		weight /= 10;
	}

	return length;
}

//Wyświetla liczbe w postaci binarnej
static int display_binary(unsigned long long number, int bits)
{
	int len = 0;

	for (int i = 0; i < bits; i++)
	{
		unsigned long long mask = 1ull << (bits-1-i);
		int bit = !(!(mask & number));
		terminal_putchar('0'+bit);
		len++;
		if ((i + 1) % 8 == 0) 
		{
			terminal_putchar(' ');
			len++;
		}
	}

	return len;
}

//Wyświetla liczbe w postaci szesnastkowej
static int display_hex(unsigned long long number, int bits)
{
	int len = 0;

	unsigned long long mask = 0xFu << (bits-4);

	for(int i=bits-4; i>=0; i-=4)
	{
		unsigned long long result = number & mask;
		result >>= i;
		if(result >=10)
			terminal_putchar('A'+result-10);
		else 
			terminal_putchar('0'+result);
		len++;
		mask >>= 4;
	}

	return len;
}

//Wyswietla sformatowany napis
int printf(char* sheme, ...)
{
	if (sheme == NULL) return 0;

	int success = 0;

	va_list lista;
	va_start(lista, sheme);

	while (*sheme != 0)
	{
		if (*sheme != '%')
		{
			terminal_putchar(*sheme);
			success++;
		}
		else
		{
			sheme++;
			if (*sheme == 's')
			{
				char* str = va_arg(lista, char*);
				puts(str);
				success+=strlen(str);
			}
			else if (*sheme == 'd')
			{
				int num = va_arg(lista, int);
				success += display_int(num, -1);
			}
			else if (*sheme == 'f')
			{
				double num = (double)va_arg(lista, double);
				success += display_float(num);
			}
			else if (*sheme == 'l' && *(sheme+1)=='f')
			{
				sheme++;
				double num = (double)va_arg(lista, double);
				success += display_float(num);
			}
			else if (*sheme == 'c')
			{
				char c = (char)va_arg(lista, int);
				terminal_putchar(c);
				success += 1;
			}
			else if (*sheme == 'u')
			{
				unsigned long long val = (unsigned long long)va_arg(lista, unsigned int);
				success += display_ull(val);
			}
			else if(*sheme == 'l' && *(sheme+1)=='u')
			{
				sheme++;
				unsigned long long val = (unsigned long)va_arg(lista, unsigned long);
				success += display_ull(val);
			}
			else if(*sheme == 'l' && *(sheme+1)=='l' && *(sheme+2)=='u')
			{
				sheme+=2;
				unsigned long long val = (unsigned long long)va_arg(lista, unsigned long long);
				success += display_ull(val);
			}
			else if (*sheme == 'b')
			{
				unsigned long long val = (unsigned long long)va_arg(lista, unsigned int);
				success += display_binary(val, sizeof(unsigned int)*8);
			}
			else if (*sheme == 'l' && *(sheme+1)=='b')
			{
				sheme++;
				unsigned long long val = (unsigned long)va_arg(lista, unsigned long);
				success += display_binary(val, sizeof(unsigned long)*8);
			}
			else if (*sheme == 'l' && *(sheme+1)=='l' && *(sheme+2)=='b')
			{
				sheme+=2;
				unsigned long long val = (unsigned long long)va_arg(lista, unsigned long long);
				success += display_binary(val, sizeof(unsigned long long)*8);
			}
			else if (*sheme == 'x')
			{
				unsigned long long val = (unsigned long long)va_arg(lista, unsigned int);
				success += display_hex(val, sizeof(unsigned int)*8);
			}
			else if (*sheme == 'l' && *(sheme+1)=='x')
			{
				sheme++;
				unsigned long long val = (unsigned long)va_arg(lista, unsigned long);
				success += display_hex(val, sizeof(unsigned long)*8);
			}
			else if (*sheme == 'l' && *(sheme+1)=='l' && *(sheme+2)=='x')
			{
				sheme+=2;
				unsigned long long val = (unsigned long long)va_arg(lista, unsigned long long);
				success += display_hex(val, sizeof(unsigned long long)*8);
			}
			else
			{
				va_end(lista);
				return success;
			}
		}
		sheme++;
	}

	va_end(lista);

	terminal_cursor_apply();
	return success;
}

