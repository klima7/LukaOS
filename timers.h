#ifndef __TIMERS_H__
#define __TIMERS_H__

#include <stddef.h>
#include <stdint.h>
#include "heap.h"
#include "uni_list.h"

// Zadanie timera
typedef void (*TIMER_TASK)(void);

// struktura timera
struct timer_t
{
    TIMER_TASK task;
    int32_t time;
    int32_t period;  // Jeżeli zadanie ma być wykonane tylko raz to <=> period = 0
};

typedef struct timer_t TIMER;

// Lista struktur będących timerami
UNI_LIST_H(timers, TIMER*)

// Prototypy
void timer_initialize(void);
TIMER *timer_add(TIMER_TASK task, uint32_t time, uint32_t period);
void timer_remove(TIMER *timer);

#endif