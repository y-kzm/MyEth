#include <stdio.h>
#include <stdint.h>
#include <string.h>     // memcmp
#include <time.h>

#include "param.h"
#include "utils.h"
#include "ether.h"
#include "arp.h"

extern struct PARAM Param;

uint8_t	AllZeroMac[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t	BcastMac[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

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
            printf("ARP\n");
		    ArpRecv(soc, ether, ptr, len);
            break;
        case ETHERTYPE_IP:
            printf("IP\n");
		    // IpRecv(soc, in_ptr, in_len, eh, ptr, len); 
            break;
        default:
            printf("Not supported\n");
            break;

    }

	return 0;
}