#ifndef IP_H
#define IP_H

#define IP_ALEN    4

#define	IP_RECV_BUF_NO    16
struct IP_RECV_BUF
{
	time_t	timestamp;
	int	id;
	uint8_t	data[64*1024];
	int	len;
};

#endif