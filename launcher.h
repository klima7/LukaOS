#ifndef __LAUNCHER_H__
#define __LAUNCHER_H__

#include <stddef.h>
#include <stdint.h>

// wzór głównej funkcji każdego programu
typedef int (*program_main_t)(const char* argv, uint32_t argc);

// Struktura opisująca każdy program
struct program_entry_t
{
    const char *program_name;
    program_main_t program_main;
};

// Prototypy
void launcher_initialize(void);
void launcher_start(const char *name, const char *argv, uint32_t argc);

#endif