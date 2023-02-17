#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>             // uintX_t
#include <stdlib.h>             // rand
#include <sys/socket.h>         // socket
#include <linux/if_ether.h>     // ETH_P_ALL
#include <sys/ioctl.h>          // ifreq
#include <net/if.h>             // ifreq 
#include <linux/if_packet.h>    // sockaddr_ll
// 仮
#include <netinet/in.h>
#include <arpa/inet.h>

#include "include.h"
#include "utils.h"

int DeviceSoc;

PARAM Param;
IP_RECV_BUF IpRecvBuf[IP_RECV_BUF_NO];

/**
 * @brief IPパケット受信バッファの初期化
 */
int IpRecvBufInit()
{
    int i;

    for(i=0; i<IP_RECV_BUF_NO; i++){
        IpRecvBuf[i].id = -1;
    }

    return 0;
}

/**
 * @brief ソケットの初期化
 * 
 * @param device デバイス名
 * @return int ソケットディスクリプタ
 */
int init_socket(const char *device)
{
    int soc;
    struct ifreq ifr;
    struct sockaddr_ll sa;

    // raw socketを作成
    if((soc = socket(PF_PACKET, SOCK_RAW, hton_s(ETH_P_ALL))) < 0){
        perror("socket");
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, device, IFNAMSIZ-1);
    // interface index を取得
    if(ioctl(soc, SIOCGIFINDEX, &ifr) < 0){
        perror("ioctl");
        close(soc);
        return -1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sll_family = AF_PACKET;
    sa.sll_protocol = hton_s(ETH_P_ALL);
    sa.sll_ifindex = ifr.ifr_ifindex;
    // socketにインターフェースをbind
    if(bind(soc, (struct sockaddr *)&sa, sizeof(sa)) < 0){
		perror("bind");
		close(soc);
		return(-1);
	}

    return soc;
}

/**
 * @brief エントリーポイント
 */
int main(int argc, char *argv[])
{
    char buf[80];

    // 擬似乱数のseedを設定
    srandom(time(NULL));

    IpRecvBufInit();

    // TODO: ファイルから読み込む
    Param.device = "enx9096f34a568d";
    my_ether_aton("02:00:00:00:00:01", Param.vmac);
    my_inet_aton("192.168.200.112", &Param.vip);
    //ip4_aton("255.255.255.0", Param.vmask);

	printf("vmac = %s\n", my_ether_ntoa(Param.vmac, buf));
	printf("vip = %s\n", inet_ntop(AF_INET, &Param.vip, buf, sizeof(buf)));
	//printf("vmask=%s\n",inet_ntop(AF_INET,&Param.vmask,buf1,sizeof(buf1)));

    if((DeviceSoc = init_socket(Param.device)) == -1){
		exit(-1);
	}

    printf("End...\n");
    return 0;
}