#include <stddef.h>
#include <stdint.h>
#include "RTL8139.h"
#include "ethernet.h"
#include "utils.h"
#include "network.h"
#include "../sys.h"
#include "../pci.h"
#include "../time.h"
#include "../ports.h"
#include "../shell.h"
#include "../pic.h"
#include "../clock.h"
#include "../idt.h"
#include "../rng.h"
#include "../clib/stdio.h"
#include "../clib/string.h"

// Zmienne
struct pci_entry_t *RTL8139_chip = NULL;
uint32_t io_base = 0;
uint8_t receive_buffer[RTL8139_RECEIVE_BUFFER_SIZE];
uint32_t next_transmit_descriptor = 0;
uint8_t *next_packet_start = 0;

uint32_t transmit_buffers[4] = { RTL8139_TRANSMIT_BUFFER_0, RTL8139_TRANSMIT_BUFFER_1, RTL8139_TRANSMIT_BUFFER_2, RTL8139_TRANSMIT_BUFFER_3 };
uint32_t transmit_registers[4] = { RTL8139_TRANSMIT_REGISTER_0, RTL8139_TRANSMIT_REGISTER_1, RTL8139_TRANSMIT_REGISTER_2, RTL8139_TRANSMIT_REGISTER_3 };

// Funkcje statyczne
static void RTL8139_init_ip_and_mac(void);
static void RTL8139_enable_bus_mastering(void);
static void RTL8139_get_io_base(void);
static void RTL8139_turn_on(void);
static void RTL8139_reset(void);
static void RTL8139_init_receive_buffer(void);
static void RTL8139_init_imr(void);
static void RTL8139_init_rcr(void);
static void RTL8139_enable_receive_and_transmit(void);
static void RTL8139_handler(void);

// Inicjalizuje karte sieciową
void RTL8139_initialize(void)
{
    next_packet_start = receive_buffer;

    // Dodaje obsługe przerwania
    interrupt_register(43, RTL8139_handler);

    // Sprawdza czy komputer posiada karte RTL8139
    RTL8139_chip = pci_get_device_by_id(RTL8139_DEVICE_ID);
    if(RTL8139_chip == NULL) report_error("Couldn't Detect RTL8139 CHIP\n");

    for(uint32_t i=0; i<RTL8139_RECEIVE_BUFFER_SIZE; i++)
        receive_buffer[i] = 2;

    // Procedury inicjujące, podzielone na małe fragmęty by się nie pobugić
    RTL8139_enable_bus_mastering();
    RTL8139_get_io_base();
    RTL8139_turn_on();
    RTL8139_reset();
    RTL8139_init_receive_buffer();
    RTL8139_init_imr();
    RTL8139_init_rcr();
    RTL8139_enable_receive_and_transmit();
}

// Ustala flagę bus mastering
static void RTL8139_enable_bus_mastering(void)
{
    uint16_t command_register = pci_get_command_register(RTL8139_chip->bus, RTL8139_chip->slot, 0);
    command_register |= RTL8139_COMMAND_REGISTER_BUS_MASTER_BIT;
    pci_save_command_register(RTL8139_chip->bus, RTL8139_chip->slot, 0, command_register);
}

// Odczytuje adres bazowy
static void RTL8139_get_io_base(void)
{
    uint32_t bar_register = pci_read(RTL8139_chip->bus, RTL8139_chip->slot, 0, PCI_TYPE0_BASE_ADDRESS_0);
    io_base = pci_bar_get_address(bar_register);
}

// Uruchamia karte sieciową
static void RTL8139_turn_on(void)
{
    outportb(io_base + RTL8139_CONFIG_1_REGISTER, RTL8139_TURN_ON);
}

// Resetuje karte
static void RTL8139_reset(void)
{
    outportb(io_base + RTL8139_CMD_REGISTER, RTL8139_RESET);

    // Czekamy do końca resetu
    while(1) 
    {
        uint32_t command_register = inportb(io_base + RTL8139_CMD_REGISTER);
        uint32_t reset_in_program = command_register & RTL8193_COMMAND_REG_RST;
        if(!reset_in_program) break;
    }
}

// Inicjuje bufer odbioru
static void RTL8139_init_receive_buffer(void)
{
    uint32_t receive_buffer_address = (uint32_t)&receive_buffer;
    outportdw(io_base + RTL8139_RBSTART_REGISTER, receive_buffer_address);
}

// Ustala rejestr IMR i włącza obsługe przerwania w PIC
static void RTL8139_init_imr(void)
{
    uint16_t new_imr = RTL8193_IMR_ISR_TOK | RTL8193_IMR_ISR_ROK | 16 | 0xFFFF;
    outportw(io_base + RTL8139_IMR_REGISTER, new_imr);

    PIC_clear_mask(9);
    PIC_clear_mask(10);
    PIC_clear_mask(11);
}

// Inicjuje rejestr odbioru
static void RTL8139_init_rcr(void)
{
    uint32_t new_crc = RTL8193_RCR_AAP | RTL8193_RCR_APM | RTL8193_RCR_AM | RTL8193_RCR_AB | RTL8193_RCR_WRAP0;
    outportdw(io_base + RTL8139_RCR_REGISTER, new_crc);
}

// Uruchamia funkcje RX orax TX
static void RTL8139_enable_receive_and_transmit(void)
{
    uint8_t new_command_reg = RTL8193_COMMAND_REG_RE | RTL8193_COMMAND_REG_TE;
    outportb(io_base + RTL8139_CMD_REGISTER, new_command_reg);
}

// Wysyla pakiet
void RTL8139_send_packet(void *ptr, uint32_t size)
{
    uint32_t transmit_buffer = transmit_buffers[next_transmit_descriptor];
    uint32_t transmit_register = transmit_registers[next_transmit_descriptor];

    uint32_t address = (uint32_t)ptr;
    outportdw(io_base + transmit_buffer, address);

    uint32_t transmit_register_data = size & RTL8193_TRANSMIT_STATUS_SIZE;
    outportdw(io_base + transmit_register, transmit_register_data);

    // Oczekiwanie aż pakiet zostanie przeniesiony przez DMA do karty sieciowej
    while(1)
    {
        transmit_register_data = inportw(io_base + transmit_register);
        if(transmit_register_data & RTL8193_TRANSMIT_STATUS_OWN) break;
    }
    
    next_transmit_descriptor++;
    next_transmit_descriptor %= 4;
}

// Funkcja wywoływana po otrzymaniu pakietu
static void RTL8139_handler(void)
{
    uint16_t isr = inportw(io_base + RTL8139_ISR_REGISTER);

    // Przepełnienie bufora
    if(isr & RTL8193_IMR_ISR_RXOVW)
    {
        outportw(io_base + RTL8139_CAPR_REGISTER, 0);
        outportw(io_base + RTL8139_ISR_REGISTER, 1);
        RTL8139_handler();
    }

    // Otrzymanie pakietu
    if(isr & RTL8193_IMR_ISR_ROK)
    {
        // Powiadomienie o obsłużeniu zdarzenia
        outportw(io_base + RTL8139_ISR_REGISTER, 0x1);

        // Struktura to jest przekazywana dalej między warstwami i uzupełniana
        struct network_packet_info packet_info;

        // Powiadomienie wartstwyl ETHERNET o otrzymaniu pakietu
        void *packet = RTL8139_get_packet(&packet_info.receive_packet_size);
        ethernet_receive_frame(packet, &packet_info);
    }

    if(isr & RTL8193_IMR_ISR_TOK)
        outportw(io_base + RTL8139_ISR_REGISTER, isr);
}

// Zwraca adres ramki i jej rozmiar
void *RTL8139_get_packet(uint32_t *size)
{
    struct RTL8139_header *header = (struct RTL8139_header*)next_packet_start;
    uint32_t header_size = (uint32_t)header->length;

    *size = header_size - 4;
    void *ret_value = next_packet_start + 4;
    next_packet_start += header_size + 4;
    if(header_size % 4 != 0)
        next_packet_start += 4 - header_size % 4;
    return ret_value;
}