#include <stddef.h>
#include <stdint.h>
#include "shell.h"
#include "terminal.h"
#include "clib/stdio.h"
#include "clib/string.h"
#include "uni_list.h"
#include "threads.h"
#include "keyboard.h"
#include "buffer.h"
#include "heap.h"

// Lista komend
UNI_LIST_C(command, struct command_entry_t)

// Lista wpisywanych tekstów
UNI_LIST_C(texts, char*)

// Funkcje statyczne
static void report_fail(void);
static uint32_t tokenize_command(char *command);
void shell_command_help(const char* argv, uint32_t argc);

static void add_command_to_history(char *command);
static void shell_prev_command(void);
static void shell_next_command(void);

// Lista wszystkich dostępnych komend
struct list_command_t *command_list = NULL;

// Lista wpisywanych w konsole tekstów
struct list_texts_t *texts_list = NULL;
int32_t texts_current = -1;

// Inicjuje powłokę
void shell_initialize(void)
{
    command_list = list_command_create();
    texts_list = list_texts_create();

    register_command("help", "Display commands list", shell_command_help);
}

// Konsola służąca do wprowadzania i wykonywania poleceń
void shell_main(void)
{
    // Przypisanie akcji do klawiszy strzałek
    keyboard_register_listener(VK_ARROW_UP, 1, shell_prev_command);
    keyboard_register_listener(VK_ARROW_DOWN, 1, shell_next_command);

    char command[SHELL_MAX_COMMAND_LENGTH+1];

    // Drukuje nagłówek
    terminal_fake_clear();
    terminal_setcolor(SHELL_HEADER_COLOR);
    printf("LUKAoS by Lukasz Klimkiewicz, Pre-Alpha Version, Build Date %s\n\n", __DATE__);

    while(1)
    {
        // Prosi o wydanie polecenia
        terminal_setcolor(SHELL_ENCOURAGEMENT_COLOR);
        printf("Enter Command >> ");
        terminal_setcolor(SHELL_COMMAND_COLOR);
        gets(command, SHELL_MAX_COMMAND_LENGTH+1);

        // Dodaje komende do listy wpisywanych w celu późniejszego przewrócenia
        add_command_to_history(command);

        // Wykonuje polecenie
        terminal_setcolor(SHELL_OUTPUT_COLOR);
        printf("\n");
        int success = execute(command);
        if(!success) report_fail();
        printf("\n");
    }
}

// Wykonuje daną komendę
int execute(char *command)
{
    uint32_t argc = tokenize_command(command);

    struct node_command_t *current = command_list->head;

    // Szukanie odpowiedniej komendy
    while(current!=NULL)
    {
        struct command_entry_t command_entry = current->data;
        if(strcmp(command_entry.command, command) == 0)
        {
            // Wywoływanie komendy
            command_fun fun = command_entry.fun;
            fun(command, argc);

            return 1;
        }
        current = current->next;
    }

    return 0;
}

// Dodaje nową komende
void register_command(char *command, char *description, command_fun fun)
{
    struct command_entry_t entry;
    entry.command = command;
    entry.description = description;
    entry.fun = fun;
    list_command_push_back(command_list, entry);
}

// Funkcja powiadami o nieprawidłowej komendzie
static void report_fail(void)
{
    terminal_setcolor(SHELL_FAIL_COLOR);
    printf("Failed To Recognize Command\n");
}

// Zamiena jedną komendę na kilka tokenów
static uint32_t tokenize_command(char *command)
{
    // Czy jesteśmy w środku cydzysłowiu
    int quote_flag = 0;

    // Liczba tokenów
    uint32_t argc = 1;

    char *current = command;

    while(*current != 0)
    {
        if(*current == ' ' && !quote_flag) 
        {
            *current = 0;
            argc++;
        }
        if(*current == '\"') 
        {
            *current = 0;
            quote_flag = !quote_flag;
        }

        current++;
    }

    return argc;
}

// Komenda wyświetlająca wszystkie komendy
void shell_command_help(const char* argv, uint32_t argc)
{
    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("| Command           | Description\n");
    terminal_setcolor(VGA_COLOR_WHITE);

    struct node_command_t *current = command_list->head;

    while(current!=NULL)
    {
        struct command_entry_t command_entry = current->data;

        uint32_t len = printf("| %s", command_entry.command);
        for(; len<20; len++) printf(" ");

        printf("| %s\n", command_entry.description);

        current = current->next;
    }
}

// Dodaje komende do listy wpisywanych w celu późniejszego przewrócenia
static void add_command_to_history(char *command)
{
    char *text = strdup(command);
    list_texts_push_front(texts_list, text);
    texts_current = -1;
}

// Przewraca wcześniej wpisaną komendę
static void shell_prev_command(void)
{
    struct buffer_t *kb_buffer = keyboard_get_buffer();

    // Symuluje wciskanie przez użytkownika backspace
    for(uint32_t i=0; i<SHELL_MAX_COMMAND_LENGTH; i++)
        buffer_put(kb_buffer, '\b');

    if(texts_current + 1 < (int32_t)(texts_list->size ))
        texts_current++;

    char *text = list_texts_get_node_at(texts_list, texts_current)->data;
    simulate_typing(text);
}

// Przewraca później wpisaną komendę
static void shell_next_command(void)
{
    struct buffer_t *kb_buffer = keyboard_get_buffer();

    // Symuluje wciskanie przez użytkownika backspace
    for(uint32_t i=0; i<SHELL_MAX_COMMAND_LENGTH; i++)
        buffer_put(kb_buffer, '\b');

    if(texts_current > -1)
        texts_current--;

    char *text = NULL;

    if(texts_current == -1) text = "";
    else text = list_texts_get_node_at(texts_list, texts_current)->data;

    simulate_typing(text);
}

// Funkcja wywoływana przed rozpoczęciem wstawki
void shell_start_insert(void)
{
    gets_reset();
    printf("\n\n");
}

// Funkcje wywoływana po zakończeniu wstawki
void shell_end_insert(void)
{
    printf("\n");
    terminal_setcolor(SHELL_ENCOURAGEMENT_COLOR);
    printf("Enter Command >> ");
    terminal_setcolor(SHELL_COMMAND_COLOR);
}