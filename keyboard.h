#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

// Kilka przydanych kodów skaningowych
#define VK_EXTRA_CODE 0xE0
#define VK_LEFT_SHIFT 0x2A
#define VK_RIGHT_SHIFT 0x36
#define VK_SPACE 0x07
#define VK_CAPS_LOCK 0x3A
#define VK_NUM_LOCK 0x45
#define VK_ALT 0x36
#define VK_TAB 0x24
#define VK_PAGE_UP 0x49
#define VK_PAGE_DOWN 0x51
#define VK_ARROW_UP 0x48
#define VK_ARROW_DOWN 0x50
#define VK_ARROW_LEFT 0x4B
#define VK_ARROW_RIGHT 0x4D
#define VK_ENTER 0x1C
#define VK_BACKSPACE 0x0E

// Zamienia kod naciśnięcia na kod zwolnienia klawisza
#define RELEASE(_X) ((_X)+128)

// Prototypy
void keyboard_initialize(void);
int keyboard_is_letter(unsigned char c);
int keyboard_is_printable(unsigned char c);
struct buffer_t *keyboard_get_buffer(void);
void keyboard_interrupt_handler(void);

#endif