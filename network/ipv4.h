#ifndef __IPV4_H__
#define __IPV4_H__

#include <stddef.h>
#include <stdint.h>
#include "network.h"

// Nagłówek ramki IPv4
struct ipv4_header
{
    uint8_t version : 4;
    uint8_t header_length : 4;
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t identifier;
    uint16_t flags : 3;
    uint16_t offset : 13;
    uint8_t time_to_live;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t source_address;
    uint32_t destination_address;
};

#define IPV4_HEADER_SIZE sizeof(struct ipv4_header)

// Możliwe wartości niektórych pól struktury
#define IPV4_VERSION 0x4
#define IPV4_HEADER_LEN 0x5
#define IPV4_PROTOCOL_UDP 0x11 
#define IPV4_PROTOCOL_ICMP 0x01 

// Znaczenie poszczególnych bitów pola flagi
#define IPV4_FLAG_MF 0x1     // Jeśli bit ustawiony to dany fragment nie jest ostatni
#define IPV4_FLAG_DF 0x2     // Jeśli bit jest ustawiony to pakietu nie można poddawać fragmentacji

// Prototypy
void ipv4_initialize(void);
void *ipv4_create_frame(uint32_t size, uint16_t identifier, uint16_t flags, uint16_t offset, uint8_t protocol, uint32_t destination_address, int *err);
void ipv4_destroy_frame(void *ptr);
void ipv4_switch_header_lsb_msb(struct ipv4_header *ipv4_header);
void ipv4_transmit_frame(void *ptr, uint32_t size);
void ipv4_receive_frame(void* ptr, struct network_packet_info *packet_info);

#endif