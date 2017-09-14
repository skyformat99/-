//#include "stdafx.h"  
#include <aes.h>  
#include <Hex.h>      // StreamTransformationFilter  
#include <modes.h>    // CFB_Mode  
#include <iostream>   // std:cerr    
#include <sstream>   // std::stringstream    
#include <string>  

#include"myAES.h"
using namespace std;
using namespace CryptoPP;
#pragma comment( lib, "cryptlib.lib" )  

std::string ECB_AESEncryptStr(std::string sKey, const char *plainText)
{
	std::string outstr;

	//填key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);


	AES::Encryption aesEncryption((byte *)key, AES::MAX_KEYLENGTH);

	ECB_Mode_ExternalCipher::Encryption ecbEncryption(aesEncryption);
	StreamTransformationFilter ecbEncryptor(ecbEncryption, new HexEncoder(new StringSink(outstr)));
	ecbEncryptor.Put((byte *)plainText, strlen(plainText));
	ecbEncryptor.MessageEnd();

	return outstr;
}



std::string ECB_AESDecryptStr(std::string sKey, const char *cipherText)
{
	std::string outstr;

	//填key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	ECB_Mode<AES >::Decryption ecbDecryption((byte *)key, AES::MAX_KEYLENGTH);

	HexDecoder decryptor(new StreamTransformationFilter(ecbDecryption, new StringSink(outstr)));
	decryptor.Put((byte *)cipherText, strlen(cipherText));
	decryptor.MessageEnd();

	return outstr;
}



std::string CBC_AESEncryptStr(std::string sKey, std::string sIV, const char *plainText)
{
	std::string outstr;

	//填key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	//填iv    
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x30, AES::BLOCKSIZE);
	sIV.size() <= AES::BLOCKSIZE ? memcpy(iv, sIV.c_str(), sIV.size()) : memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);

	AES::Encryption aesEncryption((byte *)key, AES::MAX_KEYLENGTH);

	CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

	StreamTransformationFilter cbcEncryptor(cbcEncryption, new HexEncoder(new StringSink(outstr)));
	cbcEncryptor.Put((byte *)plainText, strlen(plainText));
	cbcEncryptor.MessageEnd();

	return outstr;
}

std::string CBC_AESDecryptStr(std::string sKey, std::string sIV, const char *cipherText)
{
	std::string outstr;

	//填key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	//填iv    
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x30, AES::BLOCKSIZE);
	sIV.size() <= AES::BLOCKSIZE ? memcpy(iv, sIV.c_str(), sIV.size()) : memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);


	CBC_Mode<AES >::Decryption cbcDecryption((byte *)key, AES::MAX_KEYLENGTH, iv);

	HexDecoder decryptor(new StreamTransformationFilter(cbcDecryption, new StringSink(outstr)));
	decryptor.Put((byte *)cipherText, strlen(cipherText));
	decryptor.MessageEnd();

	return outstr;
}

std::string CBC_CTS_AESEncryptStr(std::string sKey, std::string sIV, const char *plainText)
{
	std::string outstr;

	//填key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	//填iv    
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x30, AES::BLOCKSIZE);
	sIV.size() <= AES::BLOCKSIZE ? memcpy(iv, sIV.c_str(), sIV.size()) : memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);

	AES::Encryption aesEncryption((byte *)key, AES::MAX_KEYLENGTH);

	CBC_CTS_Mode_ExternalCipher::Encryption cbcctsEncryption(aesEncryption, iv);

	StreamTransformationFilter cbcctsEncryptor(cbcctsEncryption, new HexEncoder(new StringSink(outstr)));
	cbcctsEncryptor.Put((byte *)plainText, strlen(plainText));
	cbcctsEncryptor.MessageEnd();

	return outstr;
}




std::string CBC_CTS_AESDecryptStr(std::string sKey, std::string sIV, const char *cipherText)
{
	std::string outstr;

	//填key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	//填iv    
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x30, AES::BLOCKSIZE);
	sIV.size() <= AES::BLOCKSIZE ? memcpy(iv, sIV.c_str(), sIV.size()) : memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);


	CBC_CTS_Mode<AES >::Decryption cbcctsDecryption((byte *)key, AES::MAX_KEYLENGTH, iv);

	HexDecoder decryptor(new StreamTransformationFilter(cbcctsDecryption, new StringSink(outstr)));
	decryptor.Put((byte *)cipherText, strlen(cipherText));
	decryptor.MessageEnd();

	return outstr;
}










std::string CFB_AESEncryptStr(std::string sKey, std::string sIV, const char *plainText)
{
	std::string outstr;

	//填key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	//填iv    
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x30, AES::BLOCKSIZE);
	sIV.size() <= AES::BLOCKSIZE ? memcpy(iv, sIV.c_str(), sIV.size()) : memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);

	AES::Encryption aesEncryption((byte *)key, AES::MAX_KEYLENGTH);

	CFB_Mode_ExternalCipher::Encryption cfbEncryption(aesEncryption, iv);

	StreamTransformationFilter cfbEncryptor(cfbEncryption, new HexEncoder(new StringSink(outstr)));
	cfbEncryptor.Put((byte *)plainText, strlen(plainText));
	cfbEncryptor.MessageEnd();

	return outstr;
}




std::string CFB_AESDecryptStr(std::string sKey, std::string sIV, const char *cipherText)
{
	std::string outstr;

	//填key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	//填iv    
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x30, AES::BLOCKSIZE);
	sIV.size() <= AES::BLOCKSIZE ? memcpy(iv, sIV.c_str(), sIV.size()) : memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);


	CFB_Mode<AES >::Decryption cfbDecryption((byte *)key, AES::MAX_KEYLENGTH, iv);

	HexDecoder decryptor(new StreamTransformationFilter(cfbDecryption, new StringSink(outstr)));
	decryptor.Put((byte *)cipherText, strlen(cipherText));
	decryptor.MessageEnd();

	return outstr;
}




std::string OFB_AESEncryptStr(std::string sKey, std::string sIV, const char *plainText)
{
	std::string outstr;

	//填key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	//填iv    
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x30, AES::BLOCKSIZE);
	sIV.size() <= AES::BLOCKSIZE ? memcpy(iv, sIV.c_str(), sIV.size()) : memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);

	AES::Encryption aesEncryption((byte *)key, AES::MAX_KEYLENGTH);

	OFB_Mode_ExternalCipher::Encryption ofbEncryption(aesEncryption, iv);

	StreamTransformationFilter ofbEncryptor(ofbEncryption, new HexEncoder(new StringSink(outstr)));
	ofbEncryptor.Put((byte *)plainText, strlen(plainText));
	ofbEncryptor.MessageEnd();

	return outstr;
}




std::string OFB_AESDecryptStr(std::string sKey, std::string sIV, const char *cipherText)
{
	std::string outstr;

	//填key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	//填iv    
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x30, AES::BLOCKSIZE);
	sIV.size() <= AES::BLOCKSIZE ? memcpy(iv, sIV.c_str(), sIV.size()) : memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);


	OFB_Mode<AES >::Decryption ofbDecryption((byte *)key, AES::MAX_KEYLENGTH, iv);

	HexDecoder decryptor(new StreamTransformationFilter(ofbDecryption, new StringSink(outstr)));
	decryptor.Put((byte *)cipherText, strlen(cipherText));
	decryptor.MessageEnd();

	return outstr;
}


std::string CTR_AESEncryptStr(std::string sKey, std::string sIV, const char *plainText)
{
	std::string outstr;

	//填key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	//填iv    
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x30, AES::BLOCKSIZE);
	sIV.size() <= AES::BLOCKSIZE ? memcpy(iv, sIV.c_str(), sIV.size()) : memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);

	AES::Encryption aesEncryption((byte *)key, AES::MAX_KEYLENGTH);

	CTR_Mode_ExternalCipher::Encryption ctrEncryption(aesEncryption, iv);

	StreamTransformationFilter ctrEncryptor(ctrEncryption, new HexEncoder(new StringSink(outstr)));
	ctrEncryptor.Put((byte *)plainText, strlen(plainText));
	ctrEncryptor.MessageEnd();

	return outstr;
}




std::string CTR_AESDecryptStr(std::string sKey, std::string sIV, const char *cipherText)
{
	std::string outstr;

	//填key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	//填iv    
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x30, AES::BLOCKSIZE);
	sIV.size() <= AES::BLOCKSIZE ? memcpy(iv, sIV.c_str(), sIV.size()) : memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);


	CTR_Mode<AES >::Decryption ctrDecryption((byte *)key, AES::MAX_KEYLENGTH, iv);

	HexDecoder decryptor(new StreamTransformationFilter(ctrDecryption, new StringSink(outstr)));
	decryptor.Put((byte *)cipherText, strlen(cipherText));
	decryptor.MessageEnd();

	return outstr;
}


int myAES(int argc, char * argv[])
{
	string plainText = "This Program shows how to use ECB, CBC, CBC_CTS, CFB, OFB and CTR mode of AES in Crypto++.";
	string aesKey = "0123456789ABCDEF0123456789ABCDEF";//256bits, also can be 128 bits or 192bits  
	string aesIV = "ABCDEF0123456789";//128 bits  
	string ECB_EncryptedText, ECB_DecryptedText,
		CBC_EncryptedText, CBC_DecryptedText,
		CBC_CTS_EncryptedText, CBC_CTS_DecryptedText,
		CFB_EncryptedText, CFB_DecryptedText,
		OFB_EncryptedText, OFB_DecryptedText,
		CTR_EncryptedText, CTR_DecryptedText;


	//ECB  
	ECB_EncryptedText = ECB_AESEncryptStr(aesKey, plainText.c_str());//ECB加密  
	ECB_DecryptedText = ECB_AESDecryptStr(aesKey, ECB_EncryptedText.c_str());//ECB解密  


																			 //CBC  
	CBC_EncryptedText = CBC_AESEncryptStr(aesKey, aesIV, plainText.c_str());//CBC加密  
	CBC_DecryptedText = CBC_AESDecryptStr(aesKey, aesIV, CBC_EncryptedText.c_str());//CBC解密  


																					//CBC_CTS  
	CBC_CTS_EncryptedText = CBC_CTS_AESEncryptStr(aesKey, aesIV, plainText.c_str());//CBC_CTS加密  
	CBC_CTS_DecryptedText = CBC_CTS_AESDecryptStr(aesKey, aesIV, CBC_CTS_EncryptedText.c_str());//CBC_CTS解密  

																								//CFB  
	CFB_EncryptedText = CFB_AESEncryptStr(aesKey, aesIV, plainText.c_str());//CFB加密  
	CFB_DecryptedText = CFB_AESDecryptStr(aesKey, aesIV, CFB_EncryptedText.c_str());//CFB解密  


																					//OFB  
	OFB_EncryptedText = OFB_AESEncryptStr(aesKey, aesIV, plainText.c_str());//OFB加密  
	OFB_DecryptedText = OFB_AESDecryptStr(aesKey, aesIV, OFB_EncryptedText.c_str());//OFB解密  


																					//CTR  
	CTR_EncryptedText = CTR_AESEncryptStr(aesKey, aesIV, plainText.c_str());//CTR加密  
	CTR_DecryptedText = CTR_AESDecryptStr(aesKey, aesIV, CTR_EncryptedText.c_str());//CTR解密  




	cout << "Crypto++ AES-256 加密测试" << endl;
	cout << "分别使用ECB，CBC, CBC_CTR，CFB，OFB和CTR模式" << endl;
	cout << "加密用密钥:" << aesKey << endl;
	cout << "密钥长度:" << AES::MAX_KEYLENGTH * 8 << "bits" << endl;
	cout << "IV:" << aesIV << endl;
	cout << endl;


	cout << "ECB测试" << endl;
	cout << "原文：" << plainText << endl;
	cout << "密文：" << ECB_EncryptedText << endl;
	cout << "恢复明文：" << ECB_DecryptedText << endl << endl;

	cout << "CBC测试" << endl;
	cout << "原文：" << plainText << endl;
	cout << "密文：" << CBC_EncryptedText << endl;
	cout << "恢复明文：" << CBC_DecryptedText << endl << endl;


	cout << "CBC_CTS测试" << endl;
	cout << "原文：" << plainText << endl;
	cout << "密文：" << CBC_CTS_EncryptedText << endl;
	cout << "恢复明文：" << CBC_CTS_DecryptedText << endl << endl;


	cout << "CFB测试" << endl;
	cout << "原文：" << plainText << endl;
	cout << "密文：" << CFB_EncryptedText << endl;
	cout << "恢复明文：" << CFB_DecryptedText << endl << endl;

	cout << "OFB测试" << endl;
	cout << "原文：" << plainText << endl;
	cout << "密文：" << OFB_EncryptedText << endl;
	cout << "恢复明文：" << OFB_DecryptedText << endl << endl;


	cout << "CTR测试" << endl;
	cout << "原文：" << plainText << endl;
	cout << "密文：" << CTR_EncryptedText << endl;
	cout << "恢复明文：" << CTR_DecryptedText << endl << endl;

	getchar();
	return 0;
}