#include <stddef.h>
#include <stdint.h>
#include "cmos.h"
#include "clock.h"
#include "ports.h"

// Czy przerwania nmi mają był włączone
uint8_t nmi_enabled = 0;

// Odczytuja wartość z rejestru CMOS
uint8_t cmos_read_register(uint8_t address)
{
    uint8_t byte1 = nmi_enabled << 7 | address;
    outportb(CMOS_PORT1, byte1);

    uint8_t value = inportb(CMOS_PORT2);
    return value;
}

// Zapisuje wartość do rejestru CMOS
void cmos_write_register(uint8_t address, uint8_t value)
{
    uint8_t byte1 = nmi_enabled << 7 | address;
    outportb(CMOS_PORT1, byte1);

    outportb(CMOS_PORT2, value);
}

// Włącza przerwania NMI - Not Maskable Interrupt
void cmos_enable_nmi(void)
{
    nmi_enabled = 1;
}

// Wyłącza przerwania NMI - Not Maskable Interrupt
void cmos_disable_nmi(void)
{
    nmi_enabled = 0;
}