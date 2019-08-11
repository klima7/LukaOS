#include <stddef.h>
#include <stdint.h>
#include "ipv4.h"
#include "ethernet.h"
#include "RTL8139.h"
#include "arp.h"
#include "udp.h"
#include "utils.h"
#include "network.h"
#include "icmp.h"
#include "../shell.h"
#include "../clib/string.h"
#include "../clib/stdio.h"

// Tworzy ramke protokołu IPv4
void *ipv4_create_frame(uint32_t size, uint16_t identifier, uint16_t flags, uint16_t offset, uint8_t protocol, uint32_t destination_address, int *err)
{
    if(err!=NULL) *err = 0;

    // Tłumaczy adres IP na adres MAC
    int mac_err = 0;
    uint64_t mac = arp_get_mac(destination_address, &mac_err);
    if(mac_err) 
    {
        if(err!=NULL)  *err = 1;
        return NULL;
    }

    // Tworzy ramke
    struct ipv4_header *ipv4_header = (struct ipv4_header*)ethernet_create_frame(IPV4_HEADER_SIZE + size, mac, ETHERNET_ETHERTYPE_IPV4);

    // Uzupełnia pola nagłówka
    ipv4_header->version = IPV4_VERSION;
    ipv4_header->header_length = IPV4_HEADER_LEN;
    ipv4_header->type_of_service = 0;               // Pole nieużywane
    ipv4_header->total_length = size + IPV4_HEADER_LEN * 4;
    ipv4_header->identifier = identifier;
    ipv4_header->flags = flags;
    ipv4_header->offset = offset;               
    ipv4_header->time_to_live = 0;                  // Pole nieużywane
    ipv4_header->protocol = protocol;
    ipv4_header->source_address = network_get_ip();
    ipv4_header->destination_address = destination_address;
    ipv4_header->checksum = 0;                      // Suma jest uzupełniana w czasie wysyłania

    // Zamienia format pól na Big Endian
    ipv4_switch_header_lsb_msb(ipv4_header);

    // Zwraca wskaźnik do zawartości
    return ipv4_header+1;
}

// Zmienia format pól nagłówka ramki IPv4 z Little Endian na Big Endian i odwrotnie
void ipv4_switch_header_lsb_msb(struct ipv4_header *ipv4_header)
{
    ipv4_header->total_length = hston2(ipv4_header->total_length);
    ipv4_header->identifier = hston2(ipv4_header->identifier);
    ipv4_header->checksum = hston2(ipv4_header->checksum);
    ipv4_header->source_address = hston4(ipv4_header->source_address);
    ipv4_header->destination_address = hston4(ipv4_header->destination_address);
}

// Zwalnia ramkę daną wskaźnikiem na jej zawartość
void ipv4_destroy_frame(void *ptr)
{
    ethernet_destroy_frame((uint8_t*)ptr-IPV4_HEADER_SIZE);
}

// Wysyła ramke daną wskaźnikiem do jej zawartości
void ipv4_transmit_frame(void *ptr, uint32_t size)
{
    // Uzupełnienie sumy kontrolnej
    struct ipv4_header *ipv4_header = (struct ipv4_header*)ptr;
    ipv4_header--;
    uint16_t checksum = calculate_checksum(ipv4_header, IPV4_HEADER_SIZE);
    ipv4_header->checksum = hston2(checksum);

    ethernet_transmit_frame(ipv4_header, size + IPV4_HEADER_SIZE);
}

// Funkcja uruchamiana po otrzymaniu ramki IPv4
void ipv4_receive_frame(void* ptr, struct network_packet_info *packet_info)
{
    // Rzutuje wskaźnik na strukture nagłówka
    struct ipv4_header *ipv4_header = (struct ipv4_header*)ptr;

    // Zamienia Little Endian na Big Endian
    ipv4_switch_header_lsb_msb(ipv4_header);

    // Sprawdzenie sumy kontrolnej
    uint16_t checksum = ipv4_header->checksum;
    ipv4_header->checksum = 0;
    uint16_t expected_checksum = calculate_checksum(ipv4_header-1, IPV4_HEADER_SIZE);
    ipv4_header->checksum = checksum;
    if(checksum != expected_checksum) return;

    packet_info->ipv4_header = ipv4_header;

    // Jeżeli ramka IPv4 zawiera ramkę UDP to jest przekazywana do warstwy UDP
    if(ipv4_header->protocol == IPV4_PROTOCOL_UDP)
        udp_receive_frame(ipv4_header+1, packet_info);

    // Jeżeli ramka IPv4 zawiera ramkę ICMP to jest przekazywana do warstwy ICMP
    if(ipv4_header->protocol == IPV4_PROTOCOL_ICMP)
        icmp_receive_frame(ipv4_header+1, packet_info);
}