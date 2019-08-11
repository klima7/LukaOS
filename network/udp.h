#ifndef __UDP_H__
#define __UDP_H__

#include <stddef.h>
#include <stdint.h>
#include "network.h"

// Nagłówek protokołu UDP
struct udp_header
{
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t length;
    uint16_t crc;
};

// Rozmiar pojedyńczego fragmentu na które dzielone są dane
#define UDP_FRAGMENT_SIZE 1000

// Wartości zwracane przez funkcje udp_send_data
#define UDP_SEND_OK 0
#define UDP_SEND_ERR 1

#define UDP_HEADER_SIZE sizeof(struct udp_header)
#define UDP_FULL_SIZE 1000

// Prototypy
void udp_initialize(void);
void *udp_create_frame(uint32_t size, uint16_t source_port, uint16_t destination_port, uint16_t identifier, uint16_t flags, uint16_t offset, uint32_t destination_ip, int *err);
void udp_switch_header_lsb_msb(struct udp_header *ipv4_header);
void udp_destroy_frame(void *ptr);
void udp_transmit_frame(void *ptr, uint32_t size);
int udp_send_data(void *ptr, uint32_t size, uint16_t source_port, uint16_t destination_port, uint32_t destination_ip);
void udp_receive_frame(void* ptr, struct network_packet_info *packet_info);

#endif