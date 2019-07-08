#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <stddef.h>
#include <stdint.h>

// Sprawdcza, czy kompilator nie myśli, że system to linux 
#if defined(__linux__)
#error "System should be compilled using cross-compiller"
#endif
 
// Działa tylko na komputerach 32-bit ix86
#if !defined(__i386__)
#error "This system needs to be compiled with a ix86-elf compiler"
#endif

#endif