#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"
#include "param.h"
#include "ether.h"
#include "arp.h"
#include "ip.h"
#include "icmp.h"

extern struct PARAM Param;

struct IP_RECV_BUF IpRecvBuf[IP_RECV_BUF_NO];

/**
 * @brief IPパケット受信バッファの初期化
 * 
 * @return int 
 */
int IpRecvBufInit()
{
    int i;

    for(i=0; i<IP_RECV_BUF_NO; i++){
        IpRecvBuf[i].id = -1;
    }

    return 0;
}

/**
 * @brief IPパケットの送信処理（Flagment非対応）
 * TODO: Flagment対応
 * 
 * @param soc 
 * @param smac 
 * @param dmac 
 * @param saddr 
 * @param daddr 
 * @param ip_proto 
 * @param data 
 * @param len 
 */
int IpSend(int soc, uint8_t smac[ETH_ALEN], uint8_t dmac[ETH_ALEN], uint32_t saddr, uint32_t daddr, uint8_t ip_proto, uint8_t *data, int len)
{
    struct ip_header *ip;
    uint16_t id;
    uint8_t *dptr, *ptr, sbuf[ETHERMTU];
    int lest, sndLen, off;

    if(len > ETHERMTU - sizeof(struct ip_header)){
        printf("IpSend: data toot long\n");
        return -1;
    }

    id = random();

    dptr = data;
    lest = len;

    while(lest > 0){
        if(lest > ETHERMTU - sizeof(struct ip_header)){
            // flagment
		} else {
			sndLen = lest;
		}

        ptr = sbuf;
        ip = (struct ip_header *)ptr;
        memset(ip, 0, sizeof(struct ip_header));

        ip->ihl = 5;  // オプションなし
        ip->ver = 4;
        // tos
        ip->tot_len = my_htons(sizeof(struct ip_header) + sndLen);
        ip->id = my_htons(id);
        off=(dptr - data) / 8;
		ip->frag_off = my_htons(0 | (off & 0x1fffU));
        ip->ttl = 64;
        ip->protocol = ip_proto;
        ip->saddr = saddr;
        ip->daddr = daddr;
        ip->hcheck = 0;
        ip->hcheck = checksum((uint8_t *)ip, sizeof(struct ip_header));

        ptr += sizeof(struct ip_header);
        memcpy(ptr, dptr, sndLen);
        ptr += sndLen;

        EtherSend(soc, smac, dmac, ETHERTYPE_IP, sbuf, ptr - sbuf);

        dptr += sndLen;
        lest -= sndLen;
    }

    return 0;
}


/**
 * @brief IPパケットを送信する前処理
 * 
 * @param soc 
 * @param saddr 
 * @param daddr 
 * @param ip_proto 
 * @param data 
 * @param len 
 * @return int 
 */
int IpSendProxy(int soc, uint32_t saddr, uint32_t daddr, uint8_t ip_proto, uint8_t *data, int len)
{
    uint8_t dmac[ETH_ALEN];

    // IPアドレスからMACアドレスを取得
    if(GetTargetMac(soc, daddr, dmac, 0)){
        printf("  --- Send Echo Reply\n");
        return (IpSend(soc, Param.vmac, dmac, saddr, daddr, ip_proto, data, len));
    } else {
        printf("Destination Host Unreachable\n");
        return -1;
    }

    return 0;
}


/**
 * @brief IPパケットの受信処理
 * 
 * @param soc 
 * @param data 
 * @param len 
 * @return int 
 */
int IpRecv(int soc, uint8_t *data, int len)
{
    struct ip_header *ip;
    uint8_t opt[ETHERMTU];
    uint8_t *ptr = data;
    uint16_t sum;
    int optlen, paylen;

    if(len < sizeof(struct ip_header)){
        printf("len(%d) < sizeof(struct ip_header)\n", len);
        return -1;
    }

    ip = (struct ip_header *)ptr;
    ptr += sizeof(struct ip_header);
    len -= sizeof(struct ip_header);


    // オプション長 = IPヘッダ長 - オプションを除くIPヘッダ長
    optlen = ip->ihl - sizeof(struct ip_header);
    if(optlen > 0){
        // IPデータグラムの最大値を超えていた場合
        if(optlen >= ETHERMTU){
            printf("IP optlen too big\n");
            return -1;
        }
        memcpy(opt, ptr, optlen);
        ptr += optlen;
        len -= optlen;
    }

    // チェックサムの計算
    if(optlen == 0){
        sum = checksum((uint8_t *)ip, sizeof(struct ip_header));
    } else {
        // cheksum2();
        // TODO: IPヘッダオプションがあった場合のチェックサム計算
    }

    // チェックサムの検証
    if(sum != 0 && sum != 0xffff){
		printf("Bad ip checksum\n");
		return -1;
	}

    // ペイロード長 = IPパケット長 - IPヘッダ長（32bit単位なのでx4）
    paylen = my_ntohs(ip->tot_len) - (ip->ihl * 4);

    // フラグメント関連の処理

    // IP Protocolで分岐
    switch(ip->protocol){
        case IPPROTO_ICMP:
            printf("  --- ICMP\n");
            IcmpRecv(soc, ip, ptr, len);
            break;
        default:
            printf("Not suppoted: ip_protocol(%d)\n", ip->protocol);
            break;
    }

    return 0;

}