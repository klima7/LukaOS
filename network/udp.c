#include <stddef.h>
#include <stdint.h>
#include "udp.h"
#include "ipv4.h"
#include "ethernet.h"
#include "RTL8139.h"
#include "arp.h"
#include "utils.h"
#include "network.h"
#include "network_ports.h"
#include "../shell.h"
#include "../clock.h"
#include "../heap.h"
#include "../clib/string.h"
#include "../clib/stdio.h"

// Fragmętuje i wysyła dane
int udp_send_data(void *ptr, uint32_t size, uint16_t source_port, uint16_t destination_port, uint32_t destination_ip)
{
    uint32_t fragments_count = size / UDP_FRAGMENT_SIZE;
    if(size % UDP_FRAGMENT_SIZE != 0) fragments_count++;

    uint8_t *transmit_pointer = (uint8_t*)ptr;

    // Iteruje po wszystkich fragmentach
    for(uint32_t i=0; i<fragments_count; i++)
    {
        uint8_t *frame_content = NULL;
        uint32_t fragment_size = 0;
        
        // Wszystkie fragmenty poza ostatnim
        if(i != fragments_count-1)
        {
            fragment_size = UDP_FRAGMENT_SIZE;

            int err = 0;
            frame_content = (uint8_t*)udp_create_frame(fragment_size, source_port, destination_port, i, IPV4_FLAG_MF, i*UDP_FRAGMENT_SIZE, destination_ip, &err);
            if(err) return UDP_SEND_ERR;
        }
        
        // Ostatni fragment
        else
        {
            fragment_size = size % UDP_FRAGMENT_SIZE;
            if(fragment_size == 0) fragment_size = UDP_FRAGMENT_SIZE;

            int err = 0;
            frame_content = (uint8_t*)udp_create_frame(fragment_size, source_port, destination_port, i, 0, i*UDP_FRAGMENT_SIZE, destination_ip, &err);
            if(err) return UDP_SEND_ERR;
        }

        // Dodaje do fragmentu dane
        memcpy(frame_content, transmit_pointer, fragment_size);
    
        // Gdy fragmenty są wysyłane za szybko to niekóre zostają pominięte
        sleep(10);

        // Wysyła fragmenty i zwalnia po nich pamięć
        udp_transmit_frame(frame_content, fragment_size);
        udp_destroy_frame(frame_content);

        // Przenosi wskaźnik o długość fragmentu
        transmit_pointer += fragment_size;
    }

    return UDP_SEND_OK;
}

// Tworzy ramke protokołu UDP
void *udp_create_frame(uint32_t size, uint16_t source_port, uint16_t destination_port, uint16_t identifier, uint16_t flags, uint16_t offset, uint32_t destination_ip, int *err)
{
    // Tworzy ramke
    struct udp_header *udp_header = (struct udp_header*)ipv4_create_frame(size+UDP_HEADER_SIZE, identifier, flags, offset, IPV4_PROTOCOL_UDP, destination_ip, err);
    if(*err) return NULL;

    // Uzupełnia pola nagłówka
    udp_header->source_port = source_port;
    udp_header->destination_port = destination_port;
    udp_header->length = size + UDP_HEADER_SIZE;
    udp_header->crc = 0;                                // Uzupełniane w czasie wysyłąnia

    // Zmienia format pól z Little Endian na Big Endian
    udp_switch_header_lsb_msb(udp_header);

    return udp_header+1;
}

// Zmienia format pól nagłówka ramki UDP z Little Endian na Big Endian i odwrotnie
void udp_switch_header_lsb_msb(struct udp_header *ipv4_header)
{
    ipv4_header->source_port = hston2(ipv4_header->source_port);
    ipv4_header->destination_port = hston2(ipv4_header->destination_port);
    ipv4_header->length = hston2(ipv4_header->length);
    ipv4_header->crc = hston2(ipv4_header->crc);
}

// Zwalnia ramkę daną wskaźnikiem na jej zawartość
void udp_destroy_frame(void *ptr)
{
    ipv4_destroy_frame((uint8_t*)ptr-UDP_HEADER_SIZE);
}

// Wysyła ramke daną wskaźnikiem do jej zawartości
void udp_transmit_frame(void *ptr, uint32_t size)
{
    // Liczy sume kontrolną dla nagłówka i zawartości
    struct udp_header *udp_header = (struct udp_header*)ptr;
    udp_header--;
    uint16_t checksum = calculate_checksum(udp_header, udp_header->length + UDP_HEADER_SIZE);
    udp_header->crc = hston2(checksum);

    ipv4_transmit_frame(udp_header, size + UDP_HEADER_SIZE);
}

// Funkcja uruchamiana po otrzymaniu ramki UDP
void udp_receive_frame(void* ptr, struct network_packet_info *packet_info)
{
    // Rzutuje wskaźnik na strukture nagłówka
    struct udp_header *udp_header = (struct udp_header*)ptr;
    packet_info->udp_header = udp_header;

    // Zmienia format pól z Little Endian na Big Endian
    udp_switch_header_lsb_msb(udp_header);

    // Sprawdza sume kontrolną
    uint16_t checksum = udp_header->crc;
    udp_header->crc = 0;
    uint16_t expected_checksum = calculate_checksum(udp_header-1, udp_header->length);
    udp_header->crc = checksum;
    if(checksum != expected_checksum) return;

    struct port_t *port = ports_get(udp_header->destination_port); 

    // Jeżeli nie nasłuchujemy na danym porcie to ignorujemy pakiet
    if(port==NULL) return;

    uint32_t data_size = (uint32_t)packet_info->ipv4_header->total_length - (uint32_t)packet_info->ipv4_header->header_length - UDP_HEADER_SIZE;
    uint32_t required_buffer_size = (uint32_t)packet_info->ipv4_header->offset + data_size;

    // W razie konieczności zaalokowanie pamięci na dane
    if(port->buffer_size < required_buffer_size) port->buffer_ptr = krealloc(port->buffer_ptr, required_buffer_size);

    // Przeniesienie danych do portu
    memcpy(port->buffer_ptr + (uint32_t)packet_info->ipv4_header->offset, udp_header+1, data_size);

    // Sprawdzenie czy to był ostatni frgment
    if( (packet_info->ipv4_header->flags & IPV4_FLAG_MF) == 0)
    {
        ports_notify_listener(udp_header->destination_port, packet_info);

        // Po powiadomieniu słuchacza, który wykorzystał dane w porcie, dane te są zwalniane
        kfree(port->buffer_ptr);
        port->buffer_ptr = NULL;
        port->buffer_size = 0;
    }
}
