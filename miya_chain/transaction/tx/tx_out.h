#ifndef C118AEA6_943A_490A_BD1D_4FC20E026EBE
#define C118AEA6_943A_490A_BD1D_4FC20E026EBE



#include "openssl/evp.h"
#include <stdlib.h>
#include <string>
#include <map>
#include <algorithm>
#include <iostream>


#include "../script/signature_script.h"
#include "../script/pk_script.h"
#include "../script/script.h"

#include "../../../shared_components/json.hpp"
using json = nlohmann::json;


namespace tx{

class PkScript;








struct TxOut
{
private:

	struct  Body
	{
		int64_t _value = 0; // 8 bytes
		uint64_t _pkScriptBytes; // lockingScriptのバイト長
		std::shared_ptr<PkScript> _pkScript;  // lockingScript
	} _body;


	std::shared_ptr<unsigned char> _pubKeyHash = nullptr;
	void hello(){ std::cout << "Hello TxOut"; };
public:
	TxOut();

	void init( unsigned int value , std::shared_ptr<unsigned char> pubKeyHash );

	std::shared_ptr<PkScript> pkScript(){ return _body._pkScript; }; // テスト用getter 後に削除する

	unsigned short exportRaw( std::shared_ptr<unsigned char> *retRaw );
	std::shared_ptr<unsigned char> pubKeyHash();
	void pubKeyHash( std::shared_ptr<unsigned char> pubKeyHash );

	int importRaw( unsigned char *fromRaw );


	unsigned short value();
	void value( unsigned int value );
	unsigned int pkScriptBytes();

	friend void to_json( json& from , const TxOut &to );
	friend void from_json( const json &from , TxOut &to );


};

};


#endif // C118AEA6_943A_490A_BD1D_4FC20E026EBE
