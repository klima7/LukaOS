#ifndef __SHELL_H__
#define __SHELL_H__

#include <stddef.h>
#include <stdint.h>
#include "terminal.h"
#include "uni_list.h"

#define SHELL_ENCOURAGEMENT_COLOR VGA_COLOR_LIGHT_GREEN
#define SHELL_HEADER_COLOR VGA_COLOR_LIGHT_MAGENTA
#define SHELL_COMMAND_COLOR VGA_COLOR_WHITE
#define SHELL_FAIL_COLOR VGA_COLOR_LIGHT_RED
#define SHELL_OUTPUT_COLOR VGA_COLOR_WHITE


#define SHELL_MAX_COMMAND_LENGTH 200

// Funkcja wywoływana przy wykonywaniu komendy
typedef void (*command_fun)(const char* argv, uint32_t argc);

// Informacje przechowywane na liście dynamicznej
struct command_entry_t
{
    const char *command;
    const char *description;
    command_fun fun;
};

// Lista komend
UNI_LIST_H(command, struct command_entry_t)

// Lista wpisywanych tekstów
UNI_LIST_H(texts, char*)

// Prototypy
void shell_initialize(void);
void shell_main(void);
int execute(char *command);
void register_command(char *command, char *description, command_fun fun);

void shell_start_insert(void);
void shell_end_insert(void);

#endif