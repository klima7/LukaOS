#include <stddef.h>
#include <stdint.h>
#include "arp.h"
#include "RTL8139.h"
#include "ethernet.h"
#include "utils.h"
#include "network.h"
#include "../shell.h"
#include "../heap.h"
#include "../clock.h"
#include "../terminal.h"
#include "../uni_list.h"
#include "../clib/stdio.h"
#include "../clib/string.h"

// Tworzy liste struktury mac_ip_entry
UNI_LIST_C(macip, struct mac_ip_entry)

// Funkcje statyczne
static void arp_command_askmac(const char* tokens, uint32_t tokens_count);
static void arp_command_askip(const char* tokens, uint32_t tokens_count);
static void arp_command_macip(const char* tokens, uint32_t tokens_count);

struct list_macip_t *mac_ip_list = NULL;

// Inicjuje modół ARP
void arp_initialize(void)
{
    // Dodaje własne IP i MAC do listy translacji
    mac_ip_list = list_macip_create();
    arp_add_mac_ip_entry(network_get_mac(), network_get_ip(), STATUS_OK);

    // Dodaje komendy
    register_command("askmac", "Translates IP address to MAC address", arp_command_askmac);
    register_command("askip", "Translates MAC address to IP address", arp_command_askip);
    register_command("macip", "Display connections between mac nad ip", arp_command_macip);
}

// Tworzy ramke protokołu ARP
void *arp_create_frame(uint16_t oper, uint64_t dmac, uint32_t dip)
{
    struct arp_header *arp_header = (struct arp_header*)ethernet_create_frame(ARP_HEADER_SIZE, 0x00000000, ETHERNET_ETHERTYPE_ARP);

    // Uzupełnienie pól ramki
    arp_header->hlen = 6;
    arp_header->plen = 4;
    arp_header->htype = ARP_HTYPE_ETHERNET;
    arp_header->ptype = ARP_PRYPE_IPV4;
    arp_header->oper = oper;

    arp_header->sip = network_get_ip();
    arp_header->smac = network_get_mac();
    arp_header->dip = dip;
    arp_header->dmac = dmac;

    // Zmienia format wszystkich poł do Big Endian
    arp_switch_header_lsb_msb(arp_header);

    arp_header++;
    return arp_header;
}

// Zwalnia ramkę daną wskaźnikiem na jej zawartość
void arp_destroy_frame(void *ptr)
{
    ethernet_destroy_frame((uint8_t*)ptr-ARP_HEADER_SIZE);
}

// Zmienia format pól nagłówka ramki ARP z little endian na big endian i odwrotnie
void arp_switch_header_lsb_msb(struct arp_header *arp_header)
{
    arp_header->hlen = hston1(arp_header->hlen);
    arp_header->plen = hston1(arp_header->plen);
    arp_header->htype = hston2(arp_header->htype);
    arp_header->ptype = hston2(arp_header->ptype);
    arp_header->oper = hston2(arp_header->oper);

    arp_header->sip = hston4(arp_header->sip);
    arp_header->smac = hston6(arp_header->smac);
    arp_header->dip = hston4(arp_header->dip);
    arp_header->dmac = hston6(arp_header->dmac);
}

// Wysyła ramke daną wskaźnikiem do jej zawartości
void arp_transmit_frame(void *ptr, uint32_t size)
{
    ethernet_transmit_frame((uint8_t*)ptr-ARP_HEADER_SIZE, size + ARP_HEADER_SIZE);
}

// Funkcja uruchamiana po otrzymaniu ramki ARP
void arp_receive_frame(void* ptr, struct network_packet_info *packet_info)
{
    // Przyrównanie ramki do struktury nagłówka i zamiana na Big Endian
    struct arp_header *arp_receive = (struct arp_header *)ptr;
    arp_switch_header_lsb_msb(arp_receive);

    packet_info->arp_header = arp_receive;

    // Odebranie zapytania o adres MAC
    if(arp_receive->oper == ARP_OPER_REQUEST && arp_receive->dip == network_get_ip())
    {
        // Najpierw sam dodaje mac i ip do swojej listy
        arp_add_mac_ip_entry(arp_receive->smac, arp_receive->sip, STATUS_OK);

        // Wysyła odpowiedz
        void *arp_send = arp_create_frame(ARP_OPER_REPLY, arp_receive->smac, arp_receive->sip);
        arp_transmit_frame(arp_send, 0);
        arp_destroy_frame(arp_send);
    }

    // Odebranie zapytania o adres IP
    if(arp_receive->oper == ARP_OPER_REVERSE_REQUEST && arp_receive->dmac == network_get_mac())
    {
        // Najpierw sam dodaje mac i ip do swojej listy
        arp_add_mac_ip_entry(arp_receive->smac, arp_receive->sip, STATUS_OK);

        // Wysyła odpowiedz
        void *arp_send = arp_create_frame(ARP_OPER_REVERSE_REPLY, arp_receive->smac, arp_receive->sip);
        arp_transmit_frame(arp_send, 0);
        arp_destroy_frame(arp_send);
    }

    // Odebranie odpowiedzi o adres MAC
    if(arp_receive->oper == ARP_OPER_REPLY)
    {
        struct node_macip_t *current = mac_ip_list->head;
        while(current!=NULL)
        {
            if(current->data.ip == arp_receive->sip && current->data.status == STATUS_WAITING_FOR_MAC)
            {
                current->data.mac = arp_receive->smac;
                current->data.status = STATUS_OK;
            }

            current = current->next;
        }
    }

    // Odebranie odpowiedzi o adres IP
    if(arp_receive->oper == ARP_OPER_REVERSE_REPLY)
    {
        struct node_macip_t *current = mac_ip_list->head;
        while(current!=NULL)
        {
            if(current->data.mac == arp_receive->smac && current->data.status == STATUS_WAITING_FOR_IP)
            {
                current->data.ip = arp_receive->sip;
                current->data.status = STATUS_OK;
            }

            current = current->next;
        }
    }
}

// Komenda tłumacząca adres ip na adres mac
static void arp_command_askmac(const char* tokens, uint32_t tokens_count)
{
    char *ip_in_str = (char*)get_token(tokens, 1);
    int ok_flag = 0;
    uint32_t ip = str_to_ip(ip_in_str, &ok_flag);
    if(!ok_flag)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Invalid IP Format\n");
        return;
    }

    int error = 0;
    uint64_t mac = arp_get_mac(ip, &error);

    // Udało się znaleść szukany adres
    if(!error)
    {
        printf("MAC address for given IP: ");
        terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
        display_mac(mac);
        printf("\n");
    }

    // Nie udało się znaleść szukanego adresu
    else
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Unable to get MAC address\n");
    }
}

// Komenda tłumacząca adres mac na adres ip
static void arp_command_askip(const char* tokens, uint32_t tokens_count)
{
    char *mac_in_str = (char*)get_token(tokens, 1);
    int ok_flag = 0;
    uint64_t mac = str_to_mac(mac_in_str, &ok_flag);
    if(!ok_flag)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Invalid MAC Format\n");
        return;
    }

    int error_flag = 0;
    uint32_t ip = arp_get_ip(mac, &error_flag);

    // Udało się znaleść szukany adres
    if(!error_flag)
    {
        printf("IP address for given MAC: ");
        terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
        display_ip(ip);
        printf("\n");
    }

    // Nie udało się znaleść szukanego adresu
    else
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Unable to get IP address\n");
    }
}

// Komenda wyświetlająca powiązania między adresami mac i ip
static void arp_command_macip(const char* tokens, uint32_t tokens_count)
{
    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("| MAC                        | IP\n");
    terminal_setcolor(VGA_COLOR_WHITE);

    struct node_macip_t *current = mac_ip_list->head;
    while(current != NULL)
    {
        printf("| ");
        display_mac(current->data.mac);
        for(uint32_t i=0; i<10; i++) printf(" ");
        printf("| ");
        display_ip(current->data.ip);
        printf("\n");
        current = current->next;
    }
}

// Dodaje do listy powiązanie między adresem mac oraz ip oraz zwraca wskaźnik na ten nowy wpis
struct node_macip_t *arp_add_mac_ip_entry(uint64_t mac, uint32_t ip, enum mac_ip_status status)
{
    struct mac_ip_entry entry = {0};
    entry.ip = ip;
    entry.mac = mac;
    entry.status = status;
    list_macip_push_back(mac_ip_list, entry);
    return list_macip_get_node_at(mac_ip_list, mac_ip_list->size-1);
}

// Tłumaczy adres ip na mac
uint64_t arp_get_mac(uint32_t ip, int *err)
{
    if(err != NULL) *err = 0;

    // Najpierw sprawdza czy takiego wpisu nie ma już na liście
    struct node_macip_t *current = mac_ip_list->head;
    while(current!=NULL)
    {
        if(current->data.ip == ip && current->data.status == STATUS_OK)
            return current->data.mac;

        current = current->next;
    }

    // Jeśli nie ma jeszcze takiego wpisu to go dodaje
    struct node_macip_t *entry = arp_add_mac_ip_entry(0, ip, STATUS_WAITING_FOR_MAC);

    // Wysyła zapytanie o adres mac
    void *arp_frame = arp_create_frame(ARP_OPER_REQUEST, 0, ip);
    arp_transmit_frame(arp_frame, 0);
    arp_destroy_frame(arp_frame);

    uint64_t time_start = clock();
    uint64_t time_current = clock();

    // Oczekiwanie na odpowiedz
    while(entry->data.status!=STATUS_OK)
    {
        // Nie udało się znaleść adresu MAC
        if(time_current - time_start > ARP_REPLY_WAITING_TIME)
        {
            // Usunięcie danego wpisu z listy
            list_macip_remove_node(mac_ip_list, entry);

            if(err != NULL) *err = 1;
            return 0;
        }

        time_current = clock();
    }

    // Zwraca adres mac
    return entry->data.mac;
}

// Tłumaczy adres mac na ip
uint32_t arp_get_ip(uint64_t mac, int *err)
{
    if(err != NULL) *err = 0;

    // Najpierw sprawdza czy takiego wpisu nie ma już na liście
    struct node_macip_t *current = mac_ip_list->head;
    while(current!=NULL)
    {
        if(current->data.mac == mac && current->data.status == STATUS_OK)
            return current->data.ip;

        current = current->next;
    }

    // Jeśli nie ma jeszcze takiego wpisu to go dodaje
    struct node_macip_t *entry = arp_add_mac_ip_entry(mac, 0, STATUS_WAITING_FOR_IP);

    // Wysyła zapytanie o adres ip
    void *arp_frame = arp_create_frame(ARP_OPER_REVERSE_REQUEST, mac, 0);
    arp_transmit_frame(arp_frame, 0);
    arp_destroy_frame(arp_frame);

    uint64_t time_start = clock();
    uint64_t time_current = clock();

    // Oczekiwanie na odpowiedz
    while(entry->data.status!=STATUS_OK)
    {
        // Nie udało się znaleść adresu MAC
        if(time_current - time_start > ARP_REPLY_WAITING_TIME)
        {
            // Usunięcie danego wpisu z listy
            list_macip_remove_node(mac_ip_list, entry);

            if(err != NULL) *err = 1;
            return 0;
        }

        time_current = clock();
    }

    // Zwraca adres mac
    return entry->data.ip;
}