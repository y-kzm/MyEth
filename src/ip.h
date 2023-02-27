#ifndef IP_H
#define IP_H

#define IP_ALEN    4

#define	IP_RECV_BUF_NO    16

enum{
    IPPROTO_ICMP = 1,	 
    IPPROTO_TCP = 6,	  
    IPPROTO_UDP = 17,	 
    IPPROTO_IPV6 = 41,    
};
#define IPPROTO_ICMP	IPPROTO_ICMP
#define IPPROTO_TCP		IPPROTO_TCP
#define IPPROTO_UDP		IPPROTO_UDP
#define IPPROTO_IPV6	IPPROTO_IPV6

struct IP_RECV_BUF
{
	time_t	timestamp;
	int	id;
	uint8_t	data[64*1024];
	int	len;
};

struct ip_header
{
	uint8_t ihl:4;		// 32bit単位のIPヘッダ長（4bit）
  	uint8_t ver:4;		// バージョン（4bit）
    uint8_t tos;		// サービスタイプ
    uint16_t tot_len;	// IPヘッダを含むパケット全体長
    uint16_t id;		// フラグメント識別番号
    uint16_t frag_off;	// フラグメントオフセット
    uint8_t ttl;		// TTL
    uint8_t protocol;	// 上位層のプロトコル
    uint16_t hcheck;	// IPヘッダのチェックサム
    uint32_t saddr;		// 送信元IPアドレス
    uint32_t daddr;		// 宛先IPアドレス
} __attribute__ ((__packed__));

int IpRecvBufInit();
int IpRecv(int soc, uint8_t *data, int len);

#endif