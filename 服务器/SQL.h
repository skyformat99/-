
#ifndef SQL_H
#define SQL_H
#include <stdio.h>  
#include <tchar.h>  
#include<iostream>
#include <vector>

#include "Winsock2.h"  
#include"functions.h"
#include "string"  
#include<pthread.h>
#import "c:\Program Files\Common Files\System\ADO\msado15.dll"  no_namespace rename("EOF", "EndOfFile")  //�����ļ�
using namespace std;
#define MAX 1024
/*
   ���ݿ⽨���׼������ �����ڣ��ļ�·����tips ���û��ļ����ݿ⣩
             ���������룻ע�����ڣ��û��ʻ��⣩
*/
int Compare(char a[MAX], char b[MAX]);
int Insert(Request req,char table[16]);  //��һ�������Ǵ����������ݽṹ���ڶ�����������
int Delete(char DelRow[MAX], char table[16]);//  ɾ��ĳһ��

_RecordsetPtr select(char ch[MAX], SOCKET socServer, SOCKADDR_IN addr_Clt, char result[MAX],char table[16]); //chͬ�ϣ�pRst�ǽ����ݴ����Ľṹ
#endif
