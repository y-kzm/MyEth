#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>             // signal()
#include <poll.h>               // poll()
#include <stdint.h>             // uintX_t
#include <stdlib.h>             // rand
#include <sys/socket.h>         // socket
#include <linux/if_ether.h>     // ETH_P_ALL
#include <sys/ioctl.h>          // ifreq
#include <net/if.h>             // ifreq 
#include <linux/if_packet.h>    // sockaddr_ll
#include <pthread.h>
#include <errno.h>

#include "param.h"
#include "utils.h"
#include "ether.h"
#include "arp.h"
#include "ip.h"
#include "cmd.h"

int DeviceSoc;
int	EndFlag=0;
struct PARAM Param;

extern struct IP_RECV_BUF IpRecvBuf[IP_RECV_BUF_NO];

/**
 * @brief 標準入力の受信処理
 * 
 * @param argv 
 * @return void* 
 */
void *CommandThread(void *argv)
{
    int	nready;
    struct pollfd targets[1];
    char buf[2048];

	targets[0].fd = fileno(stdin);
	targets[0].events = POLLIN | POLLERR;

	while(EndFlag == 0){
        // Stdinを監視
		switch((nready = poll(targets, 1, 1000))){
			case -1:
				if(errno != EINTR){
					perror("poll");
				}
				break;
			case 0:
				break;
			default:
                // イベント発生
				if(targets[0].revents & (POLLIN | POLLERR)){
					fgets(buf, sizeof(buf), stdin);
					AnalysCmd(buf);
				}
				break;
		}
	}

	return NULL;
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
        // DeviceSocを監視
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
    strncpy(ifr.ifr_name, device, IF_NAMESIZE - 1);
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

    // フラグの取得（SIOCGIFFLAG）
    // Ref: https://linuxjm.osdn.jp/html/LDP_man-pages/man7/netdevice.7.html
    if(ioctl(soc, SIOCGIFFLAGS, &ifr) < 0){
		perror("ioctl");
		close(soc);
		return -1;
	}

    // プロミスキャスモードとインターフェースUPのフラグを立てる
	ifr.ifr_flags = ifr.ifr_flags | IFF_PROMISC | IFF_UP;
    // フラグのセット（SIOCSIFFLAGS）
	if(ioctl(soc, SIOCSIFFLAGS, &ifr) < 0){
		perror("ioctl");
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
    pthread_attr_t	attr;
    pthread_t	thread_id;

    // 擬似乱数のseedを設定
    srandom(time(NULL));

    IpRecvBufInit();

    // TODO: ファイルから読み込む
    Param.device = "enx9096f34a568d";
    my_ether_aton("02:00:00:00:00:01", Param.vmac);
    Param.vip = my_inet_aton("192.168.100.100");
    Param.vmask = my_inet_aton("255.255.255.0");
    Param.gateway = my_inet_aton("192.168.100.1");

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

	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 102400);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    // Ethernetフレーム受信用スレッドの作成
    if(pthread_create(&thread_id, &attr, EthThread, NULL) != 0){
		printf("pthread_create: error\n");
	}
    // Command入力用スレッドの作成
	if(pthread_create(&thread_id, &attr, CommandThread, NULL) != 0){
		printf("pthread_create: error\n");
	}

    while(EndFlag == 0){
		sleep(1);
	}

    // ARPテーブルの開放
    FreeArpTable();
    printf("Good Bye...\n");
    return 0;
}
