#ifndef BF4F4979_7F92_4792_BAFB_EED69FEFA96C
#define BF4F4979_7F92_4792_BAFB_EED69FEFA96C

#include <iostream>
#include <memory>

#include "openssl/evp.h"
#include "openssl/ecdsa.h"
#include "openssl/bio.h"
#include "openssl/pem.h"
#include "openssl/ec.h"

#include "../hash/sha_hash.h"

#define DEFAULT_PUBLIC_KEY_PEM_PATH  "../.config/ecdsa_public_key.pem"
#define DEFAULT_PRIVATE_KEY_PEM_PATH  "../.config/ecdsa_private_key.pem"


namespace cipher{




class ECDSAManager{

private:
	EVP_PKEY *_pkey;

	static unsigned char* _pemPass;
	static unsigned int _pemPassSize;

	void error(  const char* errorMSG , bool isExit = true );

protected:
	bool writePubKey( EVP_PKEY *pkey , const char *path = (const char *)(DEFAULT_PUBLIC_KEY_PEM_PATH) );
	bool writePriKey(  EVP_PKEY *pkey , const char *path = (const char *)(DEFAULT_PRIVATE_KEY_PEM_PATH) , unsigned char* pemPass = _pemPass , unsigned int passSize = _pemPassSize );

	EVP_PKEY *readPubKey( const char *path = (const char *)(DEFAULT_PUBLIC_KEY_PEM_PATH) );
	EVP_PKEY *readPriKey( unsigned char *pemPass  = _pemPass , const char *path = (const char *)(DEFAULT_PRIVATE_KEY_PEM_PATH) );

	

public:
	static EVP_PKEY *_myPkey;
	EVP_PKEY* myPkey(); // getter

	void setPemPass( unsigned char* pemPass , unsigned int passSize ); // setter

	static int RawPubKey();
	bool init( unsigned char* pemPass, unsigned int passSize ); // 自身のkeypairファイルを取得&セット　なければ許可を求めて -> 新規作成


	EVP_PKEY* generatePkey();
	//bool saveNewKeyPair( unsigned char* pemPass , unsigned int passSize , const char *pubPath , const char *priPath );
	bool saveNewPkey( unsigned char* pemPass , unsigned int passSize );


	static void printPkey( EVP_PKEY *pkey );
	static int toRawPubKey( unsigned char **ret , EVP_PKEY* pkey );
	static unsigned int toRawPubKey( EVP_PKEY *pkey , std::shared_ptr<unsigned char> *ret );

	static EVP_PKEY* toPkey( unsigned char* rawPubKey , unsigned int rawPubKeySize );
	//static EVP_PKEY* toPKEY( std::shared_ptr<unsigned char> from , unsigned int fromLength );


	static int Sign( unsigned char *target, size_t targetLen , unsigned char** sig, EVP_PKEY* pri_key = ECDSAManager::_myPkey );
	static bool Verify( unsigned char *sig, size_t sigLen , unsigned char *target, size_t targetLen, EVP_PKEY* pub_key = ECDSAManager::_myPkey );

	static unsigned int sign( std::shared_ptr<unsigned char> target , unsigned int targetLength, EVP_PKEY *pkey , std::shared_ptr<unsigned char> *retSign );
	static bool verify( std::shared_ptr<unsigned char> sig , size_t sigLength , std::shared_ptr<unsigned char> msg , size_t msgLength , EVP_PKEY *pubKey , char *hashType ); 
	static bool verify( std::shared_ptr<unsigned char> sig , size_t sigLength , std::shared_ptr<unsigned char> msgDigest , size_t msgDigestLength , EVP_PKEY *pubKey );

	bool writePkey( const char *path , EVP_PKEY *pkey );
	EVP_PKEY* readPkey( const char *path );
};



}; // close cipher namespace

#endif // BF4F4979_7F92_4792_BAFB_EED69FEFA96C

