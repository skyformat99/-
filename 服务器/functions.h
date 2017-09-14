
#ifndef functions_H
#define functions_H
#include"SQL.h"
/*
规范数据包格式为：名字#请求类型#服务器端文件的绝对路径#附加消息#日期
 如果请求类型为站内信，那么第三个数据路径就是目标人的name
*/
class Request
{
public:
	char message[1024]; //完整报文
	char userName[16];
	char RequType;//请求类型
	char url[64];   //如果是上传或者下载，需要有路径
	char tips[512];      //用作对图片或者文件的说明文字
	char date[16]; //日期
	pthread_mutex_t request_mutex;
	Request()           //构造函数，全部初始化为空
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

	int UniteMsg()  //封装消息
	{
		strcpy_s(message,  userName);
		strcat_s(message,  "#");  //封装名字
		message[strlen(message)] = RequType;  //封装命令字
		strcat_s(message,  "#");
		strcat_s(message,  url);           //封装路径
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
