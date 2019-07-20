#include <stddef.h>
#include <stdint.h>
#include "keyboard.h"
#include "ports.h"
#include "buffer.h"
#include "ps2.h"
#include "idt.h"
#include "sys.h"
#include "clib/stdio.h"

#define NOSHIFT_CODE 0
#define SHIFT_CODE 1

// Funkcje statyczne
static int keyboard_is_printable(unsigned char c);
static char convert_scancode_to_ascii(unsigned char c, int shift);
static void keyboard_set_led(int ScrollLock, int NumberLock, int CapsLock);
static void keyboard_set_typematic(unsigned int rate, unsigned int delay);

//Tabela służąca do zamiany kodów skaningowych na kody ascii
unsigned char translation_table[0x100][2] = {
    [0x1E] = {'a', 'A'}, [0x30] = {'b', 'B'}, [0x2E] = {'c', 'C'} , [0x20] = {'d', 'D'}, [0x12] = {'e', 'E'},
    [0x21] = {'f', 'F'}, [0x22] = {'g', 'G'}, [0x23] = {'h', 'H'} , [0x17] = {'i', 'I'}, [0x24] = {'j', 'J'},
    [0x25] = {'k', 'K'}, [0x26] = {'l', 'L'}, [0x32] = {'m', 'M'} , [0x31] = {'n', 'N'}, [0x18] = {'o', 'O'},
    [0x19] = {'p', 'P'}, [0x10] = {'q', 'Q'}, [0x13] = {'r', 'R'} , [0x1F] = {'s', 'S'}, [0x14] = {'t', 'T'},
    [0x16] = {'u', 'U'}, [0x2F] = {'v', 'V'}, [0x11] = {'w', 'W'} , [0x2D] = {'x', 'X'}, [0x15] = {'y', 'Y'},
    [0x2c] = {'z', 'Z'}, [0x0B] = {'0', ')'}, [0x02] = {'1', '!'} , [0x03] = {'2', '@'}, [0x04] = {'3', '#'},
    [0x05] = {'4', '$'}, [0x06] = {'5', '%'}, [0x07] = {'6', '^'} , [0x08] = {'7', '&'}, [0x09] = {'8', '*'},
    [0x0A] = {'9', '('}, [0x39] = {' ', ' '}, [0x52] = {'0', '0'} , [0x4F] = {'1', '1'}, [0x50] = {'2', '2'},
    [0x51] = {'3', '3'}, [0x4B] = {'4', '4'}, [0x4C] = {'5', '5'} , [0x4D] = {'6', '6'}, [0x47] = {'7', '7'},
    [0x48] = {'8', '8'}, [0x49] = {'9', '9'}, [0x8D] = {'=', '+'} , [0x0C] = {'-', '_'}, [0x1A] = {'[', '{'},
    [0x34] = {'.', '>'}, [0x35] = {'/', '?'}, [0x37] = {'*', '*'} , [0x4A] = {'-', '-'}, [0x4E] = {'+', '+'},
    [0x1B] = {']', '}'}, [0x27] = {';', ':'}, [0x28] = {'\'', '"'}, [0x2B] = {'\\', '|'}, [0x33] = {',', '<'},
    [0x53] = {',', ','}
};

// Flagi mówiące czy któryś z tych klawiszy jest wciśnięty
int lshift_flag = 0;
int rshift_flag = 0;

int capslock_flag = 0;
int numlock_flag = 0;
int scrolllock_flag = 0;

// Stan "Automatu"
int state = 0;

// Bufor klawiatury
struct buffer_t keyboard_buffer;

// Inicjalizacja klawiatury
void keyboard_initialize(void)
{
    buffer_initialize(&keyboard_buffer);

    asm("cli");

    // Wyłączamy urządzeń, żeby nie przeszkadzały w konfiguracji
    ps2_write_command(COMMAND_DISABLE_FIRST_PORT);
    ps2_write_command(COMMAND_DISABLE_SECOND_PORT);

    // Włączenie przerwania klawiatury - chociaż zazwyczaj jest już włączone
    uint8_t config_byte = ps2_get_config_byte();
    config_byte |= CONFIG_FIRST_PORT_INTERRUPT;
    ps2_set_config_byte(config_byte);

    // Zmiana częstotliwości powtarzania klawiszy
    keyboard_set_typematic(REPEAT_RATE_2HZ, DELAY_500);

    // Wykonanie testu klawiatury
    ps2_write_command(COMMAND_TEST_FIRST_PORT);
    uint8_t test_result = ps2_read_data();
    if(test_result ==  DEVICE_TEST_PASSED) printf("PS2 Keyboard Test Passed\n");
    else report_error("PS2 Keyboard Test Failed");

    // Włączenie urządzeń po skończeniu konfiguracji
    ps2_write_command(COMMAND_ENABLE_FIRST_PORT);
    ps2_write_command(COMMAND_ENABLE_SECOND_PORT);
    
    asm("sti");

    interrupt_register(33, keyboard_interrupt_handler);
    printf("Keyboard ready\n");
}

// Zwraca bufor klawiatury
struct buffer_t *keyboard_get_buffer(void)
{
    return &keyboard_buffer;
}

// Procedura obsługi przerwania klawiatury
void keyboard_interrupt_handler(void)
{
    // Odczytuje kod skaningowy z buforu
    unsigned char scancode = inportb(PS2_DATA_PORT);

    // Sekwencja kodów, zakładamy max 2
    if(scancode == VK_EXTRA_CODE)
    {
        state = 1;
        return;
    }

    if(state)
    {
        state = 0;
    }

    else
    {
        // Aktualizuję flagi klawiszy specialnych
        if(scancode == VK_LEFT_SHIFT)
            lshift_flag = 1;
        else if(scancode == RELEASED(VK_LEFT_SHIFT))
            lshift_flag = 0;
        else if(scancode == VK_RIGHT_SHIFT)
            rshift_flag = 1;
        else if(scancode == RELEASED(VK_RIGHT_SHIFT))
            rshift_flag = 0;
        else if(scancode == VK_CAPS_LOCK)
            capslock_flag = !capslock_flag;
        else if(scancode == VK_NUMBER_LOCK)
            numlock_flag = !numlock_flag;
        else if(scancode == VK_SCROLL_LOCK)
            scrolllock_flag = !scrolllock_flag;

        // Aktualizuje diody LED
        if(scancode == VK_CAPS_LOCK || scancode == VK_NUMBER_LOCK || scancode == VK_SCROLL_LOCK)
            keyboard_set_led(scrolllock_flag, numlock_flag, capslock_flag);

        // Umieszczanie w buforze drukowalnych znaków
        if(keyboard_is_printable(scancode))
        {
            char character = convert_scancode_to_ascii(scancode, lshift_flag | rshift_flag | capslock_flag);
            buffer_put(&keyboard_buffer, character);
        }

        if(scancode == VK_ENTER)
            buffer_put(&keyboard_buffer, '\n');
    }
}

// Ustawia szybkość i opóźnienie powtarzania klawiszy
// Funkcja nie daje efektu, chociaż kontroler potwierdza wykonanie. Zakładam, że to wina systemu hosta, który na to nie pozwala.
static void keyboard_set_typematic(unsigned int rate, unsigned int delay)
{
    uint8_t typematic_byte = rate || delay << 4;

    ps2_write_command(COMMAND_KB_SET_TYPEMATIC);
    ps2_write_data(typematic_byte);
    uint8_t byte = ps2_read_data();
}

// Steruje diodami LED - zakładam że działa, w laptopie nie mam diód 
static void keyboard_set_led(int ScrollLock, int NumberLock, int CapsLock)
{
    uint8_t led_byte = ScrollLock | NumberLock << 1 | CapsLock << 2;

    ps2_write_command(COMMAND_KB_SET_LED);
    ps2_write_data(led_byte);
    ps2_read_data();
}

// Sprawdza czy podany kod skaningowy odpowiada klawiszowi który da się wydrukować
static int keyboard_is_printable(unsigned char c)
{
    if(translation_table[c][0]!=0) return 1;
    return 0;
}

// Zamienia kod skaningowy na kod ascii
static char convert_scancode_to_ascii(unsigned char c, int shift)
{
    if(shift)
        return translation_table[c][SHIFT_CODE];
    else
        return translation_table[c][NOSHIFT_CODE];
    return 0;
}

