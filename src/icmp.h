#ifndef ICMP_H
#define ICMP_H

#define ECHO_HDR_SIZE    8

#define	ICMP_ECHOREPLY    0
#define	ICMP_ECHO	      8

struct icmp
{
    struct icmp_header
    {
        uint8_t  type;
        uint8_t  code;        
        uint16_t cksum;    
    } header;
    union 
    {
        struct icmp_echo
        {
            uint16_t id;
            uint16_t seq;
        } echo;
    } hun;
#define	id      hun.echo.id
#define	seq	    hun.echo.seq
} __attribute__ ((__packed__));

int IcmpRecv(int soc, uint8_t *data, int len);

#endif