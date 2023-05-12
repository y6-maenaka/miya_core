#include "pk_script.h"

#include "../../../shared_components/cipher/ecdsa_manager.h"


namespace tx{








PkScript *PkScript::createP2PKHScript( EVP_PKEY *destPubKey  )
{
	unsigned char* hashedDestAddr;
		
	hashedDestAddr = new unsigned char[20];
	memset( hashedDestAddr, 0x0 , 20 );
	cipher::ECDSAManager::toRawPubKey( &hashedDestAddr ,destPubKey  );

	_script.push_back( std::make_pair( OP_DUP, reinterpret_cast<unsigned char*>(const_cast<char *>(&OP_DUP)) ));
	_script.push_back( std::make_pair( OP_HASH_160, reinterpret_cast<unsigned char*>(const_cast<char *>(&OP_HASH_160)) ));
	_script.push_back( std::make_pair( 0x14 , hashedDestAddr ) );
	_script.push_back( std::make_pair( OP_EQUALVERIFY , reinterpret_cast<unsigned char*>(const_cast<char *>(&OP_EQUALVERIFY )) ));
	_script.push_back( std::make_pair( OP_CHECKSIG  , reinterpret_cast<unsigned char*>(const_cast<char *>(&OP_CHECKSIG  )) ));



	return this;
};






}; // close tx namespace
