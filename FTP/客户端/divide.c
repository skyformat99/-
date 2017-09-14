#include<string.h>
#include<stdlib.h>
#include<stdio.h>
void DivideStr(char *s, char mark,char SaveStr[10][10])  //°´ÕÕmark×Ö·û×÷Îª·Ö¸î·ûÇÐ¸î×Ö·û´®
{
	int length = strlen(s);
	int num = 0;
	int counter = 0;
	for (int i = 0; i < length; i++)
	{
		
		if (s[i] != mark&&s[i]!='\0'&&s[i] != '\n')
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