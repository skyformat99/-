
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
#define P2P_MSG 'M'  //�ͻ����͵ĵ�Ե���Ϣ
struct UserInfo       //ÿһ�������ϵ��û���Ϣ
{
	char Name[MAX];   //�˺�
	SOCKADDR_IN addr_Clt;  //IP��ַ
	int state;     //��¼״̬
};
int   Login(UserInfo Client[20]);

#endif
