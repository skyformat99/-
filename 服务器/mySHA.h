
#include<string>
#ifndef mySHA_H
#define mySHA_H
using namespace std;
void CalculateDigest(string &Digest, const string &Message);
bool VerifyDigest(const string &Digest, const string &Message);


#endif

