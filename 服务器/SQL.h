
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
#import "c:\Program Files\Common Files\System\ADO\msado15.dll"  no_namespace rename("EOF", "EndOfFile")  //置入文件
using namespace std;
#define MAX 1024
/*
   数据库建库标准：姓名 ；日期，文件路径，tips （用户文件数据库）
             姓名；密码；注册日期（用户帐户库）
*/
int Compare(char a[MAX], char b[MAX]);
int Insert(Request req,char table[16]);  //第一个参数是传进来的数据结构，第二个参数表名
int Delete(char DelRow[MAX], char table[16]);//  删除某一行

_RecordsetPtr select(char ch[MAX], SOCKET socServer, SOCKADDR_IN addr_Clt, char result[MAX],char table[16]); //ch同上，pRst是将数据传出的结构
#endif
