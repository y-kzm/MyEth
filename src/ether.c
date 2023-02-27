#include <stdio.h>
#include <stdint.h>
#include <string.h>     // memcmp
#include <time.h>

#include "param.h"
#include "utils.h"
#include "ether.h"
#include "arp.h"
#include "ip.h"

extern struct PARAM Param;

uint8_t	AllZeroMac[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t	BcastMac[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


/**
 * @brief Etherパケットの送信
 * 
 * @param soc 
 * @param shost 
 * @param dhost 
 * @param type 
 * @param data 
 * @param len 
 * @return int 
 */
int EtherSend(int soc, uint8_t shost[ETH_ALEN], uint8_t dhost[ETHER_H], 
              uint16_t type, uint8_t *data, int len)
{
    struct ether_header *ether;
    uint8_t *ptr, sbuf[sizeof(struct ether_header) + ETHERMTU];

    if(len > ETHERMTU){
        printf("Too long payload\n");
        return -1;
    }

    ptr = sbuf;
    ether = (struct ether_header *)ptr;

    memset(ether, 0, sizeof(struct ether_header));
    memcpy(ether->dhost, dhost, ETH_ALEN);    
    memcpy(ether->shost, shost, ETH_ALEN);    
    ether->type = my_htons(type);

    ptr += sizeof(struct ether_header);

    memcpy(ptr, data, len);
    ptr += len;

    // Etherフレームの最小サイズ（60octed）になるようにPaddingを詰める
    int padlen;
    if((ptr - sbuf) < ETH_ZLEN){
        padlen = ETH_ZLEN - (ptr - sbuf);
        memset(ptr, 0, padlen);
        ptr += padlen;
    }

    write(soc, sbuf, ptr - sbuf);

    return 0;
}

/**
 * @brief Etherフレームの解析
 * 
 * @param soc 
 * @param in_ptr 
 * @param in_len 
 * @return int 
 */
int EtherRecv(int soc, uint8_t *in_ptr, int in_len)
{
    struct ether_header	*ether;
    uint8_t	*ptr = in_ptr;
    int	len = in_len;

	ether = (struct ether_header *)ptr;
	ptr += sizeof(struct ether_header);
	len -= sizeof(struct ether_header);

    // 宛先MACアドレスがvmacかブロードキャスト以外の場合は無視
	if(memcmp(ether->dhost, BcastMac, ETH_ALEN) != 0 && 
            memcmp(ether->dhost, Param.vmac, ETH_ALEN) !=0 ){
	    return -1;
	}

    switch(my_ntohs(ether->type)){
        case ETHERTYPE_ARP:
            printf("--- ARP\n");
		    ArpRecv(soc, ether, ptr, len);
            break;
        case ETHERTYPE_IP:
            printf("--- IP\n");
		    IpRecv(soc, ptr, len);
            // MEMO: ICMP Reqを受け取ってReplyを返せるようにする まずはIPを受け取れるように
            break;
        default:
            printf("Not supported: ether_type\n");
            break;

    }

	return 0;
}