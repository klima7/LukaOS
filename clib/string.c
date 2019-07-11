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
	uint8_t *d = (uint8_t*)dest;
	uint8_t *s = (uint8_t*)source;

	for(size_t i=0; i<len; i++) 
		*d++ = *s++;
}

// Alokuje obszar pamięci o odpowiedniej wielkości i zapisuje tam napis
char *strdup(const char *str)
{
	char *ptr = (char*)kmalloc(strlen(str)+1);
	unsigned int i = 0;
	for(; i<strlen(str); i++)
		*(ptr+i) = *(str+i);
	*(ptr+i) = 0;
	return ptr;
}