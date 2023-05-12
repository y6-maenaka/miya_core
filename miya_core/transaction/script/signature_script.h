#ifndef BC1FA169_C231_4488_BB6B_A681BFDA6156
#define BC1FA169_C231_4488_BB6B_A681BFDA6156




#include "./script.h"
#include "openssl/evp.h"


namespace tx{




enum class SCRIPT_TYPE : int {

	HashedPubKey = 1,
	Sig	,

};




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





}; // close tx namespace


#endif // BC1FA169_C231_4488_BB6B_A681BFDA6156

