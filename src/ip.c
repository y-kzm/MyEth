#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "ether.h"
#include "ip.h"

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

int IpRecv(int soc, uint8_t *data, int len)
{
    struct ip_header *ip;
    uint8_t opt[ETHERMTU];
    uint8_t *ptr = data;
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
        // cheksum();
    } else {
        // cheksum2();
    }

    // チェックサムの検証

    // ペイロード長 = IPパケット長 - IPヘッダ長（32bit単位なのでx4）
    paylen = my_ntohs(ip->tot_len) - (ip->ihl * 4);

    switch(ip->protocol){
        case IPPROTO_ICMP:
            printf("  --- ICMP\n");
            IcmpRecv(soc, ptr, len);
            break;
        default:
            printf("Not suppoted: ip_protocol(%d)\n", ip->protocol);
            break;
    }

    return 0;

}