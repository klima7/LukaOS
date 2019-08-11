
#include <stddef.h>
#include <stdint.h>
#include "launcher.h"
#include "shell.h"
#include "terminal.h"
#include "clib/stdio.h"
#include "clib/string.h"

#define PROGRAMS_COUNT 3

// Dołączamy wszystkie programy
#include "programs/thread_test.h"
#include "programs/mouse_test.h"
#include "programs/panic_test.h"

// Funkcje statyczne
static void launcher_add(const char *name, program_main_t main_fun);
static void launcher_command_start(const char* argv, uint32_t argc);

// Tablica zawierająca wszystkie programu
struct program_entry_t programs[PROGRAMS_COUNT];

// Inicjalizuje
void launcher_initialize(void)
{
    // Dodawanie wszystkich programów po jednym
    launcher_add("thread_test", thread_test_main);
    launcher_add("mouse_test", mouse_test_main);
    launcher_add("panic_test", panic_test_main);

    register_command("start", "Starts given program or display all programs", launcher_command_start);
}

// Uruchamia podany program z argumentami
void launcher_start(const char *name, const char *argv, uint32_t argc)
{
    for(uint32_t i=0; i<PROGRAMS_COUNT; i++)
    {
        if(strcmp(programs[i].program_name, name) == 0)
        {
            program_main_t main = programs[i].program_main;
            int code = main(argv, argc);
            
            terminal_setcolor(VGA_COLOR_LIGHT_BLUE);
            printf("\nProgram exited with code: %d\n", code);

            return;
        }
    }

    terminal_setcolor(VGA_COLOR_LIGHT_RED);
    printf("Program Not Found\n");
}

// Dodaje program do listy programów, tylko funkcja pomocnicza
static void launcher_add(const char *name, program_main_t main_fun)
{
    static uint32_t current_count = 0;
    programs[current_count].program_name = name;
    programs[current_count].program_main = main_fun;
    current_count++;
}

// Komenda start 
static void launcher_command_start(const char* argv, uint32_t argc)
{
    if(argc == 1)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
        printf("Programs list: ");
        terminal_setcolor(VGA_COLOR_WHITE);

        for(uint32_t i=0; i<PROGRAMS_COUNT; i++)
        {
            printf("%s", programs[i].program_name);
            if(i != PROGRAMS_COUNT-1) printf(", ");
            else printf("\n");
        }
    }

    else
    {
        const char *program_name = get_token(argv, 1);
        launcher_start(program_name, argv, argc);
    }
    
}