#include"SQL.h"
#include<iostream>
int Compare(char a[MAX], char b[MAX])  //�ַ����ȽϺ�����ϵͳ�Դ��Ĳ�����
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
	char temp[100];       //��ʱ���������ڴ洢��ѯ�Ĳ��ֽ��
	_RecordsetPtr pRst;
	char sqlcommand[1024];
	strcpy_s(sqlcommand, "select num2 from ");
		strcat_s(sqlcommand, table);
	strcat_s(sqlcommand, "  where num1 = '");

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
			 //ʹ�������������ӽ�ȥ
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
		return -1;
	}
	m_pConnection->Close();
	printf("��ѯ�û��ɹ�\n");

	//�ͷų���ռ�õ�COM ��Դ  
	::CoUninitialize();

	return pRst;
}
