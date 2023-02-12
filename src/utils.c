#include <stdio.h>

static uint16_t bswap16(uint16_t x)
{
    return (
        (((x) >> 8) & 0xff) | (((x) & 0xff) << 8)
    );
}

uint16_t hton_s(uint16_t x)
{
    return bswap16(x);
}