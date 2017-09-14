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
#import "c:\Program Files\Common Files\System\ADO\msado15.dll"  no_namespace rename("EOF", "EndOfFile")  //�����ļ�
using namespace std;
//#define MAX 1024
//define host IP and usable port.  
#define HOST_IP 127.0.0.1  
#define REQUEST_HOST_PORT 8080  
#define SOLVE_MSG_PORT   8912
#define LOG_HOST_PORT 8081
#define OK_STR "���ӷ������ɹ�"  
#define ERROR "�������"
#define NOT_FIND "���޴���"
#define MAX_CLIENT 128  //�����������
#define MAX_REQU 256     //���������
#define NO_MSG "NULLMSG"
string aesKey = "0123456789ABCDEF0123456789ABCDEF";//256bits, also can be 128 bits or 192bits  
												   //#import "c:\Program Files\Common Files\System\ADO\msado15.dll"  no_namespace rename("EOF", "EndOfFile")  //�����ļ�
int insert(void);
//int del(char ch[100]);          //ch�Ǵ���sql����е�where=��Ԫ��
//_RecordsetPtr select(char ch[MAX], SOCKET socServer, SOCKADDR_IN addr_Clt, char result[MAX]); //chͬ�ϣ�pRst�ǽ����ݴ����Ľṹ
int change(void);
void *RecvMSG(void *hi);
void *SolveMSG(void *hi);           //Ϊ�˱���������ʹ��recv����������Ϣ��ʹ��send����������Ϣ�����У�recv�߳���
                                     //send�̷߳�����Ϣ��ʵ�ַ�ʽΪ���ο�������������ģ�ͣ�recv�������ߣ��������
                              //��д����Ϣ��send�߳��������ߣ���ȡ��Ϣ�����ù��������
int Client_Num = 0;
UserInfo Client[MAX_CLIENT];      //������Ҷ��У��ô�����洢��ǰ�����û���Ϣ,ͬʱ�ʴ�128������
Request Requ[MAX_REQU]; //     �������������


int  main()
{
	FILE *file;

	pthread_t RECV_Server;                //�����߳̽�����Ϣ
	pthread_create(&RECV_Server, NULL, RecvMSG, 0); //Ϊ��ʹ���治����������ʹ�ö��߳�,������������Ϣ���߳�
													 //the version bit of Winsock  

	char result[MAX];             //���ڴ洢��ѯ���Ľ��

	int version_a = 1;//low bit  
	int version_b = 1;//high bit  
	vector<_bstr_t> column_name;            //����
	char opcode[100];                   //������
	strcpy_s(opcode, "\0");        //��ʼ��������
	_RecordsetPtr pRst;
	char in_message[MAX][100];
	char out_message[MAX][100];           //���������message
										  //makeword  
										  //     string teststr = ECB_AESEncryptStr(aesKey, "hello");����AES���ܺ���
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
	*second param is type of socket, SOCK_DGRAM is for Data Gram��UDP��, and SOCK_STREAM is for TCP.
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
	}        //��ʼ��result
	char EnCodeSend[MAX];   //���ܺ���Ϣ
	int fromlen = sizeof(SOCKADDR);
	// while() to listen all the time  
	char logInfo[64];
	while (true)
	{

		recvfrom(socServer, recvBuf, MAX, 0, (SOCKADDR*)&addr_Clt, &fromlen);        //��ȡ��Ϣ���������������Ϣ�� �˺�#����
		strcpy_s(logInfo, ECB_AESDecryptStr(aesKey, recvBuf).c_str());																		//output the data received to cmd  
																																															//sendto(socServer, OK_STR, strlen(OK_STR) + 1, 0, (SOCKADDR*)&addr_Clt, sizeof(SOCKADDR));//��client����ok��Ϣ
		//recvfrom(socServer, recvBuf, MAX, 0, (SOCKADDR*)&addr_Clt, &fromlen);       //�ٴν�����Ϣ�������Ϣ������

		//strcpy_s(Pwd, sizeof(ECB_AESDecryptStr(aesKey, recvBuf).c_str()), ECB_AESDecryptStr(aesKey, recvBuf).c_str());  //�ܵ��˺����벢����
		DivideStr(logInfo, '#', Name, Pwd);                 //�˺�+����ĸ�ʽΪ name#pwd��������һ����Ϣ�С�

		if (select(Name, socServer, addr_Clt, result,"User") != -1)  //������-1��ʾ���ҵ�����,db��Ϊuser
		{


			if (Compare(result, Pwd) == 0)  //��֤�ɹ�
			{

				//	string newstring;
				//char msg[1024];
				//newstring = ECB_AESEncryptStr(aesKey, OK_STR);
				//	strcpy_s(msg,sizeof(newstring.c_str()),  newstring.c_str());
				//strcpy_s(EnCodeSend, sizeof(msg), msg);
				//strcpy_s(EnCodeSend, sizeof(ECB_AESEncryptStr(aesKey, OK_STR).c_str()), ECB_AESEncryptStr(aesKey, OK_STR).c_str());  //����
				sendto(socServer, ECB_AESEncryptStr(aesKey, OK_STR).c_str(), strlen(ECB_AESEncryptStr(aesKey, OK_STR).c_str()) + 1, 0, (SOCKADDR*)&addr_Clt, sizeof(SOCKADDR)); //���ͳɹ���Ϣ

			
				if (Client_Num < MAX_CLIENT)
				{
					strcpy_s(Client[Client_Num].Name, sizeof(Name), Name);
					Client[Client_Num].addr_Clt = addr_Clt;              //���������д��һ���ṹ�����м�¼
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
						strcat_s(Message, inet_ntoa(Client[i].addr_Clt.sin_addr));  //IP��ַת�����ַ���
																					//strcpy_s(EnCodeSend, sizeof(ECB_AESEncryptStr(aesKey, Message)), ECB_AESEncryptStr(aesKey, Message).c_str());
																					//��Ϣ��ʽ�����ֺ͵�ַ��#�ֿ�
						sendto(socServer, ECB_AESEncryptStr(aesKey, Message).c_str(), strlen(ECB_AESEncryptStr(aesKey, Message).c_str()) + 1, 0, (SOCKADDR*)&Client[num].addr_Clt, sizeof(SOCKADDR));
						//�����������������Ϣ,��һ��ѭ�����������˱�������ͣ��ڶ���ѭ��������Ϣ���͵�ÿһ�������ˡ�
					}
				}

				//cout << "test3...." << endl;
			}
			else
			{
				sendto(socServer, ECB_AESEncryptStr(aesKey, ERROR).c_str(), strlen(ECB_AESEncryptStr(aesKey, ERROR).c_str()) + 1, 0, (SOCKADDR*)&addr_Clt, sizeof(SOCKADDR)); //���ͳɹ���Ϣ

			}
			/*
			else
			{
		
			strcpy_s(EnCodeSend, sizeof(ECB_AESEncryptStr(aesKey, ERROR).c_str())+1, ECB_AESEncryptStr(aesKey, ERROR).c_str());  //����
			sendto(socServer, EnCodeSend, strlen(EnCodeSend) + 1, 0, (SOCKADDR*)&addr_Clt, sizeof(SOCKADDR)); //����error
			fopen_s(&file, "log", "a+");
			fputs(Name, file);
			fputs("  was refused because of  wrong pwd \n", file);
			fclose(file);
			}
			*/
		}
		else    //���޴��˵����
		{
			strcpy_s(EnCodeSend, sizeof(ECB_AESEncryptStr(aesKey, NOT_FIND)), ECB_AESEncryptStr(aesKey, NOT_FIND).c_str());  //����
			sendto(socServer, EnCodeSend, strlen(EnCodeSend) + 1, 0, (SOCKADDR*)&addr_Clt, sizeof(SOCKADDR)); //����not found

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
//�����¼������ʵ��������


//_RecordsetPtr m_pRecordset;
char sqlcommand2[1024];
strcpy_s(sqlcommand2 ,"delete from table1 where num1='");
strcat_s(sqlcommand2, ch);
strcat_s(sqlcommand2, "';");              //����sql���
//sprintf_s(sqlcommand2, (size_t)"delete from table1 where num1='%s'", ch);
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

m_pConnection->Execute(sqlcommand2, NULL, 1);//ִ��SQL���
//m_pRecordset->Open("select * from  table1", (IDispatch*)sqlSp, adOpenDynamic, adLockOptimistic, adCmdText);
/*
try                                   //����ѯ���Ľ���洢�ڼ�¼��
{
pRst = m_pConnection->Execute("select * from table1", NULL, adCmdText);
//ִ��SQL�� select * from Table_BB
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
printf("�ɹ�\n");
system("pause");
//�ͷų���ռ�õ�COM ��Դ
::CoUninitialize();
return 0;
}


*/

void * RecvMSG(void *hi)  //���������Ϣ��Ȼ������Ϣ��ͨ�Ÿ���һ�����̣���һ������ִ��send����
{
	int index = 0;  //�����߽��������������ļ�����
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
	SOCKADDR_IN in_addr;        //�����ַ

								//addrSrv.sin_port = htons(MSG_HOST_PORT);      //set the port number  
	char DecryptMSG[MAX];
	string tmpstr;
	FILE *log;   //д��־
	char Buf[MAX];
	cout << "������Ϣ�߳������ɹ�\n";
	while (true)
	{


		recvfrom(socServer, Buf, MAX, 0, (SOCKADDR*)&in_addr, &fromlen);
		tmpstr=ECB_AESDecryptStr(aesKey, Buf);  //������Ϣ
		strcpy_s(DecryptMSG, MAX, tmpstr.c_str());
		fopen_s(&log, "msg.txt", "a+");
		fputs(DecryptMSG, log);
		fputc('\n', log);
		fclose(log);                                        //д��־
															//RSASignFile("pri", "log.txt", "RSASign.txt");
															//string De = ECB_AESDecryptStr(aesKey, cRecvBuf);   //����
															//cout << "�յ�һ����Ϣ: \n" << ECB_AESDecryptStr(aesKey, Buf) << endl;
		//addr_Srv.sin_port = htons(LOG_HOST_PORT);

		//bind socket to  the host  
		//bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));
		strcpy_s(requ.message, MAX, DecryptMSG);
		requ.DealMsg();
		
		while (1)  //�յ���Ϣд�봦�����
		{
			if (Compare(Requ[index].message,NO_MSG)==0&&pthread_mutex_trylock(&Requ[index].request_mutex) == 0)  
				//�����ɹ�
			{
				Requ[index] = requ;
				pthread_mutex_unlock(&Requ[index].request_mutex); //����
				break;   //�����߳ɹ�д��������У��˳�ѭ��
			}
			else index++;
		}
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

		
		//addr_Srv.sin_port = htons(MSG_HOST_PORT);

		//bind socket to  the host  
		//bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));

	}
}

void *SolveMSG(void *hi)
{
	int index = 0;  //���ѽ��������������ļ�����
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
	SOCKADDR_IN in_addr;        //�����ַ

								//addrSrv.sin_port = htons(MSG_HOST_PORT);      //set the port number  
	char DecryptMSG[MAX];
	string tmpstr;
	FILE *log;   //д��־
	char Buf[MAX];
	Request Solve;
	while (true)
	{

		/*
		recvfrom(socServer, Buf, MAX, 0, (SOCKADDR*)&in_addr, &fromlen);
		tmpstr = ECB_AESDecryptStr(aesKey, Buf);  //������Ϣ
		strcpy_s(DecryptMSG, MAX, tmpstr.c_str());
		fopen_s(&log, "msg.txt", "a+");
		fputs(DecryptMSG, log);
		fputc('\n', log);
		fclose(log);                                        //д��־
															//RSASignFile("pri", "log.txt", "RSASign.txt");
															//string De = ECB_AESDecryptStr(aesKey, cRecvBuf);   //����
															//cout << "�յ�һ����Ϣ: \n" << ECB_AESDecryptStr(aesKey, Buf) << endl;
															//addr_Srv.sin_port = htons(LOG_HOST_PORT);

															//bind socket to  the host  
															//bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));
		strcpy_s(requ.message, MAX, DecryptMSG);
		requ.DealMsg();
		*/
		while (1)  //�յ���Ϣд�봦�����
		{
			if (Compare(Requ[index].message, NO_MSG) != 0 && pthread_mutex_trylock(&Requ[index].request_mutex) == 0)
				//�����ɹ�
			{
				Solve = Requ[index];
				if (Solve.RequType == 'I')   //����sql���� ���
				{
					Insert(Solve, "UserDate");   //�����ΪUserdate�ı������Ϣ
				}
				if (Solve.RequType == 'D')   //����sql���� ɾ��
				{
					Delete(Solve.url, "UserDate");  //
				}
				if (Solve.RequType == 'U')   //����sql����  �޸�
				{

				}
				if (Solve.RequType == 'S')   //����sql����   ����
				{

				}
				pthread_mutex_unlock(&Requ[index].request_mutex); //����
				break;   //�����߳ɹ�д��������У��˳�ѭ��
			}
			else index++;
		}
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


		//addr_Srv.sin_port = htons(MSG_HOST_PORT);

		//bind socket to  the host  
		//bind(socServer, (SOCKADDR*)&addr_Srv, sizeof(SOCKADDR));

	}
}