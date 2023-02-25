#ifndef ETHER_H
#define ETHER_H

#define ETH_ALEN    6
#define ETH_ZLEN    60      /* Ether最小フレームサイズ（FCSを除く） */

#define ETHERTYPE_IP    0x0800
#define ETHERTYPE_ARP   0x0806

#define ETHERMTU    1500    /* Ether最大ペイロードサイズ*/

struct ether_header
{
    uint8_t dhost[ETH_ALEN];
    uint8_t shost[ETH_ALEN];
    uint16_t type;
} __attribute__ ((__packed__));

int EtherSend(int soc, uint8_t shost[ETH_ALEN], uint8_t dhost[ETHER_H], 
              uint16_t type, uint8_t *data, int len);
int EtherRecv(int soc, uint8_t *in_ptr, int in_len);

#endif