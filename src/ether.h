#ifndef ETHER_H
#define ETHER_H

#define ETH_ALEN    6

#define ETHERTYPE_IP    0x0800
#define ETHERTYPE_ARP   0x0806

struct ether_header
{
    uint8_t dhost[ETH_ALEN];
    uint8_t shost[ETH_ALEN];
    uint16_t type;
} __attribute__ ((__packed__));

int EtherRecv(int soc, uint8_t *in_ptr, int in_len);

#endif