#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>
#include <stdint.h>

//Prototypy
size_t strlen(const char* str);
char *strdup(const char *str);
void memcpy(void *dest, const void *source, size_t len);

#endif