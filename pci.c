#include <stddef.h>
#include <stdint.h>
#include "pci.h"
#include "ports.h"
#include "heap.h"
#include "shell.h"
#include "clib/stdio.h"

// Funkcje statyczne
static void pci_command_devicelist(const char* tokens, uint32_t tokens_count);
static void display_binary_funcs(uint8_t funcs);

// Lista z dowiązaniami, nie zmienia się ona w czasie więc jest znacznie uproszczona
struct pci_entry_t *devices_list = NULL;

void pci_initialize(void)
{
    printf("PCI Initialization\n");
    devices_list = scan_devices();
    pci_display_devices();
    register_command("devicelist", "Display list of all available devices", pci_command_devicelist);
}

// Odczytuje 4 bajty z przestrzeni konfiguracyjnej danego urządzenia
uint32_t pci_read(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset)
{
    uint32_t address = 0x80000000 | bus << 16 | slot << 11 | func << 8 | offset;
    outportdw(PCI_PORT_CONFIG_ADDRESS, address);
    uint32_t value = inportdw(PCI_PORT_CONFIG_DATA);
    return value;
}

// Zapisuje 4 bajty do przestrzeni konfiguracyjnej danego urządzenia
void pci_write(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset, uint32_t value)
{
    uint32_t address = 0x80000000 | bus << 16 | slot << 11 | func << 8 | offset;
    outportdw(PCI_PORT_CONFIG_ADDRESS, address);
    outportdw(PCI_PORT_CONFIG_DATA, value);
}

// Zwraca typ urządzenia
uint8_t pci_get_class_code(uint32_t bus, uint32_t slot, uint32_t func)
{
    uint32_t reg = pci_read(bus, slot, func, 0x08);
    uint8_t class = (reg & 0xFF000000) >> 24;
    return class;
}

// Zwraca podtyp urządzenia
uint8_t pci_get_subclass_code(uint32_t bus, uint32_t slot, uint32_t func)
{
    uint32_t reg = pci_read(bus, slot, func, 0x08);
    uint8_t subclass = (reg & 0x00FF0000) >> 16;
    return subclass;
}

// Zwraca pod-podtyp urządzenia
uint8_t pci_get_prog_if(uint32_t bus, uint32_t slot, uint32_t func)
{
    uint32_t reg = pci_read(bus, slot, func, 0x08);
    uint8_t prog = (reg & 0x0000FF00) >> 8;
    return prog;
}

// Zwraca identyfikator id urządzenia
uint16_t pci_get_device_id(uint32_t bus, uint32_t slot, uint32_t func)
{
    uint32_t reg = pci_read(bus, slot, func, 0x00);
    uint16_t id = (reg & 0xFFFF0000) >> 16;
    return id;
}

// Zwraca identyfikator producenta urządzenia
uint16_t pci_get_vendor_id(uint32_t bus, uint32_t slot, uint32_t func)
{
    uint32_t reg = pci_read(bus, slot, func, 0x00);
    uint16_t vendor = (reg & 0x0000FFFF) >> 0;
    return vendor;
}

// Zwraca rejestr poleceń urządzenia
uint16_t pci_get_command_register(uint32_t bus, uint32_t slot, uint32_t func)
{
    uint32_t reg = pci_read(bus, slot, func, 0x04);
    uint16_t command = (reg & 0x0000FFFF) >> 0;
    return command;
}

// Zapisuje rejestr poleceń urządzenia
void pci_save_command_register(uint32_t bus, uint32_t slot, uint32_t func, uint16_t command)
{
    uint32_t reg = pci_read(bus, slot, func, 0x04);
    reg &= 0xFFFF0000;
    reg |= command;
    pci_write( bus, slot, func, 0x04, reg);
}

// Zwraca typ nagłówka
uint8_t pci_get_header_type(uint32_t bus, uint32_t slot, uint32_t func)
{
    uint32_t reg = pci_read(bus, slot, func, 0x0C);
    uint8_t type = (reg & 0x00FF0000) >> 16;
    type &= 0b01111111;
    return type;
}

// Czy dane urządzenie obsługuje wiele funkcji
int pci_is_multi_funcs(uint32_t bus, uint32_t slot, uint32_t func)
{
    uint32_t reg = pci_read(bus, slot, func, 0x0C);
    uint8_t type = (reg & 0x00FF0000) >> 16;
    type &= 0b10000000;
    return !!type;
}

// Sprawdza czy urządzenie istnieje
int pci_check_device_exists(uint32_t bus, uint32_t slot, uint32_t func)
{
    uint32_t read = pci_read(bus, slot, func, 0);
    if(read != 0xFFFFFFFF) return 1;
    return 0;
}

// Analizuje dane urządzenie, alokuje i tworzy opisującą ją strukturę
struct pci_entry_t *pci_analyse_device(uint32_t bus, uint32_t slot)
{
    struct pci_entry_t *entry = (struct pci_entry_t*)kmalloc(sizeof(struct pci_entry_t));
    entry->bus = bus;
    entry->slot = slot;
    entry->device_id = pci_get_device_id(bus, slot, 0);
    entry->vendor_id = pci_get_vendor_id(bus, slot, 0);
    entry->class = pci_get_class_code(bus, slot, 0);
    entry->subclass = pci_get_subclass_code(bus, slot, 0);
    entry->prog_if = pci_get_prog_if(bus, slot, 0);
    entry->funcs = 0b00000001;

    if(pci_is_multi_funcs(bus, slot, 0))
    {
        for(uint32_t i=1; i<8; i++)
        {
            if(pci_get_device_id(bus, slot, i) != 0xFFFF)
                entry->funcs |= 1 << i;
        }
    }

    entry->next = NULL;
    return entry;
}

// Tworzy liste wrzystkich podłączonych urządzeń
struct pci_entry_t *scan_devices(void)
{
    struct pci_entry_t *head = NULL;
    struct pci_entry_t *last = NULL;

    for(uint32_t bus=0; bus<PCI_BUS_COUNT; bus++)
    {
        for(uint32_t dev=0; dev<PCI_DEVICES_COUNT; dev++)
        {
            // Urządzenie nie istnieje
            if(pci_get_device_id(bus, dev, 0) == 0xFFFF) continue;

            // Tworzenie listy
            struct pci_entry_t *device = pci_analyse_device(bus, dev);
            if(last == NULL) head = device;
            else last->next = device;

            last = device;
        }
    }

    return head;
}

// Zwraca liste wszystkich urządzeń
struct pci_entry_t *pci_get_devices(void)
{
    return devices_list;
}

// Zwraca adres struktury opisującej rządane urządzenie
struct pci_entry_t *pci_get_device_by_id(uint16_t device_id)
{
    struct pci_entry_t *current = devices_list;

    while(current != NULL)
    {
        if(current->device_id == device_id)
            return current;
        current = current->next;
    }

    return NULL;
}

// Wyświetla liste wszystkich urządzeń o opisami
void pci_display_devices(void)
{
    struct pci_entry_t *current = devices_list;

    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("| BUS   | SLOT  | CLASS | SUBCL | PROG  | DEV ID   | VENDOR ID | FUNCTIONS\n");
    terminal_setcolor(VGA_COLOR_WHITE);

    while(current!=NULL)
    {
        uint32_t len = printf("| %u", current->bus);
        for(; len<8; len++) printf(" ");

        len = printf("| %u", current->slot);
        for(; len<8; len++) printf(" ");

        len = printf("| %u", current->class);
        for(; len<8; len++) printf(" ");

        len = printf("| %u", current->subclass);
        for(; len<8; len++) printf(" ");

        len = printf("| %u", current->prog_if);
        for(; len<8; len++) printf(" ");

        len = printf("| %x", current->device_id);
        for(; len<11; len++) printf(" ");

        len = printf("| %x", current->vendor_id);
        for(; len<12; len++) printf(" ");

        printf("| ");
        display_binary_funcs(current->funcs);
        printf("\n");

        current = current->next;
    }
    printf("\n");
}

// Wyświetla 8 bitową liczbe binarną, gdzie każdy bit reprezentuje obecność danej funkcji
void display_binary_funcs(uint8_t funcs)
{
	for (int i = 0; i < 8; i++)
	{
		unsigned long long mask = 1ull << (7-i);
		int bit = !(!(mask & funcs));
		putchar('0'+bit);
	}
}

// Komenda devicelist
static void pci_command_devicelist(const char* tokens, uint32_t tokens_count)
{
    pci_display_devices();
}

// Zwraca odpowiecz na pytanie czy rejestr wskazuje na adres w pamięci czy przestrzeni io
enum address_space_t pci_bar_get_space(uint32_t bar_register)
{
    if(bar_register & PCI_BAR_SPACE) return IO_SPACE;
    else return MEMORY_SPACE;
}

// Zwraca adres wskazujący na adres w pamięci lub w przestrzeni io
uint32_t pci_bar_get_address(uint32_t bar_register)
{
    uint32_t space = pci_bar_get_space(bar_register);

    if(space == IO_SPACE) 
        return bar_register & PCI_BAR_IO_ADDRESS;

    else if(space == MEMORY_SPACE)
        return bar_register & PCI_BAR_MEMORY_ADDRESS;

    return 0;
}