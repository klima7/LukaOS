#include <stddef.h>
#include <stdint.h>
#include "icmp.h"
#include "ipv4.h"
#include "network.h"
#include "utils.h"
#include "../shell.h"
#include "../clock.h"
#include "../clib/stdio.h"
#include "../clib/string.h"

// Gdy wysłano rządanie i oczekujemy na odpowiedz to ta flaga jest ustawiona
volatile int waiting_for_reply = 0;
volatile int receive_bytes_count = 0;

// Funkcje statyczne
static void icmp_command_ping(const char* tokens, uint32_t tokens_count);

// Inicjalizuje
void icmp_initialize(void)
{
    register_command("ping", "Command test connection with given host", icmp_command_ping);
}

// Tworzy ramke protokołu ICMP
void *icmp_create_frame(uint32_t size, uint8_t type, uint8_t code, uint32_t destination_ip, int *err)
{
    // Tworzy ramke
    int mac_err = 0;
    struct icmp_header *icmp_header = (struct icmp_header*)ipv4_create_frame(size+ICMP_HEADER_SIZE, 0, 0, 0, IPV4_PROTOCOL_ICMP, destination_ip, &mac_err);
    if(mac_err) 
    {
        *err = 1;
        return NULL;
    }

    icmp_header->type = type;
    icmp_header->code = code;
    icmp_header->checksum = 0;
    
    // Zmienia format pól z Little Endian na Big Endian
    icmp_switch_header_lsb_msb(icmp_header);

    // Zwraca wskaźnik do zawartości
    return icmp_header+1;
}

// Zmienia format pól nagłówka ramki ICMP z Little Endian na Big Endian i odwrotnie
void icmp_switch_header_lsb_msb(struct icmp_header *icmp_header)
{
    icmp_header->checksum = hston2(icmp_header->checksum);
}

// Zwalnia ramkę daną wskaźnikiem na jej zawartość
void icmp_destroy_frame(void *ptr)
{
    ipv4_destroy_frame((uint8_t*)ptr-ICMP_HEADER_SIZE);
}

// Wysyła ramke daną wskaźnikiem do jej zawartości
void icmp_transmit_frame(void *ptr, uint32_t size)
{
    // Liczy sume kontrolną dla nagłówka i zawartości
    struct icmp_header *icmp_header = (struct icmp_header*)ptr;
    icmp_header--;
    uint16_t checksum = calculate_checksum(icmp_header, ICMP_HEADER_SIZE);
    icmp_header->checksum = hston2(checksum);

    ipv4_transmit_frame(icmp_header, size + ICMP_HEADER_SIZE);
}

// Funkcja uruchamiana po otrzymaniu ramki icmp
void icmp_receive_frame(void* ptr, struct network_packet_info *packet_info)
{
    struct icmp_header *icmp_header = (struct icmp_header*)ptr;
    icmp_switch_header_lsb_msb(icmp_header);
    packet_info->icmp_header = icmp_header;

    // Sprawdzenie sumy kontrolnej
    uint16_t checksum = icmp_header->checksum;
    icmp_header->checksum = 0;
    uint16_t expected_checksum = calculate_checksum(icmp_header, ICMP_HEADER_SIZE);
    icmp_header->checksum = checksum;
    if(checksum != expected_checksum) return;

    // Otrzymano rządanie odpowiedzi
    if(icmp_header->type == ICMP_TYPE_ECHO_REQUEST)
    {
        int err = 0;
        void *icmp_reply = icmp_create_frame(ICMP_PING_PACKET_SIZE, ICMP_TYPE_ECHO_REPLY, 0, packet_info->ipv4_header->source_address, NULL);
        icmp_transmit_frame(icmp_reply, ICMP_PING_PACKET_SIZE);
        icmp_destroy_frame(icmp_reply);
    }

    // Otrzymano odpowiedz
    if(icmp_header->type == ICMP_TYPE_ECHO_REPLY)
    {
        waiting_for_reply = 0;
        receive_bytes_count = packet_info->ipv4_header->total_length - packet_info->ipv4_header->header_length * 4 - ICMP_HEADER_SIZE;
    }
}

// Funkcja żąda odpowiedzi i na nią czeka
int icmp_echo(uint32_t ip, uint32_t *ping, uint32_t *bytes)
{
    // Stworzenie ramki ICMP
    int mac_err = 0;
    void *icmp_frame = icmp_create_frame(0, ICMP_TYPE_ECHO_REQUEST, 0, ip, &mac_err);
    if(mac_err)
    {
        *ping = 0;
        *bytes = 0;
        return ICMP_ECHO_MAC_ERR;
    }

    // Wysłanie ramki
    waiting_for_reply = 1;
    icmp_transmit_frame(icmp_frame, 0);
    icmp_destroy_frame(icmp_frame);

    uint64_t start_time = clock();
    uint64_t current_time = clock();

    // Oczekiwanie na odpowiedz
    while(waiting_for_reply) 
    {
        // Przekroczenie limitu oczekiwanie
        if(current_time - start_time > ICMP_MAX_REPLY_TIME)
        {
            waiting_for_reply = 0;

            *ping = 0;
            *bytes = 0;
            return ICMP_ECHO_TIMEOUT_ERR;
        }

        current_time = clock();
    }

    // Otrzymano odpowiedz
    *ping = (uint32_t)(current_time - start_time);
    *bytes = receive_bytes_count;

    // Niszczenie ramki i zwrócenie kodu błędu
    return ICMP_ECHO_OK;
}

// Komenda ping
static void icmp_command_ping(const char* tokens, uint32_t tokens_count)
{
    // Odczytanie adresu IP
    char *ip_in_str = (char*)get_token(tokens, 1);
    int ok_flag = 0;
    uint32_t ip = str_to_ip(ip_in_str, &ok_flag);
    if(!ok_flag)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Invalid IP Format\n");
        return;
    }

    // Wyświetlenie nagłówka
    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("Pinging ");
    display_ip(ip);
    printf(" with %u bytes of data\n", ICMP_PING_PACKET_SIZE);
    terminal_setcolor(VGA_COLOR_WHITE);

    // Zmienna do tworzenia statystyk
    uint32_t packets_received = 0;
    uint32_t time_minimum = 0;
    uint32_t time_maximum = 0;
    uint32_t time_sum = 0;

    for(uint32_t i=0; i<ICMP_ECHO_REPEATS; i++)
    {
        uint32_t ping = 0;
        uint32_t bytes = 0;

        int res = icmp_echo(ip, &ping, &bytes);

        // Otrzymano odpowiedz
        if(res == ICMP_ECHO_OK) 
        {
            // Aktualizacja sttystyk
            packets_received++;
            if(i==0 || ping < time_minimum) time_minimum = ping;
            if(i==0 || ping > time_maximum) time_maximum = ping;
            time_sum += ping;

            // Wyświetlenie informacji
            printf("Reply from ");
            display_ip(ip);
            printf(": bytes=%u time=%ums\n", bytes, ping);
        }

        // Nie udało się uzyskać adresu MAC
        if(res == ICMP_ECHO_MAC_ERR)
            printf("Destination Unreachable\n");

        // Przekroczono czas oczekiwania
        if(res == ICMP_ECHO_TIMEOUT_ERR)
            printf("Timeout error\n");

        sleep(ICMP_TIME_BETWEEN_REPEATS);
    }

    // Wyświetlenie podsumowania o ilości pakietów
    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("\nPing statistics for ");
    display_ip(ip);
    printf(":\n");
    terminal_setcolor(VGA_COLOR_WHITE);
    printf("Packets: Send=%u, Received=%u, Lost=%u\n\n", ICMP_ECHO_REPEATS, packets_received, ICMP_ECHO_REPEATS-packets_received);

    // Mały trik by uniknąć zaraz dzielenia prez zero
    if(packets_received == 0) packets_received = 1;

    // Wyświetlenie podsumowania o czasie odpowiedzi
    double time_avg = (double)time_sum / packets_received;
    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("Approximate round trip time in milli-seconds:\n");
    terminal_setcolor(VGA_COLOR_WHITE);
    printf("Minimum=%ums, Maximum=%ums, Avarage=%lfms\n", time_minimum, time_maximum, time_avg);
}