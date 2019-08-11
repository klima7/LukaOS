#ifndef __ARP_H__
#define __ARP_H__

#include <stddef.h>
#include <stdint.h>
#include "network.h"
#include "../uni_list.h"

// Możliwe operacje
#define ARP_OPER_REQUEST            1
#define ARP_OPER_REPLY              2
#define ARP_OPER_REVERSE_REQUEST    3
#define ARP_OPER_REVERSE_REPLY      4

// Inna możliwe wartości pól nagłówka
#define ARP_HTYPE_ETHERNET          1
#define ARP_PRYPE_IPV4              0x0800

// Nagłówek protokołu arp
struct arp_header
{
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;

    uint64_t smac : 48;
    uint64_t dmac : 48;
    uint32_t sip;
    uint32_t dip;
} __attribute__((packed));

enum mac_ip_status { STATUS_WAITING_FOR_MAC, STATUS_WAITING_FOR_IP, STATUS_OK };

// Element listy służącej do tłumaczenia adresów ip na mac
struct mac_ip_entry
{
    volatile uint32_t ip;
    volatile uint64_t mac;
    volatile enum mac_ip_status status;
};

UNI_LIST_H(macip, struct mac_ip_entry)

// Rozmiar
#define ARP_HEADER_SIZE sizeof(struct arp_header)
#define ARP_REPLY_WAITING_TIME 100

// Prototypy
void arp_initialize(void);
void arp_receive_frame(void* ptr, struct network_packet_info *packet_info);
void arp_switch_header_lsb_msb(struct arp_header *arp_header);
struct node_macip_t *arp_add_mac_ip_entry(uint64_t mac, uint32_t ip, enum mac_ip_status status);
uint64_t arp_get_mac(uint32_t ip, int *err);
uint32_t arp_get_ip(uint64_t mac, int *err);

#endif