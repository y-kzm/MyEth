#ifndef ICMP_H
#define ICMP_H

#define ECHO_HDR_SIZE    8

#define	ICMP_ECHOREPLY    0
#define	ICMP_ECHO	      8

struct icmp
{
    /* ICMPヘッダ */
    struct icmp_header
    {
        uint8_t  type;    // タイプ
        uint8_t  code;    // コード
        uint16_t cksum;   // チェックサム（ICMPメッセージ全体）
    } header;
#define icmp_type   header.type
#define icmp_code   header.code
#define icmp_cksum  header.cksum
    /* ICMP Header UNion */
    union 
    {
        uint32_t unused;    // 未使用
        struct icmp_echo    // ICMP Echo
        {
            uint16_t id;
            uint16_t seq;
        } echo;
    } hun;
#define	icmp_unused	    hun.unused
#define	icmp_id         hun.echo.id
#define	icmp_seq	    hun.echo.seq
    /* ICMP Data UNion */
    union
    {
        uint8_t data[1];    // 多くのメッセージで利用（可変長）
    } dun;
#define	icmp_data	 dun.data
} __attribute__ ((__packed__));

int IcmpRecv(int soc, struct ip_header *ip, uint8_t *data, int len);

#endif