
#ifndef functions_H
#define functions_H
#include"SQL.h"
/*
�淶���ݰ���ʽΪ������#��������#���������ļ��ľ���·��#������Ϣ#����
 �����������Ϊվ���ţ���ô����������·������Ŀ���˵�name
*/
class Request
{
public:
	char message[1024]; //��������
	char userName[16];
	char RequType;//��������
	char url[64];   //������ϴ��������أ���Ҫ��·��
	char tips[512];      //������ͼƬ�����ļ���˵������
	char date[16]; //����
	pthread_mutex_t request_mutex;
	Request()           //���캯����ȫ����ʼ��Ϊ��
	{
		pthread_mutex_init(&this->request_mutex, NULL);
		int i;
		for (i = 0; i < 512; i++)
		{
			if (i < 16)
			{
				userName[i] = '\0';
				date[i] = '\0';

			}
			if (i < 64)
				url[i] = '\0';
			if (i < 512)
				tips[i] = '\0';
			RequType = '\0';
		}
	};
	Request(const Request &requ1)
	{
		strcpy_s(message, requ1.message);
		strcpy_s(userName,  requ1.userName);
		RequType = requ1.RequType;
		strcpy_s(url,  requ1.url);
		strcpy_s(tips,  requ1.tips);
		strcpy_s(date,  requ1.date);
		pthread_mutex_init(&this->request_mutex, NULL);
	}
	int DealMsg()
	{
		int i = 0;
		int bit_num = 0;
		int type=0;
		while (message[i] != '\0')
		{
			if (message[i] == '#')
			{
				type++;
				bit_num = 0;
			}
			if (message[i] != '#'&&type==0)
			{
				userName[bit_num] = message[i];
				bit_num++;

			}
			if (message[i] != '#'&&type == 1)
			{
				RequType = message[i];
				bit_num++;

			}
			if (message[i] != '#'&&type == 2)
			{
				url[bit_num] = message[i];
				bit_num++;

			}
			if (message[i] != '#'&&type == 3)
			{
				tips[bit_num] = message[i];
				bit_num++;

			}
			if (message[i] != '#'&&type == 4)
			{
				date[bit_num] = message[i];
				bit_num++;

			}
		}
		return 0;
	};

	int UniteMsg()  //��װ��Ϣ
	{
		strcpy_s(message,  userName);
		strcat_s(message,  "#");  //��װ����
		message[strlen(message)] = RequType;  //��װ������
		strcat_s(message,  "#");
		strcat_s(message,  url);           //��װ·��
		strcat_s(message,  "#");
		strcat_s(message,  tips);
		strcat_s(message,  "#");
		strcat_s(message,  date);
	}
	~Request()
	{
		pthread_mutex_destroy(&this->request_mutex);
	}
};

#endif
