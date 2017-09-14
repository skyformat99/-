#include"myAES.h"
#include"Server.h"
#include<iostream>
#include <stdio.h>  
#include <tchar.h>  
#include<iostream>
#include <vector>

#include "Winsock2.h"  
 
#include "string"  
#include<pthread.h>


#include"mySHA.h"
#include"TransMsg.h"
#pragma comment(lib, "pthreadVC2.lib")
#pragma comment(lib,"ws2_32.lib")
//should add the under line if not add lib in the project.(appendicular dependent)  
//#pragma comment(lib, "ws2_32.lib")  
#import "c:\Program Files\Common Files\System\ADO\msado15.dll"  no_namespace rename("EOF", "EndOfFile")  //置入文件
using namespace std;

int Compare(char a[MAX], char b[MAX])  //字符串比较函数，系统自带的不好用
{
	for (int i = 0; i < 1; i++)
	{
		if (a[i] != b[i])
			return -1;
	}
	return 0;
};


int select(char ch[MAX], SOCKET socServer, SOCKADDR_IN addr_Clt, char result[1024])
{
	for (int i = 0; i < MAX; i++)
	{
		result[i] = '\0';
	}
	char temp[100];       //临时变量，用于存储查询的部分结果
	_RecordsetPtr pRst;
	char sqlcommand[1024];
	strcpy_s(sqlcommand, "select num2 from table1 where num1='");
	strcat_s(sqlcommand, ch);
	strcat_s(sqlcommand, "';");
	//_RecordsetPtr pRst;
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
	try                                   //将查询到的结果存储在记录集
	{
		pRst = m_pConnection->Execute(sqlcommand, NULL, adCmdText);
		//执行SQL： select * from Table_BB 
		if (!pRst->BOF)
		{
			pRst->MoveFirst();
		}
		else
		{
			cout << "Data is empty!" << endl;
			return -1;
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
					try {
						strcpy_s(temp, (_bstr_t)pRst->GetCollect(*iter));        //先把查到的一个结果存储在temp
																				 //strcat_s(result, sizeof((char *)((_bstr_t)pRst->GetCollect(*iter))), (char *)((_bstr_t)pRst->GetCollect(*iter)));
						strcat_s(result, sizeof(temp), temp);       //把temp的结果连接到result
					}
					catch (_com_error me)
					{
						cout << endl << me.Description() << endl;
					}

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
	m_pConnection->Close();
	printf("查询用户成功\n");

	//释放程序占用的COM 资源  
	::CoUninitialize();

	return pRst;
}

void * TransMSG(void *hi)
{

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
	addr_Srv.sin_port = htons(MSG_HOST_PORT);

	//bind socket to  the host  
	bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));
	int fromlen = sizeof(SOCKADDR);
	SOCKADDR_IN in_addr;        //接入地址

								//addrSrv.sin_port = htons(MSG_HOST_PORT);      //set the port number  
	char DecryptMSG[MAX];
	FILE *log;   //写日志
	char Buf[MAX];
	cout << "聆听消息线程启动成功\n";
	while (true)
	{


		recvfrom(socServer, Buf, MAX, 0, (SOCKADDR*)&in_addr, &fromlen);
		fopen_s(&log, "msg.txt", "a+");
		fputs(Buf, log);
		fputc('\n', log);
		fclose(log);                                        //写日志
															//RSASignFile("pri", "log.txt", "RSASign.txt");
															//string De = ECB_AESDecryptStr(aesKey, cRecvBuf);   //解密
															//cout << "收到一条消息: \n" << ECB_AESDecryptStr(aesKey, Buf) << endl;
		addr_Srv.sin_port = htons(LOG_HOST_PORT);

		//bind socket to  the host  
		bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));
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
		addr_Srv.sin_port = htons(MSG_HOST_PORT);

		//bind socket to  the host  
		bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));

	}
}