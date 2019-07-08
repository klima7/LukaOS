#include "keyboard.h"
#include "ports.h"
#include "buffer.h"
#include "clib/stdio.h"
#include <stddef.h>
#include <stdint.h>

#define CODES_SIZE 62

#define SCAN_CODE 2
#define SHIFT_CODE 1
#define NOSHIFT_CODE 0

//Tabela służąca do zamiany kodów skaningowych na kody ascii
unsigned char translation_table[CODES_SIZE][3] = {
    {'a', 'A', 0x1E}, {'b', 'B', 0x30}, {'c', 'C', 0x2E}, {'d', 'D', 0x20}, {'e', 'E', 0x12},
    {'f', 'F', 0x21}, {'g', 'G', 0x22}, {'h', 'H', 0x23}, {'i', 'I', 0x17}, {'j', 'J', 0x24},
    {'k', 'K', 0x25}, {'l', 'L', 0x26}, {'m', 'M', 0x32}, {'n', 'N', 0x31}, {'o', 'O', 0x18},
    {'p', 'P', 0x19}, {'q', 'Q', 0x10}, {'r', 'R', 0x13}, {'s', 'S', 0x1F}, {'t', 'T', 0x14},
    {'u', 'U', 0x16}, {'v', 'V', 0x2F}, {'w', 'W', 0x11}, {'x', 'X', 0x2D}, {'y', 'Y', 0x15},
    {'z', 'Z', 0x2c}, {'0', ')', 0x0B}, {'1', '!', 0x02}, {'2', '@', 0x03}, {'3', '#', 0x04},
    {'4', '$', 0x05}, {'5', '%', 0x06}, {'6', '^', 0x07}, {'7', '&', 0x08}, {'8', '*', 0x09},
    {'9', '(', 0x0A}, {' ', ' ', 0x39}, {'0', '0', 0x52}, {'1', '1', 0x4F}, {'2', '2', 0x50},
    {'3', '3', 0x51}, {'4', '4', 0x4B}, {'5', '5', 0x4C}, {'6', '6', 0x4D}, {'7', '7', 0x47},
    {'8', '8', 0x48}, {'9', '9', 0x49}, {'=', '+', 0x8D}, {'-', '_', 0x0C}, {'[', '{', 0x1A},
    {'.', '>', 0x34}, {'/', '?', 0x35}, {'*', '*', 0x37}, {'-', '-', 0x4A}, {'+', '+', 0x4E},
    {']', '}', 0x1B}, {';', ':', 0x27}, {'\'', '"', 0x28}, {'\\', '|', 0x2B}, {',', '<', 0x33},
    {',', ',', 0x53}, {'x', 'x', 0xE0}
};

// Flagi mówiące czy któryś z tych klawiszy jest wciśnięty
int lshift_mode = 0;
int rshift_mode = 0;
int capslock_mode = 0;
int numlock_mode = 0;

// Stan "Automatu"
int state = 0;

// Bufor klawiatury
struct buffer_t keyboard_buffer;

// Inicjalizacja klawiatury
void keyboard_initialize(void)
{
    printf("Keyboard Initialization\n");
    buffer_initialize(&keyboard_buffer);
}

// Sprawdza czy podany kod skaningowy odpowiada klawiszowi który da się wydrukować
int keyboard_is_printable(unsigned char c)
{
    for(int i=0; i<CODES_SIZE; i++)
    {
        if(c==translation_table[i][SCAN_CODE])
            return 1;
    }
    return 0;
}

// Zamienia kod skaningowy na kod ascii
char convert_scancode_to_ascii(unsigned char c, int shift)
{
    for(int i=0; i<CODES_SIZE; i++)
    {
        if(c==translation_table[i][SCAN_CODE])
        {
            if(shift)
                return translation_table[i][SHIFT_CODE];
            else
                return translation_table[i][NOSHIFT_CODE];
        }
    }
    return 0;
}

// Procedura obsługi przerwania klawiatury
void keyboard_interrupt_handler(void)
{
    // Odczytuje kod skaningowy z buforu
    unsigned char scancode = inportb(0x60);

    // Sekwencja kodów, zakładamy max 2
    if(scancode == EXTRA_CODE)
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
        if(scancode == LEFT_SHIFT)
            lshift_mode = 1;
        else if(scancode == RELEASE(LEFT_SHIFT))
            lshift_mode = 0;
        else if(scancode == RIGHT_SHIFT)
            rshift_mode = 1;
        else if(scancode == RELEASE(RIGHT_SHIFT))
            rshift_mode = 0;
        else if(scancode == CAPS_LOCK)
            capslock_mode = !capslock_mode;
        else if(scancode == NUM_LOCK)
            numlock_mode = !numlock_mode;

        // Umieszczanie w buforze drukowalnych znaków
        if(keyboard_is_printable(scancode))
        {
            char character = convert_scancode_to_ascii(scancode, lshift_mode | rshift_mode | capslock_mode);
            buffer_put(&keyboard_buffer, character);
        }

        if(scancode == ENTER)
            buffer_put(&keyboard_buffer, '\n');
    }
}


