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
#import "c:\Program Files\Common Files\System\ADO\msado15.dll"  no_namespace rename("EOF", "EndOfFile")  //�����ļ�
using namespace std;

int Compare(char a[MAX], char b[MAX])  //�ַ����ȽϺ�����ϵͳ�Դ��Ĳ�����
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
	char temp[100];       //��ʱ���������ڴ洢��ѯ�Ĳ��ֽ��
	_RecordsetPtr pRst;
	char sqlcommand[1024];
	strcpy_s(sqlcommand, "select num2 from table1 where num1='");
	strcat_s(sqlcommand, ch);
	strcat_s(sqlcommand, "';");
	//_RecordsetPtr pRst;
	_ConnectionPtr  sqlSp;
	_bstr_t strConnect = "Provider = SQLOLEDB.1; Integrated Security = SSPI; Persist Security Info = False; Initial Catalog = test; Data Source = .";//�������ݿ��ַ���
																																					 //��ʼ��COM��  
	::CoInitialize(NULL);
	//���һ��ָ��Connection�����ָ��m_pConnection  
	_ConnectionPtr m_pConnection(__uuidof(Connection));
	//����Connection����  
	if (FAILED(m_pConnection.CreateInstance(__uuidof(Connection))))
	{
		cout << "����Connection����ʱ����\n";
	}

	try
	{
		//�������ݿ�  
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
	}
	catch (_com_error e)
	{
		cout << "�������ݿ�ʱ����\n ";
		cout << e.Description();
		cin.get();
		return 1;
	}
	try                                   //����ѯ���Ľ���洢�ڼ�¼��
	{
		pRst = m_pConnection->Execute(sqlcommand, NULL, adCmdText);
		//ִ��SQL�� select * from Table_BB 
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

		//�洢���������������ʾ�������
		for (int i = 0; i< pRst->Fields->GetCount(); i++)
		{
			cout << pRst->Fields->GetItem(_variant_t((long)i))->Name << " ";
			column_name.push_back(pRst->Fields->GetItem(_variant_t((long)i))->Name);
		}
		cout << endl;

		//�Ա���б�������,��ʾ����ÿһ�е�����
		while (!pRst->EndOfFile)
		{
			vector<_bstr_t>::iterator iter = column_name.begin();
			for (iter; iter != column_name.end(); iter++)
			{
				if (pRst->GetCollect(*iter).vt != VT_NULL)
				{
					cout << (_bstr_t)pRst->GetCollect(*iter) << " ";
					try {
						strcpy_s(temp, (_bstr_t)pRst->GetCollect(*iter));        //�ȰѲ鵽��һ������洢��temp
																				 //strcat_s(result, sizeof((char *)((_bstr_t)pRst->GetCollect(*iter))), (char *)((_bstr_t)pRst->GetCollect(*iter)));
						strcat_s(result, sizeof(temp), temp);       //��temp�Ľ�����ӵ�result
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
	printf("��ѯ�û��ɹ�\n");

	//�ͷų���ռ�õ�COM ��Դ  
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
	SOCKADDR_IN in_addr;        //�����ַ

								//addrSrv.sin_port = htons(MSG_HOST_PORT);      //set the port number  
	char DecryptMSG[MAX];
	FILE *log;   //д��־
	char Buf[MAX];
	cout << "������Ϣ�߳������ɹ�\n";
	while (true)
	{


		recvfrom(socServer, Buf, MAX, 0, (SOCKADDR*)&in_addr, &fromlen);
		fopen_s(&log, "msg.txt", "a+");
		fputs(Buf, log);
		fputc('\n', log);
		fclose(log);                                        //д��־
															//RSASignFile("pri", "log.txt", "RSASign.txt");
															//string De = ECB_AESDecryptStr(aesKey, cRecvBuf);   //����
															//cout << "�յ�һ����Ϣ: \n" << ECB_AESDecryptStr(aesKey, Buf) << endl;
		addr_Srv.sin_port = htons(LOG_HOST_PORT);

		//bind socket to  the host  
		bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));
		for (int num = 0; num<Client_Num; num++)
		{

			//strcpy_s(Message, sizeof(Client[i].Name), Client[i].Name);
			//	strcat_s(Message, "#");
			//strcat_s(Message, inet_ntoa(Client[i].addr_Clt.sin_addr));  //IP��ַת�����ַ���
			//strcpy_s(EnCodeSend, sizeof(ECB_AESEncryptStr(aesKey, Message)), ECB_AESEncryptStr(aesKey, Message).c_str());
			//��Ϣ��ʽ�����ֺ͵�ַ��#�ֿ�
			sendto(socServer, Buf, strlen(Buf) + 1, 0, (SOCKADDR*)&Client[num].addr_Clt, sizeof(SOCKADDR));
			//ת����Ϣ
		}
		addr_Srv.sin_port = htons(MSG_HOST_PORT);

		//bind socket to  the host  
		bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));

	}
}