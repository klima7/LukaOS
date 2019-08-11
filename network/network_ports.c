#include <stddef.h>
#include <stdint.h>
#include "network_ports.h"
#include "network.h"
#include "RTL8139.h"
#include "utils.h"
#include "../heap.h"
#include "../terminal.h"
#include "../shell.h"
#include "../sys.h"
#include "../uni_list.h"
#include "../clib/stdio.h"

UNI_LIST_C(ports, struct port_t)

// Funkcje statyczne
static void ports_command_ports(const char* tokens, uint32_t tokens_count);

// Porty sieciowe
struct list_ports_t *ports = NULL;

// Inicjuje porty sieciowe
void ports_initialize(void)
{
    register_command("ports", "Display all active ports", ports_command_ports);
    ports = list_ports_create();
}

// Ustawia słuchacza oczekującego na pojawienie się danych w danym porcie
void ports_create(uint16_t nr, PORT_LISTENER listener)
{
    struct port_t *check = ports_get(nr);
    if(check!=NULL)
    {
        report_error("Cant Create Port - Port Already Exists");
        return;
    }

    // Inicjuje strukture
    struct port_t port = {0};

    port.nr = nr;
    port.buffer_ptr = NULL;
    port.buffer_size = 0;
    port.listener = listener;

    // Dodaje strukture do listy
    list_ports_push_back(ports, port);
}

// Usuwa słuchacza oczekującego na dane w danym porcie
void ports_remove(uint16_t nr)
{
    struct node_ports_t *current = ports->head;

    while(current!=NULL)
    {
        if(current->data.nr == nr)
        {
            list_ports_remove_node(ports, current);
            return;
        }

        current = current->next;
    }

    report_error("Cant Remove Port - Port Number Not Available");
}

// Wywołuje funkcje słuchacza danego portu
void ports_notify_listener(uint16_t nr, struct network_packet_info *packet_info)
{
    struct port_t *port = ports_get(nr);

    if(port==NULL)
    {
        report_error("Cant Notify Listener - Port Number Not Available");
        return;
    }

    port->listener(port->buffer_ptr, packet_info);

}

// Zwraca wskaźnik na strukture portu
struct port_t *ports_get(uint16_t nr)
{
    struct node_ports_t *current = ports->head;

    while(current!=NULL)
    {
        if(current->data.nr == nr)
            return &current->data;

        current = current->next;
    }

    return NULL;
}

// Komenda wyświetlająca porty na których prowadzony jest nasłuch
static void ports_command_ports(const char* tokens, uint32_t tokens_count)
{
    uint32_t ip = network_get_ip();

    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("| Port                        | State\n");
    terminal_setcolor(VGA_COLOR_WHITE);

    struct node_ports_t *current = ports->head;
    while(current!=NULL)
    {
        printf("| ");
        display_ip(ip);
        terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
        uint32_t len = printf(" : %u", (uint32_t)current->data.nr);

        for(; len<16; len++) printf(" ");
        terminal_setcolor(VGA_COLOR_WHITE);
        printf("| Listening\n");

        current = current->next;
    }
}