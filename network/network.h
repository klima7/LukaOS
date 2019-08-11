#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <stddef.h>
#include <stdint.h>

// Struktura przekazywana między wartswami i uzupełniana tak aby wyższe wartstyw miały łatwy dostęp do pól warstw niższych
struct network_packet_info
{
    uint32_t receive_packet_size;

    struct ethernet_header *ethernet_header;
    struct ipv4_header *ipv4_header;
    struct arp_header *arp_header;
    struct udp_header *udp_header;
    struct icmp_header *icmp_header;
};

// Prototypy
void network_initialize(void);
uint32_t network_get_ip(void);
uint64_t network_get_mac(void);

#endif