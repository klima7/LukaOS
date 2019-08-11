#include <stddef.h>
#include <stdint.h>
#include "utils.h"
#include "../clib/stdio.h"
#include "../clib/string.h"
#include "../clib/stdlib.h"

// Funkcje statyczne 
static uint64_t switch_lsb_msb(uint64_t val, uint8_t bytes);

// Zamienia liczbe big endian na little endian - network to host
uint64_t nths6(uint64_t val)
{
    return switch_lsb_msb(val, 6);
}

// Zamienia liczbe little endian na big endian - host to network
uint64_t hston6(uint64_t val)
{
    return switch_lsb_msb(val, 6);
}

// Zamienia liczbe big endian na little endian - network to host
uint32_t nths4(uint32_t val)
{
    return switch_lsb_msb(val, 4);
}

// Zamienia liczbe little endian na big endian - host to network
uint32_t hston4(uint32_t val)
{
    return switch_lsb_msb(val, 4);
}

// Zamienia liczbe big endian na little endian - network to host
uint16_t nths2(uint16_t val)
{
    return switch_lsb_msb(val, 2);
}

// Zamienia liczbe little endian na big endian - host to network
uint16_t hston2(uint16_t val)
{
    return switch_lsb_msb(val, 2);
}

// Nie nie robi
uint8_t nths1(uint8_t val)
{
    return val;
}

// Nic nie robi
uint8_t hston1(uint8_t val)
{
    return val;
}

// Dokonuje konwersji little endian <-> big endian nax 8 bitowej liczby
static uint64_t switch_lsb_msb(uint64_t val, uint8_t bytes)
{
    uint64_t ret_val = val;

    uint8_t *byte1 = (uint8_t*)&ret_val;
    uint8_t *byte2 = (uint8_t*)&ret_val + bytes-1;

    for(uint8_t i=0; i<bytes/2; i++)
    {
        uint8_t temp = *byte1;
        *byte1 = *byte2;
        *byte2 = temp;

        byte1++;
        byte2--;
    }

    return ret_val;
}

// Wyświetla odpowiednio sformatowany adres ip
void display_ip(uint32_t ip)
{
    uint32_t i1 = (ip & 0x000000FF) >> 0;
    uint32_t i2 = (ip & 0x0000FF00) >> 8;
    uint32_t i3 = (ip & 0x00FF0000) >> 16;
    uint32_t i4 = (ip & 0xFF000000) >> 24;

    printf("%d.%d.%d.%d", i1, i2, i3, i4);
}

// Wyświetla odpowiednio sformatowany adres mac
void display_mac(uint64_t mac)
{
    uint32_t m1 = (mac & 0x00000000000000FFull) >> 0;
    uint32_t m2 = (mac & 0x000000000000FF00ull) >> 8;
    uint32_t m3 = (mac & 0x0000000000FF0000ull) >> 16;
    uint32_t m4 = (mac & 0x00000000FF000000ull) >> 24;
    uint32_t m5 = (mac & 0x000000FF00000000ull) >> 32;
    uint32_t m6 = (mac & 0x0000FF0000000000ull) >> 40;

    display_hex(m6, 8);
    printf(":");
    display_hex(m5, 8);
    printf(":");
    display_hex(m4, 8);
    printf(":");
    display_hex(m3, 8);
    printf(":");
    display_hex(m2, 8);
    printf(":");
    display_hex(m1, 8);
}

// Zamienia napis na adres ip w formie liczby
uint32_t str_to_ip(char *str, int *ok)
{
    if(ok!=NULL) *ok = 1;
    int dots = 0;
    
    // Zliczania kropek
    char *temp = str;
    while(*temp!=0)
    {
        if(*temp=='.') dots++;
        temp++;
    }

    // Niepoprawny format adresu
    if(dots!=3)
    {
        if(ok!=NULL) *ok = 0;
        return 0;
    }

    uint32_t num1 = 0;
    uint32_t num2 = 0;
    uint32_t num3 = 0;
    uint32_t num4 = 0;

    // Pobieranie poszczególnych części
    char *next_dot = str;
    num1 = (uint32_t)atoi(next_dot);

    next_dot = strchr(next_dot, '.');
    next_dot++;
    num2 = (uint32_t)atoi(next_dot);

    next_dot = strchr(next_dot, '.');
    next_dot++;
    num3 = (uint32_t)atoi(next_dot);

    next_dot = strchr(next_dot, '.');
    next_dot++;
    num4 = (uint32_t)atoi(next_dot);

    // Składanie
    uint32_t ip = num1 | num2 << 8 | num3 << 16 | num4 << 24;

    return ip;
}

// Zamienia napis na adres ip
uint64_t str_to_mac(char *str, int *ok)
{
    if(ok!=NULL) *ok = 1;
    int colons = 0;
    
    // Zliczania dwukropków
    char *temp = str;
    while(*temp!=0)
    {
        if(*temp==':') colons++;
        temp++;
    }

    // Niepoprawny format adresu
    if(colons!=5)
    {
        if(ok!=NULL) *ok = 0;
        return 0;
    }

    uint64_t num1 = 0;
    uint64_t num2 = 0;
    uint64_t num3 = 0;
    uint64_t num4 = 0;
    uint64_t num5 = 0;
    uint64_t num6 = 0;

    // Pobieranie poszczególnych części
    char *next_part = str;
    num1 = (uint64_t)xtou(next_part);

    next_part = strchr(next_part, ':');
    next_part++;
    num2 = (uint32_t)xtou(next_part);

    next_part = strchr(next_part, ':');
    next_part++;
    num3 = (uint32_t)xtou(next_part);

    next_part = strchr(next_part, ':');
    next_part++;
    num4 = (uint32_t)xtou(next_part);

    next_part = strchr(next_part, ':');
    next_part++;
    num5 = (uint32_t)xtou(next_part);

    next_part = strchr(next_part, ':');
    next_part++;
    num6 = (uint32_t)xtou(next_part);

    uint64_t mac = num6 | num5 << 8 | num4 << 16 | num3 << 24 | num2 << 32 | num1 << 40;

    return mac;
}

// Liczy sumę kontrolną
uint16_t calculate_checksum(void *ptr, uint32_t len)
{
    uint16_t *ptr16 = (uint16_t*)ptr;
    uint32_t sum = 0;

    uint32_t full_words = len / 2;

    for(uint32_t i=0; i<full_words; i++)
    {
        sum += *ptr16;
        ptr16++;

        // Nastąpił overflow powyżej 2 bajty
        if((sum & 0xFFFF0000) != 0) 
        {
            sum |= 0x0000FFFF;
            sum += 1;
        }
    }

    // Czy udało ładnie się podzielić dane
    if(len%2 != 0)
    {
        uint8_t *last_byte = (uint8_t*)ptr16;
        uint16_t word = *last_byte;                     // Nie jestem pewny czy ten nieparzysty bajt powinien zostać mniej, czy bardziej znaczącym bajtem
        
        sum += word;

        // Nastąpił overflow powyżej 2 bajty
        if((sum & 0xFFFF0000) != 0) 
        {
            sum |= 0x0000FFFF;
            sum += 1;
        }
    }

    // Zwrócenie sumy kontrolnej
    return (uint16_t)sum;
}