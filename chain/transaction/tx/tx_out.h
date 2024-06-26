#ifndef C118AEA6_943A_490A_BD1D_4FC20E026EBE
#define C118AEA6_943A_490A_BD1D_4FC20E026EBE


#include "openssl/evp.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <algorithm>
#include <iostream>
#include <vector>

#ifdef __linux__
	#include <endian.h>
#endif
#ifdef __APPLE__
	#include <machine/endian.h>
#endif


#include "../script/signature_script.h"
#include "../script/pk_script.h"
#include "../script/script.h"

#include "../../../share/json.hpp"
using json = nlohmann::json;


namespace tx{

class PkScript;


struct TxOut
{
//private:
public:
	struct Meta
	{
		std::int64_t _value = 0; // 8 bytes
		std::uint64_t _pkScriptBytes; // lockingScriptのバイト長
		std::shared_ptr<PkScript> _pkScript;  // lockingScript
	} _meta;

	std::shared_ptr<unsigned char> _pubKeyHash = nullptr;
	void hello(){ std::cout << "Hello TxOut"; };

public:
	TxOut();

	void init( unsigned int value , std::shared_ptr<unsigned char> pubKeyHash );

	std::shared_ptr<PkScript> pkScript(){ return _meta._pkScript; }; // テスト用getter 後に削除する

	unsigned short exportRaw( std::shared_ptr<unsigned char> *retRaw );
	std::shared_ptr<unsigned char> pubKeyHash();
	void pubKeyHash( std::shared_ptr<unsigned char> pubKeyHash );

	int importRaw( unsigned char *fromRaw );


	unsigned short value();
	void value( unsigned int value );
	unsigned int pkScriptBytes() const;

	friend void to_json( json& from , const TxOut &to );
	friend void from_json( const json &from , TxOut &to );
  
	std::vector<std::uint8_t> export_to_binary();//const;
};


};


#endif // C118AEA6_943A_490A_BD1D_4FC20E026EBE
