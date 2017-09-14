
#ifndef TransMsg_H
#define TransMsg_H

#define MAX 1024
//define host IP and usable port.  
#define HOST_IP 127.0.0.1  
#define MSG_HOST_PORT 8080  
#define LOG_HOST_PORT 8081
#define OK_STR "连接服务器成功"  
#define ERROR "密码错误"
#define NOT_FIND "查无此人"
string aesKey = "0123456789ABCDEF0123456789ABCDEF";//256bits, also can be 128 bits or 192bits  
												   //#import "c:\Program Files\Common Files\System\ADO\msado15.dll"  no_namespace rename("EOF", "EndOfFile")  //置入文件
int Client_Num = 0;
UserInfo Client[20];

int insert(void);
//int del(char ch[100]);          //ch是传入sql语句中的where=的元素
int select(char ch[MAX], SOCKET socServer, SOCKADDR_IN addr_Clt, char result[MAX]); //ch同上，pRst是将数据传出的结构
int change(void);
void *TransMSG(void *hi);
int Compare(char a[MAX], char b[MAX]);
#endif


