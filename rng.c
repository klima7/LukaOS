#include <stddef.h>
#include <stdint.h>
#include "rng.h"

// Początkowe ziarno
static uint32_t next = 1;
 
// Znajduje liczbe pseudolosową z zakresu 0-32767
uint32_t rand(void) 
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

// Ustala nowe ziarno
void srand(unsigned int seed)
{
    next = seed;
}