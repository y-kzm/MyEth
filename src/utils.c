#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>             // uintX_t

/**
 * @brief バイトオーダーの入れ替え（16ビット）
 * 
 * @param x 
 * @return uint16_t 
 */
static uint16_t bswap16(uint16_t x)
{
    return (
        (((x) >> 8) & 0xff) | (((x) & 0xff) << 8)
    );
}

uint16_t hton_s(uint16_t x)
{
    return bswap16(x);
}

/**
 * @brief MACアドレス文字列をネットワークバイトオーダーに変換
 * 
 * @param str 
 * @param mac
 */
int my_ether_aton(const char *str, uint8_t *mac)
{
    char *ptr, *saveptr = NULL;
    int	i;
    char *tmp = strdup(str);

	for(i=0, ptr=strtok_r(tmp, ":", &saveptr); i<6; i++, ptr=strtok_r(NULL, ":", &saveptr)){
		if(ptr == NULL){
			free(tmp);
			return -1;
		}
		mac[i] = strtol(ptr, NULL, 16);
	}
	free(tmp);

	return 0;
}

/**
 * @brief ネットワークバイトオーダーをMACアドレス文字列に変換
 * 
 * @param mac
 * @param buf 
 * @return char* 
 */
char *my_ether_ntoa(const uint8_t *mac, char *buf)
{
	sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

	return buf;
}

/**
 * @brief IPv4アドレス文字列をネットワークバイトオーダーに変換 
 * 
 * @param str 
 * @param inet
 * @return int 
 */
int my_inet_aton(const char *str, uint32_t *inet)
{
    char *ptr, *saveptr = NULL;
    int	i;
    char *tmp = strdup(str);
	uint8_t in[4];

	printf("%s\n", str);

	for(i=0, ptr=strtok_r(tmp, ".", &saveptr); i<4; i++, ptr=strtok_r(NULL, ".", &saveptr)){
		if(ptr == NULL){
			free(tmp);
			return -1;
		}
		in[i] = strtol(ptr, NULL, 10);
		printf("%s\n", ptr);
		printf("%u\n", in[i]);
	}
	// inet = (uint32_t *)((in[3] << 24) | (in[2] << 16) | (in[1] << 8) | (in[0]));

	inet += (in[0] << 0);
	inet += (in[1] << 8);
	inet += (in[2] << 16);
	inet += (in[3] << 24);

	free(tmp);

	return 0;
}

char *my_inet_ntoa(const uint32_t inet, char *buf)
{	
	uint8_t addr[4];
	addr[0] = (inet & 0x000000ff);
	addr[1] = (inet & 0x0000ff00) >> 8;
	addr[2] = (inet & 0x00ff0000) >> 16;
	addr[3] = (inet & 0xff000000) >> 24;

	sprintf(buf, "%d.%d.%d.%d", addr[0],addr[1],addr[2],addr[3]);

	return buf;
}