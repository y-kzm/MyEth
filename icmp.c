#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "param.h"
#include "ip.h"
#include "icmp.h"

extern struct PARAM Param;

/**
 * @brief ICMP Echo Replyの送信
 * 
 * @param soc 
 * @param recv_ip 
 * @param recv_icmp 
 * @param data 
 * @param len 
 * @return int 
 */
int IcmpSendEchoReply(int soc, struct ip_header *recv_ip, struct icmp *recv_icmp, uint8_t *data, int len)
{
    uint8_t *ptr, sbuf[64*1024]; // TODO: バッファ調整
    struct icmp *icmp;

    ptr = sbuf;
    icmp = (struct icmp *)ptr;

    memset(icmp, 0, sizeof(struct icmp));
    icmp->icmp_type = ICMP_ECHOREPLY;
    icmp->icmp_code = 0;
    icmp->icmp_id = recv_icmp->icmp_id;
    icmp->icmp_seq = recv_icmp->icmp_seq;
    icmp->icmp_cksum = 0;

    ptr += ECHO_HDR_SIZE;

    memcpy(ptr, data, len);
    ptr += len;

    // チェックサムの算出
    icmp->icmp_cksum = checksum(sbuf, ptr-sbuf);

    IpSendProxy(soc, recv_ip->daddr, recv_ip->saddr, IPPROTO_ICMP, sbuf, ptr - sbuf);

    return 0;
}

/**
 * @brief ICMPパケットの受信処理
 * 
 * @param soc 
 * @param data 
 * @param len 
 * @return int 
 */
int IcmpRecv(int soc, struct ip_header *ip, uint8_t *data, int len)
{
    struct icmp *icmp;
    uint8_t *ptr = data;
    uint16_t sum;
    int icmplen;

    icmplen = len;

    icmp = (struct icmp *)ptr;
    ptr += ECHO_HDR_SIZE;
    len -= ECHO_HDR_SIZE;

    // チェックサムの計算
    sum = checksum((uint8_t *)icmp, icmplen);
    
    // チェックサムの検証
	if(sum != 0 && sum != 0xffff){
		printf("Bad icmp checksum\n");
		return -1;
	}

    if(Param.vip == ip->daddr){
        // ICMP Typeで分岐
        switch(icmp->header.type){
            case ICMP_ECHO:
                printf("  --- Echo Req\n");
                IcmpSendEchoReply(soc, ip, icmp, ptr, len);
                break;
            case ICMP_ECHOREPLY:
                printf("  --- Echo Reply");
                // Check()
                break;
            default:
                break;
        }
    }

    return 0;
}