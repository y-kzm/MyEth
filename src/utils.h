#ifndef UTILS_H
#define UTILS_H

uint16_t my_htons(uint16_t x);
int my_ether_aton(const char *str, uint8_t *mac);
char *my_ether_ntoa(const uint8_t *mac, char *buf);
uint32_t my_inet_aton(const char *str);
char *my_inet_ntoa(const uint32_t inet, char *buf);

#endif