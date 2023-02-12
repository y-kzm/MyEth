#ifndef INCLUDE_H
#define INCLUDE_H

typedef struct	{
	char	*device;
	u_int8_t	vmac[6];
	struct in_addr	vip;
	struct in_addr	vmask;
	struct in_addr	gateway;
}PARAM;


/* ----- IP ----- */
#define	IP_RECV_BUF_NO	16
typedef struct	{
	time_t	timestamp;
	int	id;
	u_int8_t	data[64*1024];
	int	len;
}IP_RECV_BUF;

#endif