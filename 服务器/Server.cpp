#pragma once  
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers  
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include"Server.h"
#include"myAES.h"
#include"mySHA.h"
#include"divide.h"
#include"SQL.h"
#include"functions.h"
#pragma comment(lib, "pthreadVC2.lib")
#pragma comment(lib,"ws2_32.lib")
//should add the under line if not add lib in the project.(appendicular dependent)  
//#pragma comment(lib, "ws2_32.lib")  
#import "c:\Program Files\Common Files\System\ADO\msado15.dll"  no_namespace rename("EOF", "EndOfFile")  //置入文件
using namespace std;
//#define MAX 1024
//define host IP and usable port.  
#define HOST_IP 127.0.0.1  
#define REQUEST_HOST_PORT 8080  
#define SOLVE_MSG_PORT   8912
#define LOG_HOST_PORT 8081
#define OK_STR "连接服务器成功"  
#define ERROR "密码错误"
#define NOT_FIND "查无此人"
#define MAX_CLIENT 128  //最大在线人数
#define MAX_REQU 256     //最大请求数
#define NO_MSG "NULLMSG"
string aesKey = "0123456789ABCDEF0123456789ABCDEF";//256bits, also can be 128 bits or 192bits  
												   //#import "c:\Program Files\Common Files\System\ADO\msado15.dll"  no_namespace rename("EOF", "EndOfFile")  //置入文件
int insert(void);
//int del(char ch[100]);          //ch是传入sql语句中的where=的元素
//_RecordsetPtr select(char ch[MAX], SOCKET socServer, SOCKADDR_IN addr_Clt, char result[MAX]); //ch同上，pRst是将数据传出的结构
int change(void);
void *RecvMSG(void *hi);
void *SolveMSG(void *hi);           //为了避免阻塞，使用recv函数接收消息，使用send函数发送消息，其中，recv线程向
                                     //send线程发送消息，实现方式为：参考生产者消费者模型，recv是生产者，向共享变量
                              //中写入消息，send线程是消费者，读取消息并重置共享变量。
int Client_Num = 0;
UserInfo Client[MAX_CLIENT];      //在线玩家队列，用此数组存储当前在线用户信息,同时资瓷128连接数
Request Requ[MAX_REQU]; //     待处理任务队列


int  main()
{
	FILE *file;

	pthread_t RECV_Server;                //创建线程接收消息
	pthread_create(&RECV_Server, NULL, RecvMSG, 0); //为了使界面不卡死，我们使用多线程,服务器接收消息的线程
													 //the version bit of Winsock  

	char result[MAX];             //用于存储查询到的结果

	int version_a = 1;//low bit  
	int version_b = 1;//high bit  
	vector<_bstr_t> column_name;            //列名
	char opcode[100];                   //操作码
	strcpy_s(opcode, "\0");        //初始化操作码
	_RecordsetPtr pRst;
	char in_message[MAX][100];
	char out_message[MAX][100];           //输入输出的message
										  //makeword  
										  //     string teststr = ECB_AESEncryptStr(aesKey, "hello");测试AES加密函数
	WORD versionRequest = MAKEWORD(version_a, version_b);
	WSAData wsaData;
	int err;
	//wsa startup  
	err = WSAStartup(versionRequest, &wsaData);

	if (err != 0) {
		printf("ERROR!");
		return 0;
	}
	//check whether the version is 1.1, if not print the error and cleanup wsa?  
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		printf("WRONG WINSOCK VERSION!");
		WSACleanup();
		return 0;
	}

	/*
	*build the socket
	*first param is protocol family, generally AF_INET for IP internet.
	*second param is type of socket, SOCK_DGRAM is for Data Gram（UDP）, and SOCK_STREAM is for TCP.
	*the last param is communicate protocol. Generally is zero.
	*/
	SOCKET socServer = socket(AF_INET, SOCK_DGRAM, 0);
	//infomation of address, always NOT being operated directly.  
	SOCKADDR_IN addr_Srv;
	//Struct sin_addr is  used to defind IP address,  it's a property of addr_in.  
	//It's nest three structs as S_un_b, S_un_w and S-un.(union)  
	addr_Srv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//set protocol family  
	addr_Srv.sin_family = AF_INET;
	//set host port  
	addr_Srv.sin_port = htons(LOG_HOST_PORT);

	//bind socket to  the host  
	bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));
	char Message[MAX];
	//address  of a client  
	SOCKADDR_IN addr_Clt;
	char recvBuf[MAX];
	char Name[MAX];
	char Pwd[MAX];
	for (int i = 0; i < MAX; i++)
	{
		result[i] = '\0';
		Pwd[i] = '\0';
	}        //初始化result
	char EnCodeSend[MAX];   //加密后消息
	int fromlen = sizeof(SOCKADDR);
	// while() to listen all the time  
	char logInfo[64];
	while (true)
	{

		recvfrom(socServer, recvBuf, MAX, 0, (SOCKADDR*)&addr_Clt, &fromlen);        //读取消息到缓冲区，这个消息是 账号#密码
		strcpy_s(logInfo, ECB_AESDecryptStr(aesKey, recvBuf).c_str());																		//output the data received to cmd  
																																															//sendto(socServer, OK_STR, strlen(OK_STR) + 1, 0, (SOCKADDR*)&addr_Clt, sizeof(SOCKADDR));//对client发送ok信息
		//recvfrom(socServer, recvBuf, MAX, 0, (SOCKADDR*)&addr_Clt, &fromlen);       //再次接受消息，这个消息是密码

		//strcpy_s(Pwd, sizeof(ECB_AESDecryptStr(aesKey, recvBuf).c_str()), ECB_AESDecryptStr(aesKey, recvBuf).c_str());  //受到账号密码并解密
		DivideStr(logInfo, '#', Name, Pwd);                 //账号+密码的格式为 name#pwd，整合在一条消息中。

		if (select(Name, socServer, addr_Clt, result,"User") != -1)  //不等于-1表示查找到此人,db名为user
		{


			if (Compare(result, Pwd) == 0)  //验证成功
			{

				//	string newstring;
				//char msg[1024];
				//newstring = ECB_AESEncryptStr(aesKey, OK_STR);
				//	strcpy_s(msg,sizeof(newstring.c_str()),  newstring.c_str());
				//strcpy_s(EnCodeSend, sizeof(msg), msg);
				//strcpy_s(EnCodeSend, sizeof(ECB_AESEncryptStr(aesKey, OK_STR).c_str()), ECB_AESEncryptStr(aesKey, OK_STR).c_str());  //加密
				sendto(socServer, ECB_AESEncryptStr(aesKey, OK_STR).c_str(), strlen(ECB_AESEncryptStr(aesKey, OK_STR).c_str()) + 1, 0, (SOCKADDR*)&addr_Clt, sizeof(SOCKADDR)); //发送成功消息

			
				if (Client_Num < MAX_CLIENT)
				{
					strcpy_s(Client[Client_Num].Name, sizeof(Name), Name);
					Client[Client_Num].addr_Clt = addr_Clt;              //把在线玩家写入一个结构数组中记录
					Client[Client_Num].state = ONLINE;
					Client_Num++;
				}
				fopen_s(&file, "log", "a+");
				fputs(Name, file);
				fputs("   log in \n", file);
				cout << Name << " Log in\n";
				fclose(file);
				for (int num = 0; num < Client_Num; num++)
				{
					for (int i = 0; i < Client_Num; i++)
					{
						//strcpy_s(Message, sizeof(Client[i].Name), Client[i].Name);
						strcpy_s(Message, sizeof("!"), "!");
						strcat_s(Message, Client[i].Name);
						strcat_s(Message, "#");
						strcat_s(Message, inet_ntoa(Client[i].addr_Clt.sin_addr));  //IP地址转换成字符串
																					//strcpy_s(EnCodeSend, sizeof(ECB_AESEncryptStr(aesKey, Message)), ECB_AESEncryptStr(aesKey, Message).c_str());
																					//消息格式，名字和地址用#分开
						sendto(socServer, ECB_AESEncryptStr(aesKey, Message).c_str(), strlen(ECB_AESEncryptStr(aesKey, Message).c_str()) + 1, 0, (SOCKADDR*)&Client[num].addr_Clt, sizeof(SOCKADDR));
						//发送所有在线玩家信息,第一层循环遍历在线人表，逐个发送，第二层循环，把消息发送到每一个在线人。
					}
				}

				//cout << "test3...." << endl;
			}
			else
			{
				sendto(socServer, ECB_AESEncryptStr(aesKey, ERROR).c_str(), strlen(ECB_AESEncryptStr(aesKey, ERROR).c_str()) + 1, 0, (SOCKADDR*)&addr_Clt, sizeof(SOCKADDR)); //发送成功消息

			}
			/*
			else
			{
		
			strcpy_s(EnCodeSend, sizeof(ECB_AESEncryptStr(aesKey, ERROR).c_str())+1, ECB_AESEncryptStr(aesKey, ERROR).c_str());  //加密
			sendto(socServer, EnCodeSend, strlen(EnCodeSend) + 1, 0, (SOCKADDR*)&addr_Clt, sizeof(SOCKADDR)); //发送error
			fopen_s(&file, "log", "a+");
			fputs(Name, file);
			fputs("  was refused because of  wrong pwd \n", file);
			fclose(file);
			}
			*/
		}
		else    //查无此人的情况
		{
			strcpy_s(EnCodeSend, sizeof(ECB_AESEncryptStr(aesKey, NOT_FIND)), ECB_AESEncryptStr(aesKey, NOT_FIND).c_str());  //加密
			sendto(socServer, EnCodeSend, strlen(EnCodeSend) + 1, 0, (SOCKADDR*)&addr_Clt, sizeof(SOCKADDR)); //发送not found

		}
	}
	// at last close the socket.  
	closesocket(socServer);
	//cleanup WSAData  
	WSACleanup();
}

/*
int del(char ch[100])
{
cout << "ch==" << ch<<endl;

_RecordsetPtr pRst;
//_RecordsetPtr pRst(__uuidof(Recordset));
//定义记录集对象并实例化对象


//_RecordsetPtr m_pRecordset;
char sqlcommand2[1024];
strcpy_s(sqlcommand2 ,"delete from table1 where num1='");
strcat_s(sqlcommand2, ch);
strcat_s(sqlcommand2, "';");              //构造sql语句
//sprintf_s(sqlcommand2, (size_t)"delete from table1 where num1='%s'", ch);
_ConnectionPtr  sqlSp;
_bstr_t strConnect = "Provider = SQLOLEDB.1; Integrated Security = SSPI; Persist Security Info = False; Initial Catalog = test; Data Source = .";//连接数据库字符串
//初始化COM库
::CoInitialize(NULL);
//添加一个指向Connection对象的指针m_pConnection
_ConnectionPtr m_pConnection(__uuidof(Connection));
//创键Connection对象
if (FAILED(m_pConnection.CreateInstance(__uuidof(Connection))))
{
cout << "创键Connection对象时出错\n";
}

try
{
//连接数据库
m_pConnection->Open(strConnect, "", "", adModeUnknown);
}
catch (_com_error e)
{
cout << "连接数据库时出错\n ";
cout << e.Description();
cin.get();
return 1;
}

m_pConnection->Execute(sqlcommand2, NULL, 1);//执行SQL语句
//m_pRecordset->Open("select * from  table1", (IDispatch*)sqlSp, adOpenDynamic, adLockOptimistic, adCmdText);
/*
try                                   //将查询到的结果存储在记录集
{
pRst = m_pConnection->Execute("select * from table1", NULL, adCmdText);
//执行SQL： select * from Table_BB
if (!pRst->BOF)
{
pRst->MoveFirst();
}
else
{
cout << "Data is empty!" << endl;
return 0;
}
vector<_bstr_t> column_name;

//存储表的所有列名，显示表的列名
for (int i = 0; i< pRst->Fields->GetCount(); i++)
{
cout << pRst->Fields->GetItem(_variant_t((long)i))->Name << " ";
column_name.push_back(pRst->Fields->GetItem(_variant_t((long)i))->Name);
}
cout << endl;

//对表进行遍历访问,显示表中每一行的内容
while (!pRst->EndOfFile)
{
vector<_bstr_t>::iterator iter = column_name.begin();
for (iter; iter != column_name.end(); iter++)
{
if (pRst->GetCollect(*iter).vt != VT_NULL)
{
cout << (_bstr_t)pRst->GetCollect(*iter) << " ";
}
else
{
cout << "NULL" << endl;
}
}
pRst->MoveNext();
cout << endl;
}
}
catch (_com_error &e)
{
cout << e.Description() << endl;
//cout<<e.HelpFile()<<endl;
return 0;
}
*/
/*
m_pConnection->Close();
printf("成功\n");
system("pause");
//释放程序占用的COM 资源
::CoUninitialize();
return 0;
}


*/

void * RecvMSG(void *hi)  //负责接收消息，然后解包消息，通信给另一个进程，另一个进程执行send操作
{
	int index = 0;  //生产者进程在任务队列里的计数器
	Request requ;
	int version_a = 1;//low bit  
	int version_b = 1;//high bit  
					  //makeword  
	WORD versionRequest = MAKEWORD(version_a, version_b);
	WSAData wsaData;
	int error;
	error = WSAStartup(versionRequest, &wsaData);

	if (error != 0) {
		printf("ERROR!");

	}
	//check whether the version is 1.1, if not print the error and cleanup wsa?  
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		printf("WRONG WINSOCK VERSION!");
		WSACleanup();

	}
	char requestStr[MAX];
	/*
	SOCKET socClient;
	SOCKADDR_IN addrSrv;
	//build a sockeet
	socClient = socket(AF_INET, SOCK_DGRAM, 0);
	addrSrv;        // a instance of SOCKADDR_IN, which is used in format of SOCKADDR.
	inet_pton(AF_INET, "127.0.0.1", (void *)&addrSrv.sin_addr.S_un.S_addr);        //set the host IP
	addrSrv.sin_family = AF_INET;     //set the protocol family
	addrSrv.sin_port = htons(CLIENT_PORT);      //set the port number

	// array to store the data that server feedback.
	*/
	SOCKET socServer = socket(AF_INET, SOCK_DGRAM, 0);
	//infomation of address, always NOT being operated directly.  

	//Struct sin_addr is  used to defind IP address,  it's a property of addr_in.  
	//It's nest three structs as S_un_b, S_un_w and S-un.(union)  
	SOCKADDR_IN addr_Srv;
	addr_Srv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//set protocol family  
	addr_Srv.sin_family = AF_INET;
	//set host port  
	addr_Srv.sin_port = htons(REQUEST_HOST_PORT);

	//bind socket to  the host  
	bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));
	int fromlen = sizeof(SOCKADDR);
	SOCKADDR_IN in_addr;        //接入地址

								//addrSrv.sin_port = htons(MSG_HOST_PORT);      //set the port number  
	char DecryptMSG[MAX];
	string tmpstr;
	FILE *log;   //写日志
	char Buf[MAX];
	cout << "聆听消息线程启动成功\n";
	while (true)
	{


		recvfrom(socServer, Buf, MAX, 0, (SOCKADDR*)&in_addr, &fromlen);
		tmpstr=ECB_AESDecryptStr(aesKey, Buf);  //解密消息
		strcpy_s(DecryptMSG, MAX, tmpstr.c_str());
		fopen_s(&log, "msg.txt", "a+");
		fputs(DecryptMSG, log);
		fputc('\n', log);
		fclose(log);                                        //写日志
															//RSASignFile("pri", "log.txt", "RSASign.txt");
															//string De = ECB_AESDecryptStr(aesKey, cRecvBuf);   //解密
															//cout << "收到一条消息: \n" << ECB_AESDecryptStr(aesKey, Buf) << endl;
		//addr_Srv.sin_port = htons(LOG_HOST_PORT);

		//bind socket to  the host  
		//bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));
		strcpy_s(requ.message, MAX, DecryptMSG);
		requ.DealMsg();
		
		while (1)  //收到消息写入处理队列
		{
			if (Compare(Requ[index].message,NO_MSG)==0&&pthread_mutex_trylock(&Requ[index].request_mutex) == 0)  
				//加锁成功
			{
				Requ[index] = requ;
				pthread_mutex_unlock(&Requ[index].request_mutex); //解锁
				break;   //生产者成功写入请求队列，退出循环
			}
			else index++;
		}
		for (int num = 0; num<Client_Num; num++)
		{

			//strcpy_s(Message, sizeof(Client[i].Name), Client[i].Name);
			//	strcat_s(Message, "#");
			//strcat_s(Message, inet_ntoa(Client[i].addr_Clt.sin_addr));  //IP地址转换成字符串
			//strcpy_s(EnCodeSend, sizeof(ECB_AESEncryptStr(aesKey, Message)), ECB_AESEncryptStr(aesKey, Message).c_str());
			//消息格式，名字和地址用#分开
			sendto(socServer, Buf, strlen(Buf) + 1, 0, (SOCKADDR*)&Client[num].addr_Clt, sizeof(SOCKADDR));
			//转发消息

		}

		
		//addr_Srv.sin_port = htons(MSG_HOST_PORT);

		//bind socket to  the host  
		//bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));

	}
}

void *SolveMSG(void *hi)
{
	int index = 0;  //消费进程在任务队列里的计数器
	Request requ;
	int version_a = 1;//low bit  
	int version_b = 1;//high bit  
					  //makeword  
	WORD versionRequest = MAKEWORD(version_a, version_b);
	WSAData wsaData;
	int error;
	error = WSAStartup(versionRequest, &wsaData);

	if (error != 0) {
		printf("ERROR!");

	}
	//check whether the version is 1.1, if not print the error and cleanup wsa?  
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		printf("WRONG WINSOCK VERSION!");
		WSACleanup();

	}
	char requestStr[MAX];
	/*
	SOCKET socClient;
	SOCKADDR_IN addrSrv;
	//build a sockeet
	socClient = socket(AF_INET, SOCK_DGRAM, 0);
	addrSrv;        // a instance of SOCKADDR_IN, which is used in format of SOCKADDR.
	inet_pton(AF_INET, "127.0.0.1", (void *)&addrSrv.sin_addr.S_un.S_addr);        //set the host IP
	addrSrv.sin_family = AF_INET;     //set the protocol family
	addrSrv.sin_port = htons(CLIENT_PORT);      //set the port number

	// array to store the data that server feedback.
	*/
	SOCKET socServer = socket(AF_INET, SOCK_DGRAM, 0);
	//infomation of address, always NOT being operated directly.  

	//Struct sin_addr is  used to defind IP address,  it's a property of addr_in.  
	//It's nest three structs as S_un_b, S_un_w and S-un.(union)  
	SOCKADDR_IN addr_Srv;
	addr_Srv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//set protocol family  
	addr_Srv.sin_family = AF_INET;
	//set host port  
	addr_Srv.sin_port = htons(SOLVE_MSG_PORT);

	//bind socket to  the host  
	bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));
	int fromlen = sizeof(SOCKADDR);
	SOCKADDR_IN in_addr;        //接入地址

								//addrSrv.sin_port = htons(MSG_HOST_PORT);      //set the port number  
	char DecryptMSG[MAX];
	string tmpstr;
	FILE *log;   //写日志
	char Buf[MAX];
	Request Solve;
	while (true)
	{

		/*
		recvfrom(socServer, Buf, MAX, 0, (SOCKADDR*)&in_addr, &fromlen);
		tmpstr = ECB_AESDecryptStr(aesKey, Buf);  //解密消息
		strcpy_s(DecryptMSG, MAX, tmpstr.c_str());
		fopen_s(&log, "msg.txt", "a+");
		fputs(DecryptMSG, log);
		fputc('\n', log);
		fclose(log);                                        //写日志
															//RSASignFile("pri", "log.txt", "RSASign.txt");
															//string De = ECB_AESDecryptStr(aesKey, cRecvBuf);   //解密
															//cout << "收到一条消息: \n" << ECB_AESDecryptStr(aesKey, Buf) << endl;
															//addr_Srv.sin_port = htons(LOG_HOST_PORT);

															//bind socket to  the host  
															//bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));
		strcpy_s(requ.message, MAX, DecryptMSG);
		requ.DealMsg();
		*/
		while (1)  //收到消息写入处理队列
		{
			if (Compare(Requ[index].message, NO_MSG) != 0 && pthread_mutex_trylock(&Requ[index].request_mutex) == 0)
				//加锁成功
			{
				Solve = Requ[index];
				if (Solve.RequType == 'I')   //进行sql操作 添加
				{
					Insert(Solve, "UserDate");   //向表名为Userdate的表里插信息
				}
				if (Solve.RequType == 'D')   //进行sql操作 删除
				{
					Delete(Solve.url, "UserDate");  //
				}
				if (Solve.RequType == 'U')   //进行sql操作  修改
				{

				}
				if (Solve.RequType == 'S')   //进行sql操作   查找
				{

				}
				pthread_mutex_unlock(&Requ[index].request_mutex); //解锁
				break;   //生产者成功写入请求队列，退出循环
			}
			else index++;
		}
		for (int num = 0; num<Client_Num; num++)
		{

			//strcpy_s(Message, sizeof(Client[i].Name), Client[i].Name);
			//	strcat_s(Message, "#");
			//strcat_s(Message, inet_ntoa(Client[i].addr_Clt.sin_addr));  //IP地址转换成字符串
			//strcpy_s(EnCodeSend, sizeof(ECB_AESEncryptStr(aesKey, Message)), ECB_AESEncryptStr(aesKey, Message).c_str());
			//消息格式，名字和地址用#分开
			sendto(socServer, Buf, strlen(Buf) + 1, 0, (SOCKADDR*)&Client[num].addr_Clt, sizeof(SOCKADDR));
			//转发消息

		}


		//addr_Srv.sin_port = htons(MSG_HOST_PORT);

		//bind socket to  the host  
		//bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));

	}
}