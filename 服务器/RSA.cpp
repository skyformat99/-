// test.cpp - written and placed in the public domain by Wei Dai

#define CRYPTOPP_DEFAULT_NO_DLL
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include "dll.h"
#include "aes.h"
#include "cryptlib.h"
#include "filters.h"
#include "md5.h"
#include "ripemd.h"
#include "rng.h"
#include "gzip.h"
#include "default.h"
#include "randpool.h"
#include "ida.h"
#include "base64.h"
#include "socketft.h"
#include "wait.h"
#include "factory.h"
#include "whrlpool.h"
#include "tiger.h"
#include "smartptr.h"
#include "ossig.h"
#include "trap.h"

#include "validate.h"
#include "bench.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <locale>
#include <time.h>

#ifdef CRYPTOPP_WIN32_AVAILABLE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if defined(USE_BERKELEY_STYLE_SOCKETS) && !defined(macintosh)
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

#if (_MSC_VER >= 1000)
#include <crtdbg.h>		// for the debug heap
#endif

#if defined(__MWERKS__) && defined(macintosh)
#include <console.h>
#endif

#ifdef _OPENMP
# include <omp.h>
#endif

#ifdef __BORLANDC__
#pragma comment(lib, "cryptlib_bds.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

// Aggressive stack checking with VS2005 SP1 and above.
#if (CRYPTOPP_MSC_VERSION >= 1410)
# pragma strict_gs_check (on)
#endif

#if CRYPTOPP_GCC_DIAGNOSTIC_AVAILABLE
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

USING_NAMESPACE(CryptoPP)
USING_NAMESPACE(std)

const int MAX_PHRASE_LENGTH = 250;
void RegisterFactories();

void GenerateRSAKey(unsigned int keyLength, const char *privFilename, const char *pubFilename, const char *seed); //…˙≥…√‹‘ø
string RSAEncryptString(const char *pubFilename, const char *seed, const char *message);
string RSADecryptString(const char *privFilename, const char *ciphertext);
void RSASignFile(const char *privFilename, const char *messageFilename, const char *signatureFilename);
bool RSAVerifyFile(const char *pubFilename, const char *messageFilename, const char *signatureFilename);



namespace { OFB_Mode<AES>::Encryption s_globalRNG; }
RandomNumberGenerator & GlobalRNG()
{
	return dynamic_cast<RandomNumberGenerator&>(s_globalRNG);
}

// See misc.h and trap.h for comments and usage
#if CRYPTOPP_DEBUG && (defined(CRYPTOPP_BSD_AVAILABLE) || defined(CRYPTOPP_UNIX_AVAILABLE))
static const SignalHandler<SIGTRAP, false> s_dummyHandler;
// static const DebugTrapHandler s_dummyHandler;
#endif

int CRYPTOPP_API CroptoPP(int argc, char *argv[])
{
#ifdef _CRTDBG_LEAK_CHECK_DF
	// Turn on leak-checking
	int tempflag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tempflag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tempflag);
#endif

#if defined(__MWERKS__) && defined(macintosh)
	argc = ccommand(&argv);
#endif
	try
	{
		RegisterFactories();

		// Some editors have problems with the '\0' character when redirecting output.
		std::string seed = IntToString(time(NULL));
		seed.resize(16, ' ');

		OFB_Mode<AES>::Encryption& prng = dynamic_cast<OFB_Mode<AES>::Encryption&>(GlobalRNG());
		prng.SetKeyWithIV((byte *)seed.data(), 16, (byte *)seed.data());

		std::string command, executableName, macFilename;

		if (argc < 2)
			command = 'h';
		else
			command = argv[1];

		if (command == "g")
		{
			char thisSeed[1024], privFilename[128], pubFilename[128];
			unsigned int keyLength;

			cout << "Key length in bits: ";
			cin >> keyLength;

			cout << "\nSave private key to file: ";
			cin >> privFilename;

			cout << "\nSave public key to file: ";
			cin >> pubFilename;

			cout << "\nRandom Seed: ";
			ws(cin);
			cin.getline(thisSeed, 1024);

			GenerateRSAKey(keyLength, privFilename, pubFilename, thisSeed);
		}
		else if (command == "rs")
			RSASignFile(argv[2], argv[3], argv[4]);
		else if (command == "rv")
		{
			bool verified = RSAVerifyFile(argv[2], argv[3], argv[4]);
			cout << (verified ? "valid signature" : "invalid signature") << endl;
		}
		else if (command == "r")
		{
			char privFilename[128], pubFilename[128];
			char thisSeed[1024], message[1024];

			cout << "Private key file: ";
			cin >> privFilename;

			cout << "\nPublic key file: ";
			cin >> pubFilename;

			cout << "\nRandom Seed: ";
			ws(cin);
			cin.getline(thisSeed, 1024);

			cout << "\nMessage: ";
			cin.getline(message, 1024);

			string ciphertext = RSAEncryptString(pubFilename, thisSeed, message);
			cout << "\nCiphertext: " << ciphertext << endl;

			string decrypted = RSADecryptString(privFilename, ciphertext.c_str());
			cout << "\nDecrypted: " << decrypted << endl;
		}
	}
		catch (const CryptoPP::Exception &e)
		{
			cout << "\nCryptoPP::Exception caught: " << e.what() << endl;
			return -1;
		}
		catch (const std::exception &e)
		{
			cout << "\nstd::exception caught: " << e.what() << endl;
			return -2;
		}
	}
void GenerateRSAKey(unsigned int keyLength, const char *privFilename, const char *pubFilename, const char *seed)
{
	RandomPool randPool;
	randPool.IncorporateEntropy((byte *)seed, strlen(seed));

	RSAES_OAEP_SHA_Decryptor priv(randPool, keyLength);
	HexEncoder privFile(new FileSink(privFilename));
	priv.DEREncode(privFile);
	privFile.MessageEnd();

	RSAES_OAEP_SHA_Encryptor pub(priv);
	HexEncoder pubFile(new FileSink(pubFilename));
	pub.DEREncode(pubFile);
	pubFile.MessageEnd();
}

string RSAEncryptString(const char *pubFilename, const char *seed, const char *message)
{
	FileSource pubFile(pubFilename, true, new HexDecoder);
	RSAES_OAEP_SHA_Encryptor pub(pubFile);

	RandomPool randPool;
	randPool.IncorporateEntropy((byte *)seed, strlen(seed));

	string result;
	StringSource(message, true, new PK_EncryptorFilter(randPool, pub, new HexEncoder(new StringSink(result))));
	return result;
}

string RSADecryptString(const char *privFilename, const char *ciphertext)
{
	FileSource privFile(privFilename, true, new HexDecoder);
	RSAES_OAEP_SHA_Decryptor priv(privFile);

	string result;
	StringSource(ciphertext, true, new HexDecoder(new PK_DecryptorFilter(GlobalRNG(), priv, new StringSink(result))));
	return result;
}

void RSASignFile(const char *privFilename, const char *messageFilename, const char *signatureFilename)
{
	FileSource privFile(privFilename, true, new HexDecoder);
	RSASS<PKCS1v15, SHA>::Signer priv(privFile);
	FileSource f(messageFilename, true, new SignerFilter(GlobalRNG(), priv, new HexEncoder(new FileSink(signatureFilename))));
}

bool RSAVerifyFile(const char *pubFilename, const char *messageFilename, const char *signatureFilename)
{
	FileSource pubFile(pubFilename, true, new HexDecoder);
	RSASS<PKCS1v15, SHA>::Verifier pub(pubFile);

	FileSource signatureFile(signatureFilename, true, new HexDecoder);
	if (signatureFile.MaxRetrievable() != pub.SignatureLength())
		return false;
	SecByteBlock signature(pub.SignatureLength());
	signatureFile.Get(signature, signature.size());

	VerifierFilter *verifierFilter = new VerifierFilter(pub);
	verifierFilter->Put(signature, pub.SignatureLength());
	FileSource f(messageFilename, true, verifierFilter);

	return verifierFilter->GetLastResult();
}

