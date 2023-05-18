#include <stdio.h>
#include <string.h>

#include "cmd.h"

/**
 * @brief コマンド入力を解析
 * 
 * @param cmd 
 * @return int 
 */
int AnalysCmd(char *cmd)
{
    char *ptr, *saveptr;

    // 入力がない場合の処理
	if((ptr = strtok_r(cmd, " \r\n", &saveptr)) == NULL){
		printf("---------------------------------------\n");
		printf("No command input\n");
        printf("Please refer to \"help\"\n");
		return -1;
	}

    if(strcmp(ptr, "help") == 0){
		//DoCmdArp(&saveptr);
        //ExecHelp();
        printf("command not support : %s\n", ptr);
		return 0;
	} else if(strcmp(ptr, "arp") == 0){
		//DoCmdArp(&saveptr);
        printf("command not support : %s\n", ptr);
		return 0;
	} else if(strcmp(ptr, "ping") == 0){
		//DoCmdPing(&saveptr);
        printf("command not support : %s\n", ptr);
		return 0;
	} else if(strcmp(ptr, "ifconfig") == 0){
		//DoCmdIfconfig(&saveptr);
        printf("command not support : %s\n", ptr);
		return 0;
	} else if(strcmp(ptr, "exit") == 0){
		//DoCmdEnd(&saveptr);
        printf("command not support : %s\n", ptr);
		return 0;
	} else{
		printf("command not found : %s\n", ptr);
		return -1;
	}
}
