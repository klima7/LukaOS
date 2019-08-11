#ifndef __PANIC_TEST_H__
#define __PANIC_TEST_H__

#include <stddef.h>
#include <stdint.h>

int panic_test_main(const char* argv, uint32_t argc);
void panic_test_fun(void);

#endif