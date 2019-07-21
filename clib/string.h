#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>
#include <stdint.h>

//Prototypy
size_t strlen(const char* str);
char *strdup(const char *str);
void memcpy(void *dest, const void *source, size_t len);
int strncmp(const char *str1, const char *str2, size_t size);
char *strncat(char *dest, const char *source, size_t n);
char *strcat(char *dest, const char *source);
int get_int_len(int val);
int get_ull_len(unsigned long long val);

#endif