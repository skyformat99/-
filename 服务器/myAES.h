
#include<string>
#ifndef myAES_H
#define myAES_H

std::string ECB_AESEncryptStr(std::string sKey, const char *plainText);
std::string ECB_AESDecryptStr(std::string sKey, const char *cipherText);
#endif
