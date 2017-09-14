#ifndef DIVIDE_H
#define DIVIDE_H

void DivideStr(char *s, char mark, char  SaveStr[10][10]);         //C好像不支持引用
void DivideStr(char *s, char mark, char  *userName,char* userPwd);  //重载,使用引用，传出参数
#endif