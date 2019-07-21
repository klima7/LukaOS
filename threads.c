#include <stddef.h>
#include <stdint.h>
#include "threads.h"
#include "heap.h"
#include "list.h"
#include "idt.h"
#include "pic.h"
#include "clock.h"
#include "sys.h"
#include "clib/stdio.h"

// Wszystkie wątki dostępne według kategorii
LIST *waiting_threads;         
LIST *active_threads;          
LIST *terminated_threads;

// Wszystke wątki dostępne według id
THREAD *threads_ids[THREAD_MAX_COUNT] = {0};

// Liczba kwantów czasu pozostała do zmiany obecnego wątku na inny
uint32_t current_time_quantums = 0;


// Funkcje statyczne
static NODE *find_node_from_id(LIST *l, uint32_t id);
static uint32_t allocate_thread_id(void);
static uint32_t create_kernel_thread(void);
static void scheduler(void);
static void push_thread32(THREAD *t, uint32_t val);
static void push_thread8(THREAD *t, uint8_t val);
static void thread_goodbye(void);
static void next_thread(THREAD_STATE state);

// Inicjalizauję wielokątkowość
void threads_initialize(void)
{
    waiting_threads = list_create();
    active_threads = list_create();
    terminated_threads = list_create();

    // Jądro ma wątek o id 0
    create_kernel_thread();
    interrupt_register(32, scheduler);
}

// Tworzy nowy wątek i zwraca jego identyfikator
uint32_t create_thread(uint32_t task_addr)
{
    // Uzupelnia pola struktury
    THREAD *new_thread = (THREAD*)kmalloc(sizeof(THREAD));
    new_thread->state = THREAD_WAITING_FOR_START;
    new_thread->stack = (uint32_t)kmalloc(THREAD_STACK_SIZE);
    new_thread->esp = new_thread->stack + THREAD_STACK_SIZE;
    new_thread->task = task_addr;
    new_thread->priority = THREAD_QUANTUM;

    // Uzupelnia numer id
    new_thread->thread_id = allocate_thread_id();
    threads_ids[new_thread->thread_id] = new_thread;

    // Gdy wątek się zakończy to wykonywanie przejdzie teraz do funkcji thread_goodbye
    push_thread32(new_thread, (uint32_t)thread_goodbye);

    // Dodaje wątek do listy oczekujacych na uruchomienie
    list_push_back(waiting_threads, new_thread);

    return new_thread->thread_id;
}

// Tworzy wątek jądra, nie potrzebuje ono już stosu
static uint32_t create_kernel_thread(void)
{
    // Uzupełnia pola struktury
    THREAD *new_thread = (THREAD*)kmalloc(sizeof(THREAD));
    new_thread->state = THREAD_RUNNING;
    new_thread->priority = THREAD_QUANTUM;
    new_thread->stack = 0;
    new_thread->esp = 0;
    new_thread->task = 0;

    // Uzupelnia numer id
    new_thread->thread_id = allocate_thread_id();
    threads_ids[new_thread->thread_id] = new_thread;

    // Dodaje wątek do listy
    list_push_back(active_threads, new_thread);

    return new_thread->thread_id;
}

// Zwalnia pamięć zajętą przez wątek o podanym id
void destroy_thread(uint32_t id)
{
    asm("cli");

    THREAD *thread = threads_ids[id];

    // Wątek nie został jeszcze uruchomiony
    if(thread->state == THREAD_WAITING_FOR_START)
    {
        NODE *node = find_node_from_id(waiting_threads, id);
        list_remove_node(waiting_threads, node);
    }

    // Wątek jest uruchomiony
    else if(thread->state == THREAD_READY || thread->state == THREAD_RUNNING)
    {
        terminate_thread(id);
        NODE *node = find_node_from_id(terminated_threads, id);
        list_remove_node(terminated_threads, node);
    }

    // Wątek jest zatrzymany
    else if(thread->state == THREAD_RIP)
    {
        NODE *node = find_node_from_id(terminated_threads, id);
        list_remove_node(terminated_threads, node);
    }

    // Zwalnia pamięć
    kfree((uint32_t*)thread->stack);
    kfree(thread);

    // Zwalnia id
    threads_ids[id] = NULL;

    asm("sti");
}

// Uruchamia wątek
void start_thread(uint32_t id)
{
    asm("cli");

    NODE *node = find_node_from_id(waiting_threads, id);
    THREAD *activated = list_remove_node(waiting_threads, node);
    list_push_back(active_threads, activated);

    asm("sti");
}

// Kończy wątek o podanym id
void terminate_thread(uint32_t id)
{
    asm("cli");

    // Aktualnie wykonywany wątek
    THREAD *current = list_front(active_threads);

    // Jeżeli usówanym wątkiem jest wątek aktualny wykonywany
    if(current->thread_id == id)
        next_thread(THREAD_RIP);

    // Jeśli wykonuje się obecnie inny wątek
    else
    {
        NODE *node = find_node_from_id(active_threads, id);
        THREAD *terminated = list_remove_node(active_threads, node);
        terminated->state = THREAD_RIP;
        list_push_back(terminated_threads, terminated);
    }

    asm("sti");
}

// Funkcja zatrzymują aktualny wątek do zakończenia wątku id
void join_thread(uint32_t id)
{
    while(threads_ids[id]->state != THREAD_RIP) continue;
}

// Zarządza czasem wykonywania wątków
// Schemat Round Robin
static void scheduler(void)
{
    asm("cli");              // Przerwania są znów włączane w pliku asm po zmienie kontekstów
    PIC_end_notify(32);

    if(current_time_quantums != 0)
        current_time_quantums--;

    // Zmienia wątek na kolejny, a obecny wątek zmienia status na ready
    if(active_threads->size > 1 && current_time_quantums == 0)
    {
        next_thread(THREAD_READY);
    }
}

// Przełącza wykonywanie na kolejny wątek w kolejce
// Funkcja zakłada, że przerwania są już wyłączone
static void next_thread(THREAD_STATE state)
{
    // Pobranie aktualnego wątku
    THREAD *current = list_front(active_threads);
    current->state = state;

    // Gdy obecny wątek się kończy to zmienia listę z active na dead
    if(state == THREAD_RIP) 
    {
        THREAD *dead = list_pop_front(active_threads);
        list_push_back(terminated_threads, dead);
    }

    // Umieszczenie nowego wątku na początku listy
    THREAD *next = list_pop_back(active_threads);
    list_push_front(active_threads, next);

    // Aktualizacja kwantów czasu
    current_time_quantums = next->priority;

    // ---------Pierwsze uruchomienie wątku-----------
    if(next->state == THREAD_WAITING_FOR_START)
    {
        next->state = THREAD_RUNNING;
        switch_stacks_and_jump((uint32_t)current, (uint32_t)next);
    }

    // ----------Kolejne uruchomienie wątku-----------
    else if(next->state == THREAD_READY)
    {
        next->state = THREAD_RUNNING;
        switch_stacks((uint32_t)current, (uint32_t)next);
    }
}

// Funkcja wywoływana po zakończeniu każdego wątku
static void thread_goodbye(void)
{
    // Znajdujemy kolejny wątek
    next_thread(THREAD_RIP);
}

// Funkcja pomocnicza znajduje wolny numer id
static uint32_t allocate_thread_id(void)
{
    // Dodatkowa zmienna umozliwi szybsze znajdywanie id
    static uint32_t last_id = 0;

    for(uint32_t i=last_id; i<last_id+THREAD_MAX_COUNT; i++)
    {
        uint32_t index = i % THREAD_MAX_COUNT;
        if(threads_ids[index] == NULL)
        {
            last_id = index;
            return index;
        }
    }
    return 0;
}

// Znajduje na podanej liście wątek o szukanym id
static NODE *find_node_from_id(LIST *l, uint32_t id)
{
    NODE *current = l->head;
    while(current!=NULL)
    {
        if(current->data->thread_id == id) return current;
        current = current->next;
    }

    kernel_panic("Any Free Threads ID");
    return NULL;
}

// Umieszcza wartość 32 bit na stosie danego wątku
static void push_thread32(THREAD *t, uint32_t val)
{
    uint8_t b1 = (val & 0x000000FF) >> 0;
    uint8_t b2 = (val & 0x0000FF00) >> 8;
    uint8_t b3 = (val & 0x00FF0000) >> 16;
    uint8_t b4 = (val & 0xFF000000) >> 24;

    push_thread8(t, b4);
    push_thread8(t, b3);
    push_thread8(t, b2);
    push_thread8(t, b1);
}

// Umieszcza wartość 8 bit na stosie danego wątku
static void push_thread8(THREAD *t, uint8_t val)
{
    t->esp--;
    uint8_t *addr = (uint8_t*)t->esp;
    *addr = val;
}

// Debugowanie
void debug_display_stack(uint32_t id)
{
    THREAD *t = threads_ids[id];

    printf("\n");
    printf("ADDRESS       VALUE\n");
    printf("----------------------\n");
    for(uint32_t start=t->esp+40; start>=t->esp; start-=4)
    {
        uint32_t *ptr = (uint32_t*)start;
        printf("%x  ->  %x", start, *ptr);
        if(start==t->esp) printf("   <- ESP \n");
        else printf("\n");
    }

    // Pauza
    while(1) continue;
}