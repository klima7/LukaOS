#ifndef __RTL8139_H__
#define __RTL8139_H__

#include <stddef.h>
#include <stdint.h>
#include "network.h"

// Właściwości karty
#define RTL8139_VENDOR_ID 0x10EC
#define RTL8139_DEVICE_ID 0x8139

// Adresy portów względem io base
#define RTL8139_CONFIG_1_REGISTER 0x52
#define RTL8139_MAC0_5_REGISTER 0x00
#define RTL8139_MAR0_7_REGISTER 0x08
#define RTL8139_RBSTART_REGISTER 0x30
#define RTL8139_CMD_REGISTER 0x37
#define RTL8139_IMR_REGISTER 0x3C
#define RTL8139_ISR_REGISTER 0x3E 
#define RTL8139_RCR_REGISTER 0x44
#define RTL8139_CAPR_REGISTER 0x38

// Wartości zapisywanych bajtów
#define RTL8139_TURN_ON 0x00
#define RTL8139_RESET 0x10

// Bufory 32bit wykorzystywane do transmicji, są w przestrzeni io 
#define RTL8139_TRANSMIT_BUFFER_0 0x20
#define RTL8139_TRANSMIT_BUFFER_1 0x24
#define RTL8139_TRANSMIT_BUFFER_2 0x28
#define RTL8139_TRANSMIT_BUFFER_3 0x2C

// Rejestry 32bit wykorzystywane do transmisji, są w przestrzeni io 
#define RTL8139_TRANSMIT_REGISTER_0 0x10
#define RTL8139_TRANSMIT_REGISTER_1 0x14
#define RTL8139_TRANSMIT_REGISTER_2 0x18
#define RTL8139_TRANSMIT_REGISTER_3 0x1C

// Poszczególne bity rejestru IMR I ISR
#define RTL8193_IMR_ISR_SERR    0x8000      // System Error
#define RTL8193_IMR_ISR_TIMEOUT 0x4000      // Time Out
#define RTL8193_IMR_ISR_LENCHG  0x2000      // Cable Length Change
#define RTL8193_IMR_ISR_FOVW    0x0040      // Rx FIFO Overflow
#define RTL8193_IMR_ISR_PUN     0x0020      // Packet Underrun / Link Change
#define RTL8193_IMR_ISR_RXOVW   0x0010      // Rx Buffer Overflow
#define RTL8193_IMR_ISR_TER     0x0008      // Transmit Error
#define RTL8193_IMR_ISR_TOK     0x0004      // Transmit Ok
#define RTL8193_IMR_ISR_RER     0x0002      // Receive Error
#define RTL8193_IMR_ISR_ROK     0x0001      // Receive OK

// Poszczególne bity rejestru RCR
#define RTL8193_RCR_ERTH        0x0F000000  // Early Rx threshold bits
#define RTL8193_RCR_MULERINT    0x00020000  // Multiple early interrupt select
#define RTL8193_RCR_RER         0x00010000  
#define RTL8193_RCR_RXFTH       0x0000E000  // Rx FIFO Threshold
#define RTL8193_RCR_RBLEN       0x00001800  // Rx Buffer Length
#define RTL8193_RCR_MXDMA       0x00000300  // Max DMA Burst Size 
#define RTL8193_RCR_WRAP0       0x00000040  // Wrap Buffer
#define RTL8193_RCR_AER         0x00000020  // Accept Error Packet
#define RTL8193_RCR_AR          0x00000010  // Accept Runts
#define RTL8193_RCR_AB          0x00000008  // Accept Broadcase Packets
#define RTL8193_RCR_AM          0x00000004  // Accept Multicast Packets
#define RTL8193_RCR_APM         0x00000002  // Accept Physical Match Packets
#define RTL8193_RCR_AAP         0x00000001  // Accept All Packets

// Poszczególne bity rejestru komend
#define RTL8193_COMMAND_REG_RST 0x10        // Reset
#define RTL8193_COMMAND_REG_RE 0x08         // Receiver Enable
#define RTL8193_COMMAND_REG_TE 0x04         // Transmitter Enable
#define RTL8193_COMMAND_REG_BUFE 0x01       // Buffer Rx Empty

// Poszczególe bity rejestru transmit status register
#define RTL8193_TRANSMIT_STATUS_CRS         0x80000000
#define RTL8193_TRANSMIT_STATUS_TABT        0x40000000
#define RTL8193_TRANSMIT_STATUS_OWC         0x20000000
#define RTL8193_TRANSMIT_STATUS_CDH         0x10000000
#define RTL8193_TRANSMIT_STATUS_NCC3_0      0x0F000000
#define RTL8193_TRANSMIT_STATUS_ERTHTH5_0   0x003F0000
#define RTL8193_TRANSMIT_STATUS_TOK         0x00008000      // Ustawiany na 1 po całkowitym wysłaniu pakiety
#define RTL8193_TRANSMIT_STATUS_TUN         0x00004000
#define RTL8193_TRANSMIT_STATUS_OWN         0x00002000      // Ustawiany na 1 po przeniesieniu danych do pamięci karty sieciowej
#define RTL8193_TRANSMIT_STATUS_SIZE        0x00001FFF      // Całkowity ozmiar wysyłanych danych

// Rozmiar buforu
#define RTL8139_RECEIVE_BUFFER_SIZE 0x25EC
#define RTL8139_TRANSMITER_BUFFER_SIZE 0x2010

// Bit bus mastering
#define RTL8139_COMMAND_REGISTER_BUS_MASTER_BIT 0x4

// Długość pakietu
#define RTL8139_PACKET_LENGTH 0xFFFF

// Nagłówek dopisywany przed każdym pakietem
struct RTL8139_header
{
    uint16_t receive_status;
    uint16_t length;
};

// Prototypy
void RTL8139_initialize(void);
void RTL8139_send_packet(void *ptr, uint32_t size);
void *RTL8139_get_packet(uint32_t *size);

#endif