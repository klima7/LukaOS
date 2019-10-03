#include <stddef.h>
#include <stdint.h>
#include "timers.h"
#include "clib/stdio.h"
#include "heap.h"
#include "uni_list.h"
#include "threads.h"
#include "clock.h"
#include "heap.h"

// Funkcje statyczne
static void timer_thread(void);
static void timer_insert(TIMER *timer, uint32_t time);
static TIMER *timer_create(TIMER_TASK task, uint32_t time, uint32_t period);

// Lista struktur będących timerami
UNI_LIST_C(timers, TIMER*)

// delta queue
struct list_timers_t *delta_queue = NULL;
uint32_t thread_id = 0;
uint64_t last_time = 0;

// Inicjlizacja timerów
void timer_initialize(void)
{
    printf("Timers initialization\n");
    delta_queue = list_timers_create();
    last_time = clock();

    // Tworzy wątek i uruchamia
    thread_id = create_thread((uint32_t)timer_thread);
    start_thread(thread_id);
}

// Wątek kontrolujący timery, działa w tle przez cały czas
static void timer_thread(void)
{
    while(1)
    {
        uint64_t current_time = clock();
        uint32_t ellapsed_time = (uint32_t)(current_time - last_time);
        last_time = current_time;

        if(delta_queue->size>0)
        {
            TIMER *front = list_timers_front(delta_queue);
            front->time -= ellapsed_time;
            if(front->time<=0)
            {
                front->task();
                TIMER *timer = list_timers_pop_front(delta_queue);

                // Timer miał był wykonany tylko raz
                if(timer->period == 0)
                    kfree(timer);

                // Timer ma być wykonywany w kółko
                else
                    timer_insert(timer, timer->period);
                
            }
        }

        yield();
    }
}

// Funkcja pomocnicza, tworzy nowy timer
static TIMER *timer_create(TIMER_TASK task, uint32_t time, uint32_t period)
{
    TIMER *timer = (TIMER*)kmalloc(sizeof(TIMER))
    timer->task = task;
    timer->time = time;
    timer->period = period;
    return timer;
}

// Funkcja pomocnicza, wstawia timer w odpowiednia miejsce w kolejce
static void timer_insert(TIMER *timer, uint32_t time)
{
    // Proste dodanie węzła na początek kolejki gdy jest ona pusta
    if(delta_queue->size == 0)
    {
        timer->time = time;
        list_timers_push_front(delta_queue, timer);
        return;
    }

    // Nieco trudniejszy wariant aktualizacji
    else
    {
        uint32_t current_delay = 0;

        struct node_timers_t *current_node = delta_queue->head;
        for(uint32_t i=0; i<delta_queue->size; i++)
        {
            current_delay += current_node->data->time;

            if(current_delay >= time)
            {
                // Wstawienie na początek kolejki
                if(current_node == delta_queue->head)
                {
                    current_node->data->time -= time;
                    timer->time = time;
                    list_timers_push_front(delta_queue, timer);
                    return;
                }

                // Wstawienie w środek kolejki
                else
                {
                    int32_t temp = current_node->data->time;
                    current_node->data->time = current_delay - time;
                    timer->time = temp-current_node->data->time;
                    list_timers_insert(delta_queue, timer, i);
                    return;
                }
            }

            current_node = current_node->next;
        }

        // Wstawienie na koniec kolejki
        timer->time = time-current_delay;
        list_timers_push_back(delta_queue, timer);
        return;
    }
}

// Funkcja dodaje nowy timer
TIMER *timer_add(TIMER_TASK task, uint32_t time, uint32_t period)
{   
    TIMER *timer = timer_create(task, 0, period);
    timer_insert(timer, time);
    return timer;
}

// Funkcja usówa timer
void timer_remove(TIMER *timer)
{
    struct node_timers_t *current_node = delta_queue->head;
    for(uint32_t i=0; i<delta_queue->size; i++)
    {
        if(current_node->data == timer)
        {
            if(current_node->next!=NULL)
                current_node->next->data->time += current_node->data->time;

            list_timers_remove_node(delta_queue, current_node);
            return;
        }
        current_node = current_node->next;
    }
}