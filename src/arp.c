#include <stdio.h>
#include <stdint.h>

#include "utils.h"
#include "ether.h"
#include "arp.h"

/**
 * @brief ARPパケットの受信処理
 * 
 * @param soc 
 * @param ether 
 * @param data 
 * @param len 
 * @return int 
 */
int ArpRecv(int soc, struct ether_header *ether, uint8_t *data, int len)
{
    struct ether_arp *arp;
    uint8_t	*ptr = data;

    arp = (struct ether_arp *)ptr;
	ptr += sizeof(struct ether_arp);
	len -= sizeof(struct ether_arp);

    switch (arp->hdr.hrd_type){
        case ARPHRD_ETHER:
        default:
            if(arp->hdr.pro_type != my_htons(ETHERTYPE_IP)){
                printf("Not supported\n");
                return -1;
            }
		    break;
    }

    if(my_ntohs(arp->hdr.op) == ARPOP_REQUEST){
        printf("arp request\n");
        // uint32_t addr = my_ntohl(arp->tpa);
        // printf("ip = %s\n", my_inet_ntoa(arp->tpa, buf)); 
    } else if(my_ntohs(arp->hdr.op) == ARPOP_REPLY){
        printf("arp reply\n");
    }
    
    return 0;
}