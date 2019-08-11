#ifndef __ETHERNET_H__
#define __ETHERNET_H__

#include <stddef.h>
#include <stdint.h>
#include "network.h"

// Definicje
#define ETHERNET_ETHERTYPE_IPV4     0x0800
#define ETHERNET_ETHERTYPE_ARP      0x0806

// Początek ramki protokołu ethernetowego
struct ethernet_header
{
    uint64_t dest_mac : 48;
    uint64_t src_mac : 48;
    uint16_t ethertype;
    // Dane
} __attribute__((packed));

#define ETHERNET_HEADER_SIZE sizeof(struct ethernet_header)

// Prototypy
void *ethernet_create_frame(uint32_t size, uint64_t dest_mac, uint16_t ethertype);
void ethernet_destroy_frame(void *ptr);
void ethernet_switch_header_lsb_msb(struct ethernet_header *ethernet_header);
void ethernet_transmit_frame(void *ptr, uint32_t size);
void ethernet_receive_frame(void *ptr, struct network_packet_info *packet_info);

#endif