#ifndef INCLUDE_H
#define INCLUDE_H

typedef struct	{
	char	*device;
	uint8_t	vmac[6];
	uint32_t vip;
	uint32_t vmask;
	uint32_t gateway;
}PARAM;


/* ----- IP ----- */
#define	IP_RECV_BUF_NO	16
typedef struct	{
	time_t	timestamp;
	int	id;
	uint8_t	data[64*1024];
	int	len;
}IP_RECV_BUF;

#endif