#include <stddef.h>
#include <stdint.h>
#include "beeper.h"
#include "clib/stdio.h"
#include "ports.h"
#include "timers.h"
#include "shell.h"
#include "clib/string.h"
#include "clib/stdlib.h"

// Funkcje statyczne
static void beeper_command_beep(const char* tokens, uint32_t tokens_count);

// Inicjalizacja
void beeper_initialize(void)
{
    register_command("beep", "Just beep", beeper_command_beep);
}

// Wydaje dzwięk o podanej częstotliwości
void beep_start(uint32_t frequency)
{
    if(frequency==0)
    {
        silent();
        return;
    }

 	uint32_t div;
 	uint8_t tmp;
 
 	div = 1193180 / frequency;
 	outportb(0x43, 0xb6);
 	outportb(0x42, (uint8_t)div);
 	outportb(0x42, (uint8_t)(div >> 8));

    uint8_t port = inportb(BEEP_PORT);
    outportb(BEEP_PORT, port | BEEP_ON);
}

// Wydaje jednokrotny dzwięk
void beep(void)
{
    beep_start(BEEP_FREQ);
    timer_add(silent, BEEP_LENGTH, 0);
}

// Po prostu cisza
void silent(void)
{
    uint8_t beep_off_mask = ~BEEP_ON;
 	uint8_t tmp = inportb(BEEP_PORT) & beep_off_mask;
 	outportb(BEEP_PORT, tmp);
}

// Komenda wywołująca dzwięk
static void beeper_command_beep(const char* tokens, uint32_t tokens_count)
{
    if(tokens_count==1)
        beep();

    else if(tokens_count==3)
    {
        char *freq_a = (char*)get_token(tokens, 1);
        char *len_a = (char*)get_token(tokens, 2);

        int freq = atoi(freq_a);
        int len = atoi(len_a);

        if(freq<0 || freq > 20000 || len<0)
        {
            terminal_setcolor(VGA_COLOR_LIGHT_RED);
            printf("Invalid arguments!\n");
            return;
        }

        beep_start(freq);
        timer_add(silent, len, 0);
    }

    else
    {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        printf("Invalid arguments count!\n");
        return;
    }
    

    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("BEEP\n");
}