#ifndef BC1FA169_C231_4488_BB6B_A681BFDA6156
#define BC1FA169_C231_4488_BB6B_A681BFDA6156




#include "./script.h"
#include "openssl/evp.h"


namespace tx{




enum class SCRIPT_TYPE : int {

	HashedPubKey = 1,
	Sig	,

};



/*
class SignatureScript : public Script {

private:
	int _scriptType;

public:
	SignatureScript() : _scriptType(0) {};
	
	int scriptType(); // getter
	
	EVP_PKEY *pubKey();
	unsigned int sig( unsigned char **sig );

	// 署名関係
	SignatureScript *createSigScript( unsigned char* sig , unsigned int sigSize , EVP_PKEY *pkey );
	SignatureScript *createHashedPubKeyScript( EVP_PKEY *pkey ); 
};
*/




class SignatureScript
{
private:
	int _scriptType; // pubkeyHash , signature
	unsigned int _scriptLength; // 実用上は必要ない, 確認用

	EVP_PKEY *_pkey; // pubKeyHash用

public:
	void toPubKeyHash( EVP_PKEY *pkey );
	void toP2PKHSignatureScript();

	unsigned short exportRawWithPubKeyHash( std::shared_ptr<unsigned char> ret );
	// unsigned short exportRaw( std::shared_ptr<unsigned char> ret );
};





}; // close tx namespace


#endif // BC1FA169_C231_4488_BB6B_A681BFDA6156

