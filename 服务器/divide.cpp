#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include"divide.h"
void DivideStr(char *s, char mark, char SaveStr[10][10])  //°´ÕÕmark×Ö·û×÷Îª·Ö¸î·ûÇÐ¸î×Ö·û´®
{
	int length = strlen(s);
	int num = 0;
	int counter = 0;
	for (int i = 0; i < length; i++)
	{

		if (s[i] != mark&&s[i] != '\0'&&s[i] != '\n')
		{
			SaveStr[num][counter] = s[i];
			counter++;
		}
		else {
			SaveStr[num][i] = '\0';
			counter = 0;
			num++;

		}
	}
	//return SaveStr;
}

void DivideStr(char *s, char mark, char  *userName, char *userPwd) 
{
	int length = strlen(s);
	int num = 0;
	int counter = 0;
	for (int i = 0; i < length; i++)
	{

		if (s[i] != mark&&s[i] != '\0'&&s[i] != '\n')
		{
			userName[counter] = s[i];
			counter++;
		}
		else {
			userName[counter] = '\0';
			counter = 0;
			num++;

		}
		if (num == 1)
		{
			if (s[i] != mark&&s[i] != '\0'&&s[i] != '\n')
			userPwd[counter] = s[i];
			else userPwd[counter] = '\0';
		}
	}
}