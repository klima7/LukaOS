#include <stddef.h>
#include <stdint.h>
#include "clib/stdio.h"
#include "ps2.h"
#include "acpi.h"
#include "ports.h"

// Funkcje statyczne
static int ps2_check_controller_exist(void);
static int ps2_check_controller_work(void);

// Inicjuje kontroler PS2 - sprawdza, czy jest dostępny i działa
void ps2_initialize(void)
{
    if(!ps2_check_controller_exist() || !ps2_check_controller_work())
    {
        printf("Problem with PS2 Controller\n");
        // Kernel Panic
    }
    else printf("PS2 Controller Ready\n");
}

// Sprawdza za pomocą ACPI czy kontroler PS/2 jest dostępny
static int ps2_check_controller_exist(void)
{
    // W wersji ACPI 1 pole BootArchitectureFlag nie jest wykorzystywane, nie możemy tego sprawdzić i zakładamy że kontroler jest
    if(get_ACPI_version() == ACPI10) return 1;

    // Jeśli ACPI nie posiada takiej strony, to przyjmij że kontroler jest
    struct FADT *fadt = (struct FADT*)find_table("FACP");
    if(fadt==NULL) return 1;

    // Jeśli komputer posiada kontroler PS/2 to drugi bit jest ustawiony, jeśli nie to nie
    uint16_t flags = fadt->BootArchitectureFlags;
    if((flags & 0x2) == 0) return 0;
    return 1;
}

// Sprawdza czy kontroler działa poprawnie
static int ps2_check_controller_work(void)
{
    asm("cli");

    ps2_write_command(COMMAND_DISABLE_FIRST_PORT);
    ps2_write_command(COMMAND_DISABLE_SECOND_PORT);

    ps2_write_command(COMMAND_TEST_CONTROLLER);
    uint8_t test_result = ps2_read_data();

    ps2_write_command(COMMAND_ENABLE_FIRST_PORT);
    ps2_write_command(COMMAND_ENABLE_SECOND_PORT);

    asm("sti");

    if(test_result == PS2_TEST_PASSED) return 1;
    else return 0;
}

// Wysyła komendę po portu komend, gdy wysłanie jest możliwe
// W przyszłości zalecane zastosowanie Timera dla ograniczenia czasu
void ps2_write_command(uint8_t command)
{
	uint8_t status;
	do {
		status = inportb(PS2_COMMAND_PORT);
	} while(status & STATUS_OUTPUT_BUFFER_STATUS);
	return outportb(PS2_COMMAND_PORT, command);
}

// Odczytuje dane z portu danych gdy odczyt jest możliwy
// W przyszłości zalecane zastosowanie Timera dla ograniczenia czasu 
uint8_t ps2_read_data(void)
{
	uint8_t status;
	do {
		status = inportb(PS2_COMMAND_PORT);
	} while(!(status & STATUS_INPUT_BUFFER_STATUS));
	return inportb(PS2_DATA_PORT);
}

// Zapisuje komendę do portu danych, gdy zapis jest możliwy
// W przyszłości zalecane zastosowanie Timera dla ograniczenia czasu 
void ps2_write_data(uint8_t data)
{
	uint8_t status;
	do {
		status = inportb(PS2_COMMAND_PORT);
	} while(status & STATUS_OUTPUT_BUFFER_STATUS);
	return outportb(PS2_DATA_PORT, data);
}

// Wysyła komendę dla myszy
void ps2_write_mouse_command(uint8_t command)
{
    ps2_write_command(COMMAND_MOUSE_PREFIX);
    ps2_write_data(command);
}

// Zwraca bajt konfiguracyjny
uint8_t ps2_get_config_byte(void)
{
    ps2_write_command(COMMAND_READ_CONFIG_BYTE);
    uint8_t byte = ps2_read_data();
    return byte;
}

// Ustala bajt konfiguracyjny
void ps2_set_config_byte(uint8_t byte)
{
    ps2_write_command(COMMAND_SAVE_CONFIG_BYTE);
    ps2_write_data(byte);
}
