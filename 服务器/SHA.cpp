#include <iostream>
  #include <string.h>
  
  #include "sha.h"
  #include "secblock.h"
  #include "modes.h"
  #include "hex.h"
#include"mySHA.h"
 #pragma comment( lib, "cryptlib.lib")
 
 using namespace std;
 using namespace CryptoPP;


 int my_SHA( )
 {
     //main������ע�͵��ģ�����strMessage2�Ĵ��룬��ʵ�Ǳ���ģ����һ��
     //ͨ����Hashֵ���ԡ��������ݽ���У���������ܵ����á�
     //ע��֮�󲢲�Ӱ����δ������˼������̡�
    string strMessage( "Hello world" );
   string strDigest;
     //string strMessage2( "hello world" ); //ֻ�ǵ�һ����ĸ��ͬ
     //string strDigest2;

    CalculateDigest( strDigest, strMessage );  //����Hashֵ����ӡһЩdebug��Ϣ
   cout << "the size of Digest is: " << strDigest.size() << endl;
    cout << "Digest is: " << strDigest << endl;

    //CalculateDigest( strDigest2, strMessage2 );
    //why put this function here will affect the Verify function?
     //������д����Ĺ������������������⡣
     //��������д����ע��ȡ������ô֮������н���Ͳ���Ԥ���е�һ����
    //��ʹstrDigestҲ�޷���ӦstrMessage�����߲�֪��Ϊʲô��ϣ������ָ����лл��
 
    bool bIsSuccess = false;
   bIsSuccess = VerifyDigest( strDigest, strMessage ); 
    //ͨ��У�飬����strDigest�Ƿ��Ӧԭ����message
    if( bIsSuccess )
    {
        cout << "sussessive verify" << endl;
        cout << "origin string is: " << strMessage << endl << endl;
     }
    else
    {
        cout << "fail!" << endl;
    }
 
   //ͨ��strDigest2��strMessage����У�飬Ҫ����ȣ�
    //��֤��strDigest2�Ƕ�Ӧ��strMessage2��strMessage1��ȡ�
    //��������������е�����һ��������message�ǲ���ȵ�
     /*CalculateDigest( strDigest2, strMessage2 );
 55    bIsSuccess = VerifyDigest( strDigest2, strMessage );
 56    if( !bIsSuccess )
 57    {
 58        cout << "success! the tiny modification is discovered~" << endl;
 59        cout << "the origin message is: \n" << strMessage << endl;
 60        cout << "after modify is: \n" << strMessage2 << endl;
 61    }*/
    return 0;
}


//����ĳЩԭ�����������Ӻ�����ʵ�ָ�ԭ���ο������е�ʵ����������,
//��ϸԭ�򣬱�����CalculateDigest������ע����д��
void CalculateDigest(string &Digest, const string &Message)
{
    SHA256 sha256;
    int DigestSize = sha256.DigestSize();
   char* byDigest;
     char* strDigest;
 
    byDigest = new char[ DigestSize ];
    strDigest = new char[ DigestSize * 2 + 1 ];

    sha256.CalculateDigest((byte*)byDigest, (const byte *)Message.c_str(), Message.size());
    memset(strDigest, 0, sizeof(strDigest));
    //uCharToHex(strDigest, byDigest, DigestSize);
    //�ο��Ĵ�������������ôһ�У�����ò�Ʋ���ʲô�⺯����
    //ԭ���ߴ�������Hashֵת����16���������浽һ��string buffer�У�
    //Ȼ���������������������debug��ʱ����ղ鿴��
     //�����Ⲣ��Ӱ�����Hashֵ����Ϊ��
     //��˱���ע�͵������д��룬�����޸���һ����������ͺ����VerifyDigest������
     //��ȥԭ�����ⲿ�ֵ���ͼ���������ǵĳ���ִ�С�
 
     Digest = byDigest;
 
     delete []byDigest;
     byDigest = NULL;
     delete []strDigest;
     strDigest = NULL;
 
     return;
 }
 
 bool VerifyDigest(const string &Digest, const string &Message)
 {
    bool Result;
    SHA256 sha256;
    char* byDigest;

    byDigest = new char[ sha256.DigestSize() ];
    strcpy_s( byDigest, sizeof(Digest.c_str()), Digest.c_str());

    //HexTouChar(byDigest, Digest.c_str(), Digest.size());
    //Ϊ��ע�͵�����ο�CalculateDigest������ע��
    Result = sha256.VerifyDigest( (byte*)byDigest, (const byte *)Message.c_str(), Message.size() );

    delete []byDigest;
    byDigest = NULL;
    return Result;
}