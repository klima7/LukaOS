#include <stddef.h>
#include <stdint.h>
#include "talk.h"
#include "../network/udp.h"
#include "../network/network.h"
#include "../network/ipv4.h"
#include "../network/utils.h"
#include "../network/network_ports.h"
#include "../shell.h"
#include "../heap.h"
#include "../terminal.h"
#include "../clib/stdio.h"
#include "../clib/string.h"

char talk_nick[TALK_MAX_NICK_LEN+1] = {0};
uint32_t last_ip = 0;
int last_ip_valid = 0;

// Funkcje statyczne
static void talk_command_talk(const char* tokens, uint32_t tokens_count);
static void talk_listener(void *packet_data, struct network_packet_info *packet_info);
static void talk_command_setnick(const char* tokens, uint32_t tokens_count);
static void talk_command_reply(const char* tokens, uint32_t tokens_count);

// Inicjuje komende talk
void talk_initialize(void)
{
    // Ustawia domyślny pseudonim
    strcpy(talk_nick, TALK_DEFAULT_NICK);

    // Tworzy port na którym będzie oczekiwał wiadomości
    ports_create(TALK_PORT, talk_listener);

    // Dodaje komende
    register_command("talk", "Command sends message to given IP", talk_command_talk);
    register_command("setnick", "Command sets talk nick", talk_command_setnick);
    register_command("reply", "Command send message to lasn known ip", talk_command_reply);
}

// Wysyła wiadomość
int talk_send_message(uint32_t ip, char *message)
{
    // Odczytuje czas
    struct time_t time = {0};
    get_time(&time);

    uint32_t data_size = sizeof(struct talk_message_t) + strlen(message) + 1;
    struct talk_message_t *data_to_send = (struct talk_message_t*)kmalloc(data_size);

    strcpy(data_to_send->nick, talk_nick);
    data_to_send->send_time = time;
    strcpy(data_to_send->message, message);

    int res = udp_send_data(data_to_send, data_size, TALK_PORT, TALK_PORT, ip);
    return res;
}

// Komenda talk
static void talk_command_talk(const char* tokens, uint32_t tokens_count)
{
    if(tokens_count < 3)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Not enough arguments. You must enter IP address and message\n");
        return;
    }

    // Odczytuje adres IP
    char *ip_in_str = (char*)get_token(tokens, 1);
    int ok_flag = 0;
    uint32_t ip = str_to_ip(ip_in_str, &ok_flag);
    if(!ok_flag)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Invalid IP Format\n");
        return;
    }

    // Odczytuje wiadomość
    char *message = (char*)get_token(tokens, 2);

    // Wysyła wiadomość
    int res = talk_send_message(ip, message);

    // Udało się wysłać wiadomość
    if(res == 0)
    {
        terminal_setcolor(VGA_COLOR_WHITE);
        printf("Message send to ");
        terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
        display_ip(ip);
        printf("\n");
    }

    // Nie udało się wysłać wiadomości
    else
    {
        terminal_setcolor(VGA_COLOR_WHITE);
        printf("Couldn't send message to ");
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        display_ip(ip);
        printf("\n");
    }   
}

// Komenda reply
static void talk_command_reply(const char* tokens, uint32_t tokens_count)
{
    // Czy podano wszystkie argumenty
    if(tokens_count < 1)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Not enough arguments. You must enter message\n");
        return;
    }

    if(!last_ip_valid)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Any message to reply\n");
        return;
    }

   // Odczytuje wiadomość
    char *message = (char*)get_token(tokens, 1);

    // Wysyła wiadomość
    int res = talk_send_message(last_ip, message);

    // Udało się wysłać wiadomość
    if(res == 0)
    {
        terminal_setcolor(VGA_COLOR_WHITE);
        printf("Message send to ");
        terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
        display_ip(last_ip);
        printf("\n");
    }

    // Nie udało się wysłać wiadomości
    else
    {
        terminal_setcolor(VGA_COLOR_WHITE);
        printf("Couldn't send message to ");
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        display_ip(last_ip);
        printf("\n");
    }   
}

// Komenda setnick
static void talk_command_setnick(const char* tokens, uint32_t tokens_count)
{
    // Czy podano wszystkie argumenty
    if(tokens_count < 2)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Not enough arguments. You must enter new nick\n");
        return;
    }

    // Sprawdzenie czy nick nie jest za długi
    char *new_nick = (char*)get_token(tokens, 1);
    if(strlen(new_nick) > TALK_MAX_NICK_LEN)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Nick is loo long, max length is %u\n", TALK_MAX_NICK_LEN);
        return;
    }

    // Ustawienie nicku
    strcpy(talk_nick, new_nick);

    // Komunikat
    terminal_setcolor(VGA_COLOR_WHITE);
    printf("Nick changed to ");
    terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
    printf("%s\n", new_nick);
}

// Funkcja wywoływana po otrzymaniu wiadomości
static void talk_listener(void *packet_data, struct network_packet_info *packet_info)
{
    struct talk_message_t *data_received = (struct talk_message_t*)packet_data;

    shell_start_insert();

    terminal_setcolor(VGA_COLOR_WHITE);
    printf("Message from ");
    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("%s", data_received->nick);
    terminal_setcolor(VGA_COLOR_WHITE);
    printf(" Sender IP ");
    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    display_ip(packet_info->ipv4_header->source_address);
    printf("\n");
    terminal_setcolor(VGA_COLOR_WHITE);
    printf("Send time  : ");
    time_display(&data_received->send_time);
    printf("\n");
    terminal_setcolor(VGA_COLOR_WHITE);
    printf("Message    : %s\n", data_received->message);

    shell_end_insert();

    last_ip_valid = 1;
    last_ip = packet_info->ipv4_header->source_address;
}
