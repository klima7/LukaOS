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