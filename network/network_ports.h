#ifndef __NETWORK_PORTS_H__
#define __NETWORK_PORTS_H__

#include <stddef.h>
#include <stdint.h>
#include "../heap.h"
#include "../uni_list.h"
#include "network.h"

#define PORTS_COUNT 0xFFFF

// Wzór funkcji oczekującej na dane w porcie
typedef void (*PORT_LISTENER)(void *packet_data, struct network_packet_info *packet_info);

enum port_state_t { PORT_IDLE=0, PORT_LISTENING };

// Struktura portu
struct port_t
{
    uint16_t nr;

    uint8_t *buffer_ptr;
    uint32_t buffer_size;
    
    PORT_LISTENER listener;
};

UNI_LIST_H(ports, struct port_t)

// Prototypy
void ports_initialize(void);
void ports_create(uint16_t nr, PORT_LISTENER listener);
void ports_remove(uint16_t nr);
void ports_notify_listener(uint16_t nr, struct network_packet_info *packet_info);
struct port_t *ports_get(uint16_t nr);

#endif