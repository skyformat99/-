#include"SQL.h"
#include<iostream>
int Compare(char a[MAX], char b[MAX])  //字符串比较函数，系统自带的不好用
{
	for (int i = 0; i < 1; i++)
	{
		if (a[i] != b[i])
			return -1;
	}
	return 0;
};


_RecordsetPtr select(char ch[MAX], SOCKET socServer, SOCKADDR_IN addr_Clt, char result[1024],char table[16])
{
	for (int i = 0; i < MAX; i++)
	{
		result[i] = '\0';
	}
	char temp[100];       //临时变量，用于存储查询的部分结果
	_RecordsetPtr pRst;
	char sqlcommand[1024];
	strcpy_s(sqlcommand, "select num2 from ");
		strcat_s(sqlcommand, table);
	strcat_s(sqlcommand, "  where num1 = '");

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
			 //使用容器把列名加进去
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
		return -1;
	}
	m_pConnection->Close();
	printf("查询用户成功\n");

	//释放程序占用的COM 资源  
	::CoUninitialize();

	return pRst;
}
