#include <stddef.h>
#include <stdint.h>
#include "ethernet.h"
#include "ipv4.h"
#include "RTL8139.h"
#include "utils.h"
#include "arp.h"
#include "network.h"
#include "../heap.h"
#include "../shell.h"
#include "../clib/stdio.h"
#include "../clib/string.h"

// Zmienia format pól nagłówka ramki ETHERNET z little endian na big endian i odwrotnie
void ethernet_switch_header_lsb_msb(struct ethernet_header *ethernet_header)
{
    ethernet_header->dest_mac = hston6(ethernet_header->dest_mac);
    ethernet_header->src_mac = hston6(ethernet_header->src_mac);
    ethernet_header->ethertype = hston2(ethernet_header->ethertype);
}

// Tworzy ramke protokołu eternetowego o rozmiarze size i zwraca wskaźnik do jej zawartości
void *ethernet_create_frame(uint32_t size, uint64_t dest_mac, uint16_t ethertype)
{
    // Tworzy ramke
    struct ethernet_header *frame = (struct ethernet_header*)kmalloc(size + ETHERNET_HEADER_SIZE);

    // Uzupełnia pola nagłówka
    frame->src_mac = network_get_mac();
    frame->dest_mac = dest_mac;
    frame->ethertype = ethertype;

    // Zmienia format pól na Big Endian
    ethernet_switch_header_lsb_msb(frame);

    return frame+1;
}

// Zwalnia ramkę daną wskaźnikiem na jej zawartość
void ethernet_destroy_frame(void *ptr)
{
    kfree((uint8_t*)ptr-ETHERNET_HEADER_SIZE);
}

// Wysyła ramke daną wskaźnikiem do jej zawartości
void ethernet_transmit_frame(void *ptr, uint32_t size)
{
    RTL8139_send_packet((uint8_t*)ptr-ETHERNET_HEADER_SIZE, size+ETHERNET_HEADER_SIZE);
}

// Funkcja uruchamiana po otrzymaniu ramki ETHERNET
void ethernet_receive_frame(void *ptr, struct network_packet_info *packet_info)
{
    struct ethernet_header *header = (struct ethernet_header*)ptr;
    ethernet_switch_header_lsb_msb(header);

    packet_info->ethernet_header = header;

    // Przejście do warstwy ARP
    if(header->ethertype == ETHERNET_ETHERTYPE_ARP)
        arp_receive_frame(header+1, packet_info);
    
    if(header->dest_mac == network_get_mac())
    {
        // Przejście do warstwy IPv4
        if(header->ethertype == ETHERNET_ETHERTYPE_IPV4)
            ipv4_receive_frame(header+1, packet_info);
    }
}
