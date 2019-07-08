#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

// Kilka przydanych kodów skaningowych
#define EXTRA_CODE 0xE0
#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define SPACE 0x07
#define CAPS_LOCK 0x3A
#define NUM_LOCK 0x45
#define ALT 0x36
#define TAB 0x24
#define PAGE_UP 0x49
#define PAGE_DOWN 0x51
#define ARROW_UP 0x48
#define ARROW_DOWN 0x50
#define ARROW_LEFT 0x4B
#define ARROW_RIGHT 0x4D
#define ENTER 0x1C
#define BACKSPACE 0x0E

// Zamienia kod naciśnięcia na kod zwolnienia klawisza
#define RELEASE(_X) ((_X)+128)

// Prototypy
void keyboard_initialize(void);
int keyboard_is_letter(unsigned char c);
int keyboard_is_printable(unsigned char c);
void keyboard_interrupt_handler(void);

// Wszystkie programy mają dostęp do zawartości bufora
extern struct buffer_t keyboard_buffer;

#endif