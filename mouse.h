#ifndef __MOUSE_H__
#define __MOUSE_H__

#include <stddef.h>
#include <stdint.h>

// Pierwszy bajt pakietu od myszy
#define BYTE1_LEFT_BTN 0x1
#define BYTE1_RIGHT_BTN 0x2
#define BYTE1_MIDDLE_BTN 0x4
#define BYTE1_ALWAYS_ONE 0x8
#define BYTE1_X_SIGN 0x10
#define BYTE1_Y_SIGN 0x20
#define BYTE1_X_OVERFLOW 0x40
#define BYTE1_Y_OVERFLOW 0x80

// Czwarty bajt pakietu od myszy
#define BYTE4_Z_MOVEMENT 0x0F
#define BYTE4_EXTRA_BUTTON1 0x10
#define BYTE4_EXTRA_BUTTON2 0x20
#define BYTE4_UNUSED 0xC0

// Możliwe stany kółka myszy
#define WHEEL_NO_SCROLL 0x0
#define WHEEL_SCROLL_UP 0x1
#define WHEEL_SCROLL_DOWN 0xF

// Możliwe wartości bajta id
#define MOUSE_ID_STANDARD 1
#define MOUSE_ID_WHEEL 3
#define MOUSE_ID_EXTRA_BUTTONS 4

// Struktura reprezentująca aktualny stan myszki
struct mouse_t
{
    int posx;
    int posy;

    int left_button;
    int right_button;
    int middle_button;

    int scroll_wheel;
    int extra_button1;
    int extra_button2;
};

// Prototypy
void mouse_initialize(void);
void mouse_interrupt_handler(void);
void debug_display_mouse(void);
struct mouse_t *get_mouse_info(void);

#endif