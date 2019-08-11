#include <stddef.h>
#include <stdint.h>
#include "network.h"
#include "utils.h"
#include "../rng.h"
#include "../time.h"
#include "../clib/stdio.h"
#include "../terminal.h"
#include "../shell.h"
#include "../clib/string.h"

// Adresy
uint32_t ip = 0;
uint64_t mac = 0;

// Funkcje statyczne
static void network_init_ip_and_mac(void);
static void network_command_ipconfig(const char* tokens, uint32_t tokens_count);
static void network_command_setip(const char* tokens, uint32_t tokens_count);

// Inicjalizuje
void network_initialize(void)
{
    // Dodaje nowe komendy
    register_command("ipconfig", "Display IP and MAC address", network_command_ipconfig);
    register_command("setip", "Command sets new IP", network_command_setip);

    // Inicjuje adres MAC i IP
    network_init_ip_and_mac();
}

// Przypisuje losowo maszynie numer mac i ip 
static void network_init_ip_and_mac(void)
{
    srand(time_hash());
    ip = rand();
    mac = ((uint64_t)rand() << 16) + (uint64_t)rand();
}

// Komenda wyświetlające adres IP oraz MAC
static void network_command_ipconfig(const char* tokens, uint32_t tokens_count)
{
    printf("IP Address:    ");
    display_ip(network_get_ip());
    printf("\n");

    printf("MAC Address:   ");
    display_mac(network_get_mac());
    printf("\n");
}

// Komenda setip
static void network_command_setip(const char* tokens, uint32_t tokens_count)
{
    // Czy podano wszystkie argumenty
    if(tokens_count < 1)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Not enough arguments. You must enter new ip\n");
        return;
    }

    // Odczytuje adres IP
    char *ip_in_str = (char*)get_token(tokens, 1);
    int ok_flag = 0;
    uint32_t new_ip = str_to_ip(ip_in_str, &ok_flag);
    if(!ok_flag)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Invalid IP Format\n");
        return;
    }

    ip = new_ip;
    terminal_setcolor(VGA_COLOR_WHITE);
    printf("IP changed to ");
    terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
    display_ip(ip);
    printf("\n");
}

// Zwraca adres ip
uint32_t network_get_ip(void)
{
    return ip;
}

// Zwraca adres mac
uint64_t network_get_mac(void)
{
    return mac;
}