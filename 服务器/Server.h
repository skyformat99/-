
#ifndef Server_H
#define Server_H

#include "Winsock2.h"  
#include<string>
#define MAX 1024
#define ONLINE 1
#define OFFINE 0
#define INSERT 'I'
#define DELETE 'D'
#define UPDATE 'U'
#define SLECT 'S'
#define P2P_MSG 'M'  //客户发送的点对点消息
struct UserInfo       //每一个连接上的用户信息
{
	char Name[MAX];   //账号
	SOCKADDR_IN addr_Clt;  //IP地址
	int state;     //登录状态
};
int   Login(UserInfo Client[20]);

#endif
