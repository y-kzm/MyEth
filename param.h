#ifndef PARAM_H
#define PARAM_H

struct PARAM 
{
	char	*device;
	uint8_t	vmac[6];
	uint32_t vip;
	uint32_t vmask;
	uint32_t gateway;
};

#endif