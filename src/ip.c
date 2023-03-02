#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "ether.h"
#include "ip.h"
#include "icmp.h"

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