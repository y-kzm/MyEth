#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "param.h"
#include "utils.h"
#include "ether.h"
#include "arp.h"
#include "ip.h"

extern struct PARAM Param;
extern uint8_t AllZeroMac[ETH_ALEN];
extern uint8_t	BcastMac[ETH_ALEN];

// Hash Table（Linear Probing）
struct ARP_TABLE *ArpHashTable[ARP_TABLE_SIZE];

/**
 * @brief ARPテーブルの開放
 * 
 */
void FreeArpTable()
{
    int i;
    for(i=0; i<ARP_TABLE_SIZE; i++){
        free(ArpHashTable[i]);
    }
}

/**
 * @brief Jenkinsハッシュ関数
 * 
 * @param key 
 * @return int 
 */
static int ArpHash(uint8_t *key)
{
    int i;
    uint32_t hash = 0;

    hash = 0;
    for(i=0; i<IP_ALEN; i++){
        hash += key[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash % ARP_TABLE_SIZE;
}

/**
 * @brief エントリの追加
 * 
 * @param mac_addr 
 * @param ip_addr 
 * @param device 
 * @return int 
 */
int ArpAddTableEntry(uint8_t *mac_addr, uint32_t ip_addr, char *device)
{
    struct ARP_TABLE *item = (struct ARP_TABLE *)malloc(sizeof(struct ARP_TABLE));
    int hashindex, tempindex;

    uint8_t temp[4];
    temp[0] = (ip_addr & 0x000000ff);
	temp[1] = (ip_addr & 0x0000ff00) >> 8;
	temp[2] = (ip_addr & 0x00ff0000) >> 16;
	temp[3] = (ip_addr & 0xff000000) >> 24;

    // ip_addrのハッシュをindexとする
    hashindex = ArpHash(temp);
    tempindex = hashindex;

    // エントリの生成
    memcpy(item->mac_addr, mac_addr, ETH_ALEN);
    item->ip_addr = ip_addr;
    item->device = device;
    // timestamp

    while(ArpHashTable[hashindex] != NULL){
        // 登録済みは無視
        if(ArpHashTable[hashindex]->ip_addr == ip_addr){
            return 0;
        }
        ++hashindex;
        if(hashindex > ARP_TABLE_SIZE || hashindex < 0){
            printf("Error\n");
            free(item);
            return -1;
        }
        if(hashindex == tempindex){
            printf("Full of Arp Table\n");
            free(item);
            return -1;
        }

        // 最大サイズを超えないための処理
        hashindex %= ARP_TABLE_SIZE;
    }
    ArpHashTable[hashindex] = item;

    return 0;
}

/**
 * @brief ARPテーブルの表示
 * 
 */
void ShowArpTable()
{
    int i = 0;
    char buf[20];

    puts("+-----------------+-------------------+-----------------+");
    puts("|    IP Address   |    MAC Address    |      Device     |");
    puts("+-----------------+-------------------+-----------------+");
    for(i=0; i<ARP_TABLE_SIZE; i++){
        if (ArpHashTable[i] != NULL){
            printf("| %15s ", my_inet_ntoa(ArpHashTable[i]->ip_addr, buf));
            printf("| %15s ", my_ether_ntoa(ArpHashTable[i]->mac_addr, buf));
            printf("| %15s |\n", ArpHashTable[i]->device);
        }
    }
    puts("+-----------------+-------------------+-----------------+");
}

/**
 * @brief ARPパケットの送信
 * 
 * @param soc 
 * @param op 
 * @param eth_shost 
 * @param eth_dhost 
 * @param sha 
 * @param tha 
 * @param spa 
 * @param tpa 
 * @return int 
 */
int ArpSend(int soc, uint16_t op, 
            uint8_t eth_shost[ETH_ALEN], uint8_t eth_dhost[ETH_ALEN], 
            uint8_t sha[ETH_ALEN], uint8_t tha[ETH_ALEN], 
            uint32_t spa, uint32_t tpa)
{
    struct ether_arp arp;

    memset(&arp, 0, sizeof(struct ether_arp));
    arp.hdr.hrd_type = my_htons(ARPHRD_ETHER);
    arp.hdr.pro_type = my_htons(ETHERTYPE_IP);
    arp.hdr.hrd_len = ETH_ALEN;
    arp.hdr.pro_len = IP_ALEN;
    arp.hdr.op = my_htons(op);

    memcpy(arp.sha, sha, ETH_ALEN);
    memcpy(arp.tha, tha, ETH_ALEN);
    arp.spa = spa;
    arp.tpa = tpa;

    EtherSend(soc, eth_shost, eth_dhost, ETHERTYPE_ARP, (uint8_t *)&arp, sizeof(struct ether_arp));

    return 0;
}

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
    uint32_t addr;

    arp = (struct ether_arp *)ptr;
	ptr += sizeof(struct ether_arp);
	len -= sizeof(struct ether_arp);

    // ハードウェアタイプで分岐    
    switch(arp->hdr.hrd_type){
        case ARPHRD_ETHER:
        default:
            // IPのみサポート
            if(arp->hdr.pro_type != my_htons(ETHERTYPE_IP)){
                printf("Not supported\n");
                return -1;
            }
		    break;
    }

    // オペレーションコードで分岐
    if(my_ntohs(arp->hdr.op) == ARPOP_REQUEST){
        addr = arp->tpa;
        if(Param.vip == arp->tpa){
            // printf("arp request\n");
            ArpAddTableEntry(arp->sha, arp->spa, Param.device);
            ArpSend(soc, ARPOP_REPLY, 
                    Param.vmac, ether->shost, 
                    Param.vmac, arp->sha,
                    arp->tpa, arp->spa);
        } else {
            return -1;
        }
    } else if(my_ntohs(arp->hdr.op) == ARPOP_REPLY){
        addr = my_ntohl(arp->tpa);
        if(Param.vip == addr || addr == 0){
            // printf("arp reply\n");
            // AddArpTable()
        } else {
            return -1;
        }
    }
    
    return 0;
}