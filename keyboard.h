#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "uni_list.h"

// Kilka przydanych kodów skaningowych
#define VK_EXTRA_CODE 0xE0
#define VK_LEFT_SHIFT 0x2A
#define VK_RIGHT_SHIFT 0x36
#define VK_SPACE 0x07
#define VK_CAPS_LOCK 0x3A
#define VK_SCROLL_LOCK 0x46
#define VK_NUMBER_LOCK 0x45
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
#define VK_ESCAPE 0x01

// Możliwe opóżnienia powtarzania klawiszy
#define DELAY_250 0x1
#define DELAY_500 0x2
#define DELAY_750 0x3
#define DELAY_1000 0x4

// Przykładowe częstotliwości powtarzania
// Polegją na zmapowaniu 0x00-0x1F na 30Hz-2Hz
#define REPEAT_RATE_2HZ 0x1F

// Zamienia kod naciśnięcia na kod zwolnienia klawisza
#define RELEASED(_X) ((uint8_t)(_X) | 0x80)

// Wzór słuchacz czekającego na zdarzenie
typedef void (*kb_listener_t)(void);

// Lista funkcji będących słuchaczami zdarzeń
UNI_LIST_H(kblistener, kb_listener_t)

// Prototypy
void keyboard_initialize(void);
struct buffer_t *keyboard_get_buffer(void);
void keyboard_interrupt_handler(void);

void keyboard_register_listener(uint8_t scancode, int extra, kb_listener_t listener);
void keyboard_unregister_listener(uint8_t scancode, int extra, kb_listener_t listener);

#endif