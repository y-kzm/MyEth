#include <stdio.h>
#include <time.h>
#include <string.h>
#include <signal.h>             // signal()
#include <poll.h>               // poll()
#include <stdint.h>             // uintX_t
#include <stdlib.h>             // rand
#include <sys/socket.h>         // socket
#include <linux/if_ether.h>     // ETH_P_ALL
#include <sys/ioctl.h>          // ifreq
#include <net/if.h>             // ifreq 
#include <linux/if_packet.h>    // sockaddr_ll

#include "param.h"
#include "utils.h"
#include "ether.h"
#include "arp.h"
#include "ip.h"

int DeviceSoc;

int	EndFlag=0;

struct PARAM Param;
struct IP_RECV_BUF IpRecvBuf[IP_RECV_BUF_NO];

extern struct ARP_TABLE *ArpHashTable[ARP_TABLE_SIZE];

/**
 * @brief IPパケット受信バッファの初期化
 * 
 * @return int 
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
 * @brief Etherフレームの受信処理
 * 
 * @param arg 
 * @return void* 
 */
void *EthThread(void *arg)
{
    int	nready;
    struct pollfd fds[1];
    uint8_t buf[2048];
    int	len;

	fds[0].fd = DeviceSoc;
	fds[0].events = POLLIN | POLLERR;

	while(EndFlag == 0){
		switch((nready = poll(fds, 1, 1000))){
			case -1:
				perror("poll");
				break;
			case 0:
				break;
			default:
                // イベント発生時の処理
				if(fds[0].revents & (POLLIN | POLLERR)){
					if((len = read(DeviceSoc, buf, sizeof(buf))) <= 0){
						perror("read");
					} 
                    else{
                        // Etherフレームの解析
						EtherRecv(DeviceSoc, buf, len);
					}
				}
				break;
		}
	}

	return NULL;
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
    if((soc = socket(PF_PACKET, SOCK_RAW, my_htons(ETH_P_ALL))) < 0){
        perror("socket");
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, device, IFNAMSIZ - 1);
    // interface index を取得
    if(ioctl(soc, SIOCGIFINDEX, &ifr) < 0){
        perror("ioctl");
        close(soc);
        return -1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sll_family = AF_PACKET;
    sa.sll_protocol = my_htons(ETH_P_ALL);
    sa.sll_ifindex = ifr.ifr_ifindex;
    // socketにインターフェースをbind
    if(bind(soc, (struct sockaddr *)&sa, sizeof(sa)) < 0){
		perror("bind");
		close(soc);
		return -1;
	}

    return soc;
}

/**
 * @brief 終了時のシグナルハンドラ
 * 
 * @param signal 
 */
void sig_term(int signal)
{
    EndFlag = 1;
}

/**
 * @brief エントリーポイント
 */
int main(int argc, char *argv[])
{
    char buf[80];
    int i;

    // 擬似乱数のseedを設定
    srandom(time(NULL));

    IpRecvBufInit();

    // TODO: ファイルから読み込む
    Param.device = "enx9096f34a568d";
    my_ether_aton("02:00:00:00:00:01", Param.vmac);
    Param.vip = my_inet_aton("192.168.200.112");
    Param.vmask = my_inet_aton("255.255.255.0");
    Param.gateway = my_inet_aton("192.168.200.1");

    puts("+----------------------------------+");
	printf("vmac    = %s\n", my_ether_ntoa(Param.vmac, buf));
	printf("vip     = %s\n", my_inet_ntoa(Param.vip, buf)); 
	printf("vmask   = %s\n", my_inet_ntoa(Param.vmask, buf)); 
	printf("gateway = %s\n", my_inet_ntoa(Param.gateway, buf)); 
    puts("+----------------------------------+");

    if((DeviceSoc = init_socket(Param.device)) == -1){
		exit(-1);
	}

    // 終了時のシグナルハンドラをセット
	signal(SIGINT, sig_term);
	signal(SIGTERM, sig_term);
	signal(SIGQUIT, sig_term);

    // SIGIPEによる予期しない終了を防ぐ
	signal(SIGPIPE, SIG_IGN);

    // TODO: スレッドの作成
    EthThread(NULL);

    // ARPテーブルの開放
    for(i=0; i<ARP_TABLE_SIZE; i++){
        free(ArpHashTable[i]);
    }

    printf("Good Bye...\n");
    return 0;
}