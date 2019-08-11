#ifndef __ICMP_H__
#define __ICMP_H__

#include <stddef.h>
#include <stdint.h>
#include "network.h"

// Struktura nagłówka protokołu ICMP
struct icmp_header
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
};

// definicje
#define ICMP_HEADER_SIZE sizeof(struct icmp_header)
#define ICMP_MAX_REPLY_TIME 2000
#define ICMP_PING_PACKET_SIZE 64
#define ICMP_ECHO_REPEATS 6
#define ICMP_TIME_BETWEEN_REPEATS 1000

// Możliwe typy
#define ICMP_TYPE_ECHO_REPLY 0x00
#define ICMP_TYPE_ECHO_REQUEST 0x08

// Kody błędu funkcji icmp_echo
#define ICMP_ECHO_OK 0
#define ICMP_ECHO_MAC_ERR 1
#define ICMP_ECHO_TIMEOUT_ERR 2


// Prototypy
void icmp_initialize(void);
void *icmp_create_frame(uint32_t size, uint8_t type, uint8_t code, uint32_t destination_ip, int *err);
void icmp_switch_header_lsb_msb(struct icmp_header *icmp_header);
void icmp_destroy_frame(void *ptr);
void icmp_transmit_frame(void *ptr, uint32_t size);
void icmp_receive_frame(void* ptr, struct network_packet_info *packet_info);
int icmp_echo(uint32_t ip, uint32_t *ping, uint32_t *bytes);

#endif