#ifndef A464F000_6B41_4360_B39C_A3EA44A8E80B
#define A464F000_6B41_4360_B39C_A3EA44A8E80B


#include "./script.h"

#include <iostream>
#include <vector>
#include <map>
#include <variant>

#include "openssl/evp.h"


namespace tx{







class PkScript 
{
//private:
public:
	std::shared_ptr<Script> _script;

public:
	PkScript();

	unsigned int exportRawWithP2PKHPkScript( std::shared_ptr<unsigned char> *retRaw , std::shared_ptr<unsigned char> pubKeyHash );

	unsigned int importRaw( unsigned char *fromRaw, unsigned int fromRawLength );

	std::shared_ptr<Script> script(){ return _script; };
};












}



#endif // A464F000_6B41_4360_B39C_A3EA44A8E80B
