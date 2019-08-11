#ifndef __UTILS_H__
#define __UTILS_H__

#include <stddef.h>
#include <stdint.h>

// Prototypy
uint64_t nths6(uint64_t val);
uint64_t hston6(uint64_t val);
uint32_t nths4(uint32_t val);
uint32_t hston4(uint32_t val);
uint16_t nths2(uint16_t val);
uint16_t hston2(uint16_t val);
uint8_t nths1(uint8_t val);
uint8_t hston1(uint8_t val);

void display_ip(uint32_t ip);
void display_mac(uint64_t mac);

uint32_t str_to_ip(char *str, int *ok);
uint64_t str_to_mac(char *str, int *ok);

uint16_t calculate_checksum(void *ptr, uint32_t len);

#endif