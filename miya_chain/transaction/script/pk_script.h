#ifndef A464F000_6B41_4360_B39C_A3EA44A8E80B
#define A464F000_6B41_4360_B39C_A3EA44A8E80B


#include "./script.h"

#include <iostream>
#include <vector>
#include <map>
#include <variant>

#include "openssl/evp.h"


namespace tx{





/* OPECODE */

/*
constexpr char OP_DUP = 0x4c;          // 76
constexpr char OP_HASH_160 = 0xa9;     // 169
constexpr char OP_EQUALVERIFY = 0x88;  // 136
constexpr char OP_CHECKSIG = 0xac;     // 172
*/






/*
class PkScript : public Script { // lockingScript

private:

public:

	PkScript *createP2PKHScript( EVP_PKEY *destPubKey  = NULL );
	unsigned int exportRawWithP2PKHScript( std::shared_ptr<unsigned char> destAddress );

};
*/






class PkScript 
{
private:
	std::shared_ptr<Script> _script;

public:
	unsigned int exportRawWithP2PKHPkScript( std::shared_ptr<unsigned char> retRaw , std::shared_ptr<unsigned char> pubKeyHash );

	unsigned int importRaw( unsigned char *fromRaw, unsigned int fromRawLength );
};












}



#endif // A464F000_6B41_4360_B39C_A3EA44A8E80B
