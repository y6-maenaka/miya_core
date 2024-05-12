#ifndef BC1FA169_C231_4488_BB6B_A681BFDA6156
#define BC1FA169_C231_4488_BB6B_A681BFDA6156


#include <memory>
#include <variant>
#include <iostream>
#include <unistd.h>
#include <map>

#include "./script.h"
#include "openssl/evp.h"

#include "../../../share/cipher/ecdsa_manager.h"
#include "../../../share/hash/sha_hash.h"
#include "openssl/asn1.h"


namespace tx{


enum class SCRIPT_TYPE : int {

	HashedPubKey = 1,
	Sig	,

};


class SignatureScript // unLockingScript // 秘密鍵を持っていることの証明
{
//private:
public:
	int _scriptType; // pubkeyHash , signature
	unsigned int _scriptLength; // 実用上は必要ない, 確認用

	struct
	{
		std::shared_ptr<unsigned char> _sign;
		unsigned int _signLength;
		bool _isSigned = false;
	} _signature;


	EVP_PKEY *_pkey = nullptr; // with公開鍵書き出し,署名書き出しで必要 (必須)
	std::shared_ptr<Script> _script; // unlockingScript読み書きだし用


//public:
	std::shared_ptr<Script> script(){ return _script; };
	SignatureScript();
	void pkey( EVP_PKEY *pkey );
	EVP_PKEY *pkey();
	void sign( std::shared_ptr<unsigned char> sign, unsigned int signLength , bool isSigned = false );

	bool isSigned();
	// void toPubKeyHash( EVP_PKEY *pkey );
	// void toP2PKHSignatureScript( std::shared_ptr<unsigned char> sign );
	unsigned int importRaw( unsigned char* fromRaw , unsigned int fromRawLength );
	
	unsigned short exportRawWithPubKeyHash( std::shared_ptr<unsigned char> *ret ); // 署名用に公開鍵ハッシュをセットして書き出す
	unsigned short exportRawWithSignatureScript( std::shared_ptr<unsigned char> *ret ); //  署名が格納されたものを書き出す(本命)
	// unsigned short exportRaw( std::shared_ptr<unsigned char> ret );
};


}; // close tx namespace


#endif // BC1FA169_C231_4488_BB6B_A681BFDA6156

