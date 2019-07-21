#ifndef __CMOS_H__
#define __CMOS_H__

#include <stddef.h>
#include <stdint.h>

// Porty umożliwiające dostęp do CMOS
#define CMOS_PORT1 0x70
#define CMOS_PORT2 0x71

// Rejestry RTC
#define CMOS_REGISTER_SECONDS 0x00
#define CMOS_REGISTER_MINUTES 0x02
#define CMOS_REGISTER_HOURS 0x04
#define CMOS_REGISTER_WEEKDAY 0x06
#define CMOS_REGISTER_DAY_OF_MONTH 0x07
#define CMOS_REGISTER_MONTH 0x08
#define CMOS_REGISTER_YEAR 0x09
#define CMOS_REGISTER_CENTURY 0x32
#define CMOS_REGISTER_STATUS_A 0x0A
#define CMOS_REGISTER_STATUS_B 0x0B

// Zawiera informacje o zainstalowanej stacji dyskietek
#define CMOS_REGISTER_FLOPPY_DRIVE 0x10

// Prototypy
uint8_t cmos_read_register(uint8_t address);
void cmos_write_register(uint8_t address, uint8_t value);

#endif