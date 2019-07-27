#include <stddef.h>
#include <stdint.h>
#include "mouse_test.h"
#include "../clock.h"
#include "../mouse.h"
#include "../keyboard.h"
#include "../buffer.h"
#include "../terminal.h"
#include "../clib/stdio.h"

// Główna funkcja
int mouse_test_main(const char* argv, uint32_t argc)
{   

    struct buffer_t *keyboard_buffer = keyboard_get_buffer();

    while(buffer_isempty(keyboard_buffer))
    {
        terminal_clear();
        debug_display_mouse();
        terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
        printf("Press Any Key To Exit\n");
        sleep(50);
    }   

    buffer_get(keyboard_buffer);

    return 0;
}