#include <stdio.h>
#include <stdint.h>

#include "utils.h"
#include "param.h"
#include "icmp.h"

extern struct PARAM Param;

int IcmpRecv(int soc, uint8_t *data, int len)
{
    struct icmp *icmp;
    uint8_t *ptr = data;
    int icmplen;

    icmplen = len;

    icmp = (struct icmp *)ptr;
    ptr += ECHO_HDR_SIZE;
    len -= ECHO_HDR_SIZE;

    // チェックサム

    switch(icmp->header.type){
        case ICMP_ECHO:
            printf("  --- Echo Req\n");
            // SendReply()
            break;
        default:
            break;
    }

    return 0;
}