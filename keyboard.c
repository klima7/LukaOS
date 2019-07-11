#include <stddef.h>
#include <stdint.h>
#include "keyboard.h"
#include "ports.h"
#include "buffer.h"
#include "clib/stdio.h"

#define NOSHIFT_CODE 0
#define SHIFT_CODE 1

//Tabela służąca do zamiany kodów skaningowych na kody ascii
unsigned char translation_table[0xFF][2] = {
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
    if(translation_table[c][0]!=0) return 1;
    return 0;
}

// Zamienia kod skaningowy na kod ascii
char convert_scancode_to_ascii(unsigned char c, int shift)
{
    if(shift)
        return translation_table[c][SHIFT_CODE];
    else
        return translation_table[c][NOSHIFT_CODE];
    return 0;
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
    unsigned char scancode = inportb(0x60);

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
            lshift_mode = 1;
        else if(scancode == RELEASE(VK_LEFT_SHIFT))
            lshift_mode = 0;
        else if(scancode == VK_RIGHT_SHIFT)
            rshift_mode = 1;
        else if(scancode == RELEASE(VK_RIGHT_SHIFT))
            rshift_mode = 0;
        else if(scancode == VK_CAPS_LOCK)
            capslock_mode = !capslock_mode;
        else if(scancode == VK_NUM_LOCK)
            numlock_mode = !numlock_mode;

        // Umieszczanie w buforze drukowalnych znaków
        if(keyboard_is_printable(scancode))
        {
            char character = convert_scancode_to_ascii(scancode, lshift_mode | rshift_mode | capslock_mode);
            buffer_put(&keyboard_buffer, character);
        }

        if(scancode == VK_ENTER)
            buffer_put(&keyboard_buffer, '\n');
    }
}


