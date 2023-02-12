#include <stdio.h>
#include <time.h>
#include <stdint.h>  // uint16_t
#include <stdlib.h>  // rand
#include <sys/socket.h>  // socket
#include <linux/if_ether.h>  // ETH_P_ALL

#include "include.h"
#include "utils.h"

int DeviceSoc;

PARAM Param;
IP_RECV_BUF IpRecvBuf[IP_RECV_BUF_NO];

int IpRecvBufInit()
{
    int i;

    for(i=0; i<IP_RECV_BUF_NO; i++){
        IpRecvBuf[i].id = -1;
    }

    return 0;
}

int init_socket(char *device)
{
    int soc;

    soc = socket(AF_PACKET, SOCK_RAW, hton_s(ETH_P_ALL));

    return soc;
}

int main(int argc, char *argv[])
{
    // 擬似乱数のseedを設定
    srandom(time(NULL));

    IpRecvBufInit();


    return 0;
}