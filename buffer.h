#ifndef __BUFFER_H__
#define __BUFFER_H__

// Rozmiar buforu
#define BUFFER_SIZE 1024

// Ile znaków można bezpiecznie zwrócić 
#define UNGET_COUNT 2

// Struktura buforu
struct buffer_t
{
    char buffer[BUFFER_SIZE];
    int write_ptr;
    int read_ptr;
};

// Prototypy
void buffer_initialize(struct buffer_t *buff);
int buffer_size(struct buffer_t *buff);
int buffer_isempty(struct buffer_t *buff);
int buffer_isfull(struct buffer_t *buff);
void buffer_put(struct buffer_t *buff, char c);
char buffer_get(struct buffer_t *buff);
void buffer_unget(struct buffer_t *buff, char c);

#endif