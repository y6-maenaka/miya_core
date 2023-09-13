#ifndef C118AEA6_943A_490A_BD1D_4FC20E026EBE
#define C118AEA6_943A_490A_BD1D_4FC20E026EBE



#include "openssl/evp.h"
#include <stdlib.h>
#include <string.h>

#include <map>

namespace tx{

class PkScript;




/*
struct TxOut{

	int64_t _value;
	uint32_t _pkScriptBytes;
	PkScript *_pkScript;
	//void* pk_script;

	unsigned int exportRaw( unsigned char **ret );
	unsigned int exportRawSize();

	TxOut();
	TxOut( unsigned int value );


	unsigned int autoTakeInPkScript( unsigned char* from ); // 取り込んだバイト分リターンする
	unsigned int pkScriptBytes();

} __attribute__((__packed__));
*/







struct TxOut
{
private:

	struct 
	{
		int64_t _value;
		uint64_t _pkScriptBytes; // lockingScriptのバイト長
		std::shared_ptr<PkScript> _pkScript; 
		
	} _body;

	std::shared_ptr<unsigned char> _pubKeyHash = nullptr;

public:
	unsigned short exportRaw( std::shared_ptr<unsigned char> retRaw );
	void pubKeyHash( std::shared_ptr<unsigned char> pubKeyHash );
};






};


#endif // C118AEA6_943A_490A_BD1D_4FC20E026EBE
