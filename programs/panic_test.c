#include <stddef.h>
#include <stdint.h>
#include "panic_test.h"
#include "../threads.h"
#include "../clib/stdio.h"
#include "../clock.h"

int panic_test_main(const char* argv, uint32_t argc)
{
    while(1)
    {
        uint32_t id = create_thread((uint32_t)panic_test_fun);
        printf("Thread ID: %u\n", id);
        sleep(10);
    }
}

void panic_test_fun(void)
{
    // Do nothing
    return;
}