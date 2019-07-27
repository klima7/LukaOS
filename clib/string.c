#include <stddef.h>
#include <stdint.h>
#include "string.h"
#include "../heap.h"

// Zwraca długość napisu
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (*str++)
		len++;
	return len;
}

// Kopiuje obszar pamięci o długości len z source do dest
void memcpy(void *dest, const void *source, size_t len)
{
	// Kopiowanie po 8 bajtów
	uint64_t *d64 = (uint64_t*)dest;
	uint64_t *s64 = (uint64_t*)source;
	uint32_t count64 = len / 8;

	for(uint32_t i=0; i<count64; i++)
		*d64++ = *s64++;

	// Kopiowanie po 1 bajt
	uint8_t *d8 = (uint8_t*)d64;
	uint8_t *s8 = (uint8_t*)s64;
	uint32_t count8 = len % 8;

	for(uint32_t i=0; i<count8; i++) 
		*d8++ = *s8++;
}

// Alokuje obszar pamięci o odpowiedniej wielkości i zapisuje tam napis
char *strdup(const char *str)
{
	uint32_t len = strlen(str);
	char *ptr = (char*)kmalloc(len+1);
	memcpy(ptr, str, len+1);
	return ptr;
}

// Sprawdza, czy pierwsze size znaków napisów jest identycznych
int strncmp(const char *str1, const char *str2, size_t size)
{
	for(uint32_t i=0; i<(uint32_t)size; i++)
	{
		if(*str1 != *str2) return 1;
		str1++; str2++;
	}
	return 0;
}

// Sprawdza, czy napisy są identyczne
int strcmp(const char *str1, const char *str2)
{
	uint32_t len1 = strlen(str1);
	uint32_t len2 = strlen(str2);

	if(len1 != len2) return 1;
	return strncmp(str1, str2, len1);
}

// Dodaje do napisu dest n znaków z napisu source
char *strncat(char *dest, const char *source, size_t n)
{
	uint32_t len = strlen(dest);
	dest += len;

	for(uint32_t i=0; i<n; i++)
	{
		*dest = *source;
		if(source == 0) return dest;
		dest++; source++;
	}

	return dest;
}

// Dodaje na koniec napisu dest napis source
char *strcat(char *dest, const char *source)
{
	uint32_t len = strlen(source);
	return strncat(dest, source, len+1);
}

// Zwraca dlugosc liczby ze znakiem
int get_int_len(int val)
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

// Zwraca dlugosc liczby bez znaku
int get_ull_len(unsigned long long val)
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

// Str to ciąg kilku napisów, jeden po drugim, kończących się terminatorem, funkcja zwraca napis o numerze nr
// Funkcja przydatna do wydobywania poszczególnych tokenów z argv
const char *get_token(const char *str, uint32_t nr)
{
	for(uint32_t i=0; i<nr; i++)
	{
		uint32_t len = strlen(str);
		str += len + 1;
	}

	return str;
}