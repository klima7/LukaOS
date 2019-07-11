#include <stddef.h>
#include <stdint.h>
#include "buffer.h"

// Inicjuje bufor
void buffer_initialize(struct buffer_t *buff)
{
    buff->read_ptr = 0;
    buff->write_ptr = 0;
}

// Zwraca liczbe elementów w buforze
int buffer_size(struct buffer_t *buff)
{
    // Bez zawinięcia - bufor jest cykliczny
    if(buff->read_ptr < buff->write_ptr)
    {
        int size =  buff->write_ptr - buff->read_ptr;
        return size;
    }

    // Zawinięcie - bufor jest cykliczny
    else if(buff->read_ptr > buff->write_ptr)
    {
        int size = buff->write_ptr + BUFFER_SIZE - buff->read_ptr;
        return size;
    }

    // Bufor pusty
    return 0;
}

// Sprawdza czy bufor jest pusty
int buffer_isempty(struct buffer_t *buff)
{
    if(buffer_size(buff)==0)
        return 1;
    else
        return 0;
}

// Sprawdza czy bufor jest pełny
int buffer_isfull(struct buffer_t *buff)
{
    int size = buffer_size(buff);
    int free = BUFFER_SIZE - size;
    if(free>UNGET_COUNT)
        return 0;
    else
        return 1;
}

// Zapisuje znak w buforze
void buffer_put(struct buffer_t *buff, char c)
{
    if(buffer_isfull(buff))
        return;

    buff->buffer[buff->write_ptr] = c;
    (buff->write_ptr)++;
    if((buff->write_ptr)>=BUFFER_SIZE)
        buff->write_ptr = 0;
}

// Odczytuje znak z buforu
char buffer_get(struct buffer_t *buff)
{
    if(buffer_isempty(buff))
        return 0;

    char c = buff->buffer[buff->read_ptr];
    (buff->read_ptr)++;
    if((buff->read_ptr)>=BUFFER_SIZE)
        buff->write_ptr = 0;

    return c;
}

// Zwraca wcześniej odczytany znak do buforu
void buffer_unget(struct buffer_t *buff, char c)
{
    if(buff->read_ptr>0)
        (buff->read_ptr)--;
    else
        buff->read_ptr = BUFFER_SIZE - 1;
    
    buff->buffer[buff->read_ptr] = c;
}