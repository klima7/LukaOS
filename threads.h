#ifndef __THREADS_H__
#define __THREADS_H__

#include <stddef.h>
#include <stdint.h>
#include "uni_list.h"

// Rozmiar stosu wątku - 4kb
#define THREAD_STACK_SIZE 0x4000
#define THREAD_MAX_COUNT 256

// Kwant czasu, co 20 ms następuje zmiana wątku
#define THREAD_QUANTUM 20

// Stany wątków
enum thread_state_t
{
    THREAD_WAITING_FOR_START,
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_RIP
};

// Struktura wątku
struct thread_t
{
    uint32_t esp;
    uint32_t stack;
    uint32_t task;
    uint32_t thread_id;
    uint32_t priority;
    uint32_t process_pid;
    volatile enum thread_state_t state;
};

typedef struct thread_t THREAD;
typedef enum thread_state_t THREAD_STATE;

// Lista wątków
UNI_LIST_H(threads, THREAD*)
typedef struct list_threads_t THREADS_LIST;
typedef struct node_threads_t THREADS_NODE;

// Prototypy
void threads_initialize(void);
uint32_t create_kernel_thread(void);
uint32_t create_thread(uint32_t task_addr);
void destroy_thread(uint32_t id);
void start_thread(uint32_t id);
void terminate_thread(uint32_t id);
void join_thread(uint32_t id);
void debug_display_stack(uint32_t id);
THREAD *get_thread(uint32_t id);
THREAD *get_current_thread(void);
void yield(void);

// Funkcje zewnętrzne
extern void switch_stacks_and_jump(uint32_t current_thread, uint32_t next_thread);
extern void switch_stacks(uint32_t current_thread, uint32_t next_thread);

#endif

typedef struct thread_t THREAD;
typedef enum thread_state_t THREAD_STATE;