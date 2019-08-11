#ifndef __STDIO_H__
#define __STDIO_H__

int display_binary(unsigned long long number, int bits);
int display_hex(unsigned long long number, int bits);

void gets(char *str, uint32_t len);
void gets_reset(void);
void simulate_typing(char *text);
void putchar(char c);
void puts(const char *s);
int printf(char* sheme, ...);
void display_center(const char *str);
void display_right(const char *str);

#endif