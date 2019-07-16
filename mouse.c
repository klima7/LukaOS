#include <stddef.h>
#include <stdint.h>
#include "mouse.h"
#include "acpi.h"
#include "ports.h"
#include "ps2.h"
#include "idt.h"
#include "terminal.h"
#include "clib/stdio.h"

// Funkcje statyczne
static uint8_t mouse_get_id_byte(void);
static void mouse_init_scroll_wheel(void);
static void mouse_init_extra_buttons(void);
static void mouse_set_sample_rate(uint8_t rate);
static void mouse_send_data(uint8_t data);

// Aktualny stan myszki 
// Na starcie zakładam pozycje (0,0) W wyniku tego może być ona potem ujemna
struct mouse_t mouse_info = {0};

// Inicjuje mysz
void mouse_initialize(void)
{
    asm("cli");

    // Wyłączamy urządzenia, żeby nie przeszkadzały w konfiguracji
    ps2_write_command(COMMAND_DISABLE_FIRST_PORT);
    ps2_write_command(COMMAND_DISABLE_SECOND_PORT);

    // Włączenie przerwania myszy
    uint8_t config_byte = ps2_get_config_byte();
    config_byte |= CONFIG_SECOND_PORT_INTERRUPT;
    ps2_set_config_byte(config_byte);

    // Włączenie dodatkowych przycisków
    mouse_init_scroll_wheel();
    mouse_init_extra_buttons();

    // Włączenie automatycznej transmisji pakietów
    mouse_send_data(COMMAND_MOUSE_ENABLE_STREAMING);

    // Wykonanie testu myszy
    ps2_write_command(COMMAND_TEST_SECOND_PORT);
    uint8_t test_result = ps2_read_data();
    if(test_result ==  DEVICE_TEST_PASSED) printf("PS2 Mouse Test Passed\n");
    else
    {
        printf("PS2 Mouse Test Failed\n");
        // Kernel Panic
    }

    // Włączenie urządzeń po skończeniu konfiguracji
    ps2_write_command(COMMAND_ENABLE_FIRST_PORT);
    ps2_write_command(COMMAND_ENABLE_SECOND_PORT);

    asm("sti");

    interrupt_register(44, mouse_interrupt_handler);
    printf("Mouse Ready\n");
}

// Funkcja obsługująca przerwanie myszy
void mouse_interrupt_handler(void)
{
    // Nie wiem czemu, ale każda akcja myszki generuje dwa przerwania, tylko pierwsze jest poprawne
    // Nie udało mi się znaleść przyczyny powstawania fałszywego przerwania, więc musze je teraz przefiltrować
    uint8_t status =  inportb(PS2_COMMAND_PORT);
    if(!(status & STATUS_MOUSE_DATA_AVAILABLE)) return;

    // Ignorowanie błędnych pakietów
    if(status & BYTE1_Y_OVERFLOW || status & BYTE1_X_OVERFLOW) return;

    // Pobranie paczki z portu
    uint32_t b1 = inportb(PS2_DATA_PORT);
    uint32_t b2 = inportb(PS2_DATA_PORT);
    uint32_t b3 = inportb(PS2_DATA_PORT);
    uint32_t b4 = inportb(PS2_DATA_PORT);

    // Poprawka na wartości ujemne
    if(b1 & BYTE1_X_SIGN) b2 = b2 | 0xFFFFFF00;
    if(b1 & BYTE1_Y_SIGN) b3 = b3 | 0xFFFFFF00;

    // Aktualizacja stanu myszki
    mouse_info.posx += b2;
    mouse_info.posy += b3;

    if(b1 & BYTE1_LEFT_BTN) mouse_info.left_button = 1;
    else mouse_info.left_button = 0;

    if(b1 & BYTE1_MIDDLE_BTN) mouse_info.middle_button = 1;
    else mouse_info.middle_button = 0;

    if(b1 & BYTE1_RIGHT_BTN) mouse_info.right_button = 1;
    else mouse_info.right_button = 0;

    if(b4 & BYTE4_EXTRA_BUTTON1) mouse_info.extra_button1 = 1;
    else mouse_info.extra_button1 = 0;

    if(b4 & BYTE4_EXTRA_BUTTON2) mouse_info.extra_button2 = 1;
    else mouse_info.extra_button2 = 0;

    uint32_t wheel = b4 & BYTE4_Z_MOVEMENT;
    if(wheel == 15) mouse_info.scroll_wheel--;
    else if(wheel == WHEEL_SCROLL_UP) mouse_info.scroll_wheel++;
}

// Wyświetla aktualny stan myszki
void debug_display_mouse(void)
{
    printf("L: %d | R: %d | M: %d | E1: %d | E2: %d\n", mouse_info.left_button, mouse_info.right_button, mouse_info.middle_button, mouse_info.extra_button1, mouse_info.extra_button2);
    printf("Pos X: %d\n", mouse_info.posx);
    printf("Pos Y: %d\n", mouse_info.posy);
    printf("Pos Z: %d\n", mouse_info.scroll_wheel);
}

// Zwraca strukture informacji o myszce
struct mouse_t *get_mouse_info(void)
{
    return &mouse_info;
}

// Zwraca aktualne ID myszki
static uint8_t mouse_get_id_byte(void)
{
    mouse_send_data(COMMAND_MOUSE_GET_ID);
    return ps2_read_data();
}

// Aktywuje kółko myszki
static void mouse_init_scroll_wheel(void)
{
    mouse_set_sample_rate(200);
    mouse_set_sample_rate(100);
    mouse_set_sample_rate(80);
}

// Aktywuje dodatkowe przycisi
static void mouse_init_extra_buttons(void)
{
    mouse_set_sample_rate(200);
    mouse_set_sample_rate(200);
    mouse_set_sample_rate(80);
}

// Ustala częstotliwość wystyłania paczek
static void mouse_set_sample_rate(uint8_t rate)
{
    mouse_send_data(COMMAND_MOUSE_SET_SAMPLE_RATE);
    mouse_send_data(rate);
}

// Wysyła dane do myszy
static void mouse_send_data(uint8_t data)
{
    ps2_write_command(COMMAND_MOUSE_PREFIX);
    ps2_write_data(data);
    ps2_read_data();
}






