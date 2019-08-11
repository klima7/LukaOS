#ifndef __TALK_H__
#define __TALK_H__

#include <stddef.h>
#include <stdint.h>
#include "../terminal.h"
#include "../time.h"

// Definicje
#define TALK_PORT 5
#define TALK_MAX_NICK_LEN 20
#define TALK_MAX_MESSAGE_LEN 200
#define TALK_DEFAULT_NICK "SOMEONE"

// Struktura wiadomości
struct talk_message_t
{
    struct time_t send_time;
    char nick[TALK_MAX_NICK_LEN];
    char message[];
};

// Prototypy
void talk_initialize(void);
int talk_send_message(uint32_t ip, char *message);

#endif