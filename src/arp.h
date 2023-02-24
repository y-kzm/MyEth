#ifndef ARP_H
#define ARP_H

#define	ARPOP_REQUEST	1		
#define	ARPOP_REPLY     2

#define ARPHRD_ETHER 	1
#define	ARPHRD_IEEE802	6

struct arphdr
{
    uint16_t hrd_type;  // ハードウェアタイプ
    uint16_t pro_type;  // プロトコルタイプ
    uint8_t hrd_len;    // ハードウェアアドレス長
    uint8_t pro_len;    // プロトコルアドレス長
    uint16_t op;        // オペレーションコード
};

struct ether_arp
{
    struct arphdr hdr;
    uint8_t sha[ETH_ALEN];  // 送信元ハードウェアアドレス
    uint32_t spa;           // 送信元プロトコルアドレス
    uint8_t tha[ETH_ALEN];  // 宛先（ターゲット）ハードウェアアドレス
    uint32_t tpa;           // 宛先（ターゲット）プロトコルアドレス
} __attribute__ ((__packed__));


#define ARP_TABLE_SIZE    5
struct ARP_TABLE
{
    uint8_t mac_addr[ETH_ALEN];
    uint32_t ip_addr;
    char *device;
    //time_t timestamp;
};

int ArpRecv(int soc, struct ether_header *ether, uint8_t *data, int len);

#endif