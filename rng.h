#ifndef __RNG_H__
#define __RNG_H__

#include <stddef.h>
#include <stdint.h>

// Prototypy
uint32_t rand(void);
void srand(unsigned int seed);

// Funkcje assemblerowe
extern uint32_t check_true_rng(void);
extern uint32_t generate_true_numer(void);

#endif