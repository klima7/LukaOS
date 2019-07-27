#ifndef __PCI_H__
#define __PCI_H__

#include <stddef.h>
#include <stdint.h>

// Adresy portów obsługujących PCI
#define PCI_PORT_CONFIG_ADDRESS 0x0CF8
#define PCI_PORT_CONFIG_DATA 0x0CFC

// Bajt typu nagłówka - header_type
#define PCI_MULTIPLY_FUNCTIONS 0x80
#define PCI_HEADER_TYPE 0x7F

// Możliwa liczba szyn i urządzeń wpiętych do jednej szyny
#define PCI_BUS_COUNT 256
#define PCI_DEVICES_COUNT 32

// Struktura służąca do przechowywania informacji o jednym urządzeniu
struct pci_entry_t{
    uint8_t bus;
    uint8_t slot;
    uint16_t device_id;
    uint16_t vendor_id;
    uint8_t class;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t funcs;

    struct pci_entry_t *next;
};

// Prototypy 
void pci_initialize(void);

uint32_t pci_read(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset);
void pci_write(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset, uint32_t value);

int pci_check_device_exists(uint32_t bus, uint32_t slot, uint32_t func);

uint8_t pci_get_header_type(uint32_t bus, uint32_t slot, uint32_t func);
int pci_is_multi_funcs(uint32_t bus, uint32_t slot, uint32_t func);

uint8_t pci_get_class_code(uint32_t bus, uint32_t slot, uint32_t func);
uint8_t pci_get_subclass_code(uint32_t bus, uint32_t slot, uint32_t func);
uint8_t pci_get_prog_if(uint32_t bus, uint32_t slot, uint32_t func);

uint16_t pci_get_device_id(uint32_t bus, uint32_t slot, uint32_t func);
uint16_t pci_get_vendor_id(uint32_t bus, uint32_t slot, uint32_t func);

uint16_t pci_get_command_register(uint32_t bus, uint32_t slot, uint32_t func);
void pci_save_command_register(uint32_t bus, uint32_t slot, uint32_t func, uint16_t command);

struct pci_entry_t *pci_analyse_device(uint32_t bus, uint32_t slot);
struct pci_entry_t *scan_devices(void);

void pci_display_devices(void);

void display_binary_funcs(uint8_t funcs);

#endif