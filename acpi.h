#ifndef __ACPI_H__
#define __ACPI_H__

// Miejsca gdzie może znajdować się struktura RSDP
#define SEARCH1_START 0x1
#define SEARCH1_END 0x400
#define SEARCH2_START 0x000E0000
#define SEARCH2_END 0x000FFFFF

// Zawartość pola revision mówi o wersji ACPI
#define ACPI10 0
#define ACPI20 2

// RSDP w wersji ACPI 1.0
struct RSDP_descriptor {
    char signature[8];
    uint8_t checksum;
    char OEMID[6];
    uint8_t revision;
    uint32_t rsdt_address;
} __attribute__ ((packed));

// RSDP w wersji ACPI 2.0
struct RSDP_descriptor20 {
    struct RSDP_descriptor first_part;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__ ((packed));

// Wpis w tablicy RSDT
struct ACPISDT_header {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char OEMID[6];
    char OEM_table_ID[8];
    uint32_t OEM_revision;
    uint32_t creator_ID;
    uint32_t creator_revision;
};

// Jakaś struktura wykorzystywana przez ACPI
struct GenericAddressStructure
{
  uint8_t AddressSpace;
  uint8_t BitWidth;
  uint8_t BitOffset;
  uint8_t AccessSize;
  uint64_t Address;
};

// Tablica Fixed ACPI Description Table - chcemy sprawdzić obsługę myszy
struct FADT
{
    struct   ACPISDT_header h;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;
    uint8_t  Reserved;
    uint8_t  PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t  DutyOffset;
    uint8_t  DutyWidth;
    uint8_t  DayAlarm;
    uint8_t  MonthAlarm;
    uint8_t  Century;
 
    // ACPI 2
    uint16_t BootArchitectureFlags;
 
    uint8_t  Reserved2;
    uint32_t Flags;
 
    struct GenericAddressStructure ResetReg;
 
    uint8_t  ResetValue;
    uint8_t  Reserved3[3];
 
    // ACPI 2
    uint64_t X_FirmwareControl;
    uint64_t X_Dsdt;
 
    struct GenericAddressStructure X_PM1aEventBlock;
    struct GenericAddressStructure X_PM1bEventBlock;
    struct GenericAddressStructure X_PM1aControlBlock;
    struct GenericAddressStructure X_PM1bControlBlock;
    struct GenericAddressStructure X_PM2ControlBlock;
    struct GenericAddressStructure X_PMTimerBlock;
    struct GenericAddressStructure X_GPE0Block;
    struct GenericAddressStructure X_GPE1Block;
};

// Prototypy
void acpi_initialize(void);
int get_ACPI_version(void);
uint32_t find_table(const char *signature);

#endif