#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


/**
 * @brief バイトオーダの入れ替え（16ビット）
 * 
 * @param x 
 * @return uint16_t 
 */
static uint16_t bswap16(uint16_t x)
{
    return (
        (x & 0x00ff) << 8 |
		(x & 0xff00) >> 8 
	);
}

/**
 * @brief バイトオーダの入れ替え（32ビット）
 * 
 * @param x 
 * @return uint32_t 
 */
static uint32_t bswap32(uint32_t x)
{
    return (
        (x & 0x000000ff) << 24 |
        (x & 0x0000ff00) << 8 |
        (x & 0x00ff0000) >> 8 |
        (x & 0xff000000) >> 24
    );
}

uint16_t my_htons(uint16_t x)
{
    return bswap16(x);
}

uint16_t my_ntohs(uint16_t x)
{
    return bswap16(x);
}

uint32_t my_htonl(uint32_t x)
{
    return bswap32(x);
}

uint32_t my_ntohl(uint32_t x)
{
    return bswap32(x);
}

/**
 * @brief MACコロン表記をバイナリのネットワークバイトオーダに変換
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
 * @brief バイナリのネットワークバイトオーダをMACコロン表記に変換
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
 * @brief IPv4ドット表記をネットワークバイトオーダのバイナリに変換 
 * 
 * @param str 
 * @param inet
 * @return int 
 */
uint32_t my_inet_aton(const char *str)
{
    char *ptr, *saveptr = NULL;
    int	i;
    char *tmp = strdup(str);
	uint8_t in[4];

	for(i=0, ptr=strtok_r(tmp, ".", &saveptr); i<4; i++, ptr=strtok_r(NULL, ".", &saveptr)){
		if(ptr == NULL){
			free(tmp);
			return -1;
		}
		in[i] = strtol(ptr, NULL, 10);
	}
	free(tmp);

	return (
		(in[3] << 24) | 
		(in[2] << 16) | 
		(in[1] << 8) | 
		(in[0])
	);
}

/**
 * @brief ネットワークバイトオーダのバイナリをIPv4ドット表記に変換 
 * 
 * @param inet 
 * @param buf 
 * @return char* 
 */
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


/**
 * @brief チェックサムの計算（引数データ1つ）
 * TODO: IPヘッダオプションのチェックサム計算では引数データが２つあるとやりやすい
 * 
 * @param data 
 * @param len 
 * @return uint16_t 
 */
uint16_t checksum(uint8_t *data, int len)
{
	uint32_t sum = 0;
	uint16_t *ptr;
	int i;

	ptr = (uint16_t *)data;

	// 16bitごとに和をとる
	for(i=len; i>1; i-=2){
		sum += *ptr++;
	}
	// 8bit残っていた場合にそれを加える
	if(i == 1){
		sum += *(uint8_t *)ptr;
	}

	// 桁溢れを戻す
	while(sum >> 16){
		sum = (sum & 0xffff) + (sum >> 16);
	}

	// NOTをとる
	return(~sum);
}