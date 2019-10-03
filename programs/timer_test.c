#include <stddef.h>
#include <stdint.h>
#include "../timers.h"
#include "../clib/stdio.h"
#include "../clock.h"

TIMER *t1, *t2, *t3, *t4 = NULL;

void timer_test_fun1(void)
{
	printf("Hi from fun1!\n");
}

void timer_test_fun2(void)
{
	printf("Hi from fun2!\n");
}

void timer_test_fun3(void)
{
	printf("Hi from fun3!\n");
}

void timer_test_fun4(void)
{
    printf("Hi from fun4!\n");
	timer_remove(t3);
}

int timer_test_main(const char* argv, uint32_t argc)
{
	t1 = timer_add(timer_test_fun1, 4000, 0);
	t2 = timer_add(timer_test_fun2, 2000, 0);
	t3 = timer_add(timer_test_fun3, 6000, 1000);
	t4 = timer_add(timer_test_fun4, 15000, 0);

    sleep(16000);

    return 0;
}