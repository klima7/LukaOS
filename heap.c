#include <stddef.h>
#include <stdint.h>
#include "heap.h"
#include "terminal.h"
#include "sys.h"
#include "memory_map.h"
#include "multiboot.h"
#include "clib/stdio.h"
#include "clib/math.h"

// Prototypy funkcji statycznych
static MEMORY_BLOCK_HEADER *take_new_pages(uint32_t size);
static void return_free_pages(MEMORY_BLOCK_HEADER *block);
static MEMORY_BLOCK_HEADER *memory_join(MEMORY_BLOCK_HEADER *block);
static MEMORY_BLOCK_HEADER *memory_split(MEMORY_BLOCK_HEADER *block, uint32_t size);
static MEMORY_BLOCK_HEADER *create_header_at(uint32_t address, MEMORY_BLOCK_HEADER *prev, MEMORY_BLOCK_HEADER *next, uint32_t size, int state);

// Lista zawierająca wszystkie zaalokowany i puste bloki pamięci
struct mem_list mem_list;

// Inicjalizuje sterte
void heap_initialize(void)
{
    printf("Heap Initialization\n");
    mem_list.head = NULL;
    mem_list.tail = NULL;
    mem_list.size = 0;
}

// Alokuje obszar pamięci na size bajtów i zwraca jego adres
void *__kmalloc(uint32_t size, const char *filename, uint32_t line)
{   
    MEMORY_BLOCK_HEADER *current = mem_list.head;
    MEMORY_BLOCK_HEADER *found = NULL;

    // Szuka wolnego bloku o dobrym rozmiarze
    for(uint32_t i=0; i<mem_list.size; i++)
    {
        if(current->state == FREE && current->size >= size) { found = current; break; }
        current = current->next_block;
    }

    // Trzeba przydzielić nową stronę
    if(found == NULL) 
        found = take_new_pages(size);

    // Dzieli blok na dwa jeśli można
    if((int64_t)found->size - (int64_t)size - (int64_t)HSIZE > 0)
        found = memory_split(found, size);

    found->state = USED;
    found->filename = filename;
    found->line = line;

    return found+1;
}

// Alokuje przekazaną liczbe bajtów i wszystkie zeruje
void *__kcalloc(uint32_t size, const char *filename, uint32_t line)
{
    char *ptr = __kmalloc(size, filename, line);
    if(ptr == NULL) return NULL;

    for(uint32_t i=0; i<size; i++)
        *(ptr + i) = 0;

    return ptr;
}

// Przeprowadza relokacje przekazanego obszaru pamięci
void *__krealloc(void *old_ptr, uint32_t new_size, const char *filename, uint32_t line)
{
    // Przeprowadz zwykłą alokacje
    if(old_ptr == NULL) 
        return __kmalloc(new_size, filename, line);

    MEMORY_BLOCK_HEADER *old_block = (MEMORY_BLOCK_HEADER*)old_ptr;
    old_block--;

    // Nie rób nic, aktualny rozmiar jest wystarczający
    if(old_block->size >= new_size)
        return old_ptr;

    // Przeprowadz relokacje
    char *new_ptr = __kmalloc(new_size, filename, line);
    if(new_ptr == NULL) return NULL;

    // Kopiowanie
    char *old_ptr_char = (char*)old_ptr;
    for(uint32_t i=0; i<old_block->size; i++)
        *(new_ptr+i) = *(old_ptr_char + i);
    
    kfree(old_ptr);
    return new_ptr;
}

// Zwalnia zaalokowany wcześniej blok
void kfree(void *ptr)
{
    MEMORY_BLOCK_HEADER *block = (MEMORY_BLOCK_HEADER*)ptr;
    block--;

    if(block->state != USED)
    {
        report_error("Memory Already Free\n");
        return;
    }

    block->state = FREE;

    // Łączy powstały pusty blok z sąsiednimi
    MEMORY_BLOCK_HEADER *connect1 = NULL;
    if(block != mem_list.tail && block->next_block->state == FREE) connect1 = memory_join(block);
    if(connect1!=NULL) block = connect1;

    // Łączy powstały pusty blok z sąsiednimi
    MEMORY_BLOCK_HEADER *connect2 = NULL;
    if(block != mem_list.head && block->prev_block->state == FREE) connect2 = memory_join(block->prev_block);
    if(connect2!=NULL) block = connect2;

    // Zwraca stronę jeśli stała się pusta
    return_free_pages(block);
}

// Debugowanie, wyświetla liste bloków pamięci
void debug_display_heap(void)
{
    MEMORY_BLOCK_HEADER *current = mem_list.head;

    uint8_t last_color = terminal_getcolor();
    terminal_setcolor(VGA_COLOR_WHITE);
    printf("| BEGIN          | LEN            | TYPE           | FILE           | LINE\n");
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    for(uint32_t i=0; i<mem_list.size; i++)
    {
        int count = printf("| %u", (unsigned int)current);
        for(; count<17; count++) printf(" ");
        count = printf("| %u", current->size);
        for(; count<17; count++) printf(" ");
        if(current->state == FREE) count = printf("| Free");
        if(current->state == USED) count = printf("| Used");
        for(; count<17; count++) printf(" ");
        if(current->state==USED) count = printf("| %s", current->filename);
        else count = printf("| ");
        for(; count<17; count++) printf(" ");
        if(current->state==USED) count = printf("| %u\n", current->line);
        else count = printf("| \n");
        current = current->next_block;
    }
    terminal_setcolor(last_color);
}

// Tworzy nagłówek bloku pamięci w danym miejscu o podanych parametrach
static MEMORY_BLOCK_HEADER *create_header_at(uint32_t address, MEMORY_BLOCK_HEADER *prev, MEMORY_BLOCK_HEADER *next, uint32_t size, int state)
{
    MEMORY_BLOCK_HEADER *header = (MEMORY_BLOCK_HEADER*)address;
    header->prev_block = prev;
    header->next_block = next;
    header->size = size;
    header->state = state;
    return header;
}

// Dzieli blok pamięci na dwa, tak aby pierwszy z nich miał size bajtów i zwraca adres pierwszego z nich
static MEMORY_BLOCK_HEADER *memory_split(MEMORY_BLOCK_HEADER *block, uint32_t size)
{
    if(block->size <= HSIZE || block->state != FREE)
        return NULL;

    uint32_t new_header_address = (uint32_t)block + sizeof(MEMORY_BLOCK_HEADER) + size;
    MEMORY_BLOCK_HEADER *new_block = create_header_at(new_header_address, block, block->next_block, block->size - size - HSIZE, FREE);

    if(block == mem_list.tail) mem_list.tail = new_block;
    else block->next_block->prev_block = new_block;

    block->next_block = new_block;
    block->size = size;

    mem_list.size++;

    return block;
}

// Łączy podany blok z blokiem następnym i zwraca adres połączonego bloku
static MEMORY_BLOCK_HEADER *memory_join(MEMORY_BLOCK_HEADER *block)
{
    if(block == mem_list.tail || block->state != FREE || block->next_block->state != FREE) return NULL;

    // Sąsiednie bloki z listy mogą pochodzić z różnich obszarów pamięci więc trzeba sprawdzić czy na prawde sąsiadują
    if((uint32_t)block + block->size + HSIZE != (uint32_t)block->next_block) return NULL;

    if(block->next_block == mem_list.tail) mem_list.tail = block;
    mem_list.size--;

    block->size = block->size + block->next_block->size + HSIZE;
    block->next_block = block->next_block->next_block;
    block->next_block->prev_block = block;

    return block;
}

// Przydziela stercie nowe strony na minimum size bajtów, tworzy na nich wolny blok, dołącza na koniec listy i zwraca
static MEMORY_BLOCK_HEADER *take_new_pages(uint32_t size)
{
        uint32_t pages_count = (size + HSIZE) / PAGE_SIZE;
        pages_count += ((size + HSIZE) % PAGE_SIZE) > 0;

        uint32_t page_addr = page_claim(pages_count);

        MEMORY_BLOCK_HEADER *new_header = create_header_at(page_addr, mem_list.tail, NULL, pages_count*PAGE_SIZE-HSIZE, FREE);

        if(mem_list.size == 0) mem_list.head = new_header;
        else mem_list.tail->next_block = new_header;

        mem_list.size++;
        mem_list.tail = new_header;

        return new_header;
}

// Jeśli przekazany blok pamięci jest pusty i zajmuję całą stronę to dana strona jest zwalniana
static void return_free_pages(MEMORY_BLOCK_HEADER *block)
{
    uint32_t page_start = (uint32_t)block;
    if((uint32_t)block % PAGE_SIZE != 0) page_start = (uint32_t)block + PAGE_SIZE - (uint32_t)block % PAGE_SIZE;
    uint32_t block_end = (uint32_t)block + block->size + HSIZE;
    uint32_t len = block_end - page_start;

    if(len >= PAGE_SIZE && block_end > page_start) // len jest unsigned! więc trzeba się upewnić
    {
        if((uint32_t)block < page_start) block = memory_split(block, page_start - (uint32_t)block - HSIZE)->next_block;
        if(block == mem_list.head) mem_list.head = block->next_block;
        else block->prev_block->next_block = block->next_block;
        if(block == mem_list.tail) mem_list.tail = block->prev_block;
        else block->next_block->prev_block = block->prev_block;
        mem_list.size--;
        page_set_range(page_start, len, PAGE_FREE);
    }
}