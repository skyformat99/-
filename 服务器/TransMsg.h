
#ifndef TransMsg_H
#define TransMsg_H

#define MAX 1024
//define host IP and usable port.  
#define HOST_IP 127.0.0.1  
#define MSG_HOST_PORT 8080  
#define LOG_HOST_PORT 8081
#define OK_STR "���ӷ������ɹ�"  
#define ERROR "�������"
#define NOT_FIND "���޴���"
string aesKey = "0123456789ABCDEF0123456789ABCDEF";//256bits, also can be 128 bits or 192bits  
												   //#import "c:\Program Files\Common Files\System\ADO\msado15.dll"  no_namespace rename("EOF", "EndOfFile")  //�����ļ�
int Client_Num = 0;
UserInfo Client[20];

int insert(void);
//int del(char ch[100]);          //ch�Ǵ���sql����е�where=��Ԫ��
int select(char ch[MAX], SOCKET socServer, SOCKADDR_IN addr_Clt, char result[MAX]); //chͬ�ϣ�pRst�ǽ����ݴ����Ľṹ
int change(void);
void *TransMSG(void *hi);
int Compare(char a[MAX], char b[MAX]);
#endif


