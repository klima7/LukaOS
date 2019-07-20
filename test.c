#include <stddef.h>
#include <stdint.h>
#include "test.h"
#include "threads.h"
#include "clock.h"
#include "clib/stdio.h"

void fun1(void)
{
    printf("[FUN1]");
    while(1) continue;
}

void fun2(void)
{
    while(1)
    {
        printf("[FUN2]");
        sleep(1000);
    }
}

void fun3(void)
{
    for(int i=0; i<5; i++)
    {
        printf("[FUN3]");
        sleep(1000);
    }
}

void fun4(void)
{
    printf("[FUN4]");
}

void test_main(void)
{
    uint32_t thread_1 = create_thread((uint32_t)fun1);
    uint32_t thread_2 = create_thread((uint32_t)fun2);
    uint32_t thread_3 = create_thread((uint32_t)fun3);
    uint32_t thread_4 = create_thread((uint32_t)fun4);

    start_thread(thread_1);
    start_thread(thread_2);
    start_thread(thread_3);
    start_thread(thread_4);

    join_thread(thread_3);
    terminate_thread(thread_1);
    terminate_thread(thread_2);

    destroy_thread(thread_1);
    destroy_thread(thread_2);
    destroy_thread(thread_3);
    destroy_thread(thread_4);
}