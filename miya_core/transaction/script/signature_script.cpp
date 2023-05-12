#include "signature_script.h"

#include "../../../shared_components/cipher/ecdsa_manager.h"
#include "openssl/asn1.h"

namespace tx{








EVP_PKEY* SignatureScript::pubKey()
{

	unsigned char* rawPubKey = static_cast<unsigned char *>(_script.at(1).second);

	return  cipher::ECDSAManager::toPkey( rawPubKey,   OPDataSize( _script.at(1).first) );
}


unsigned int SignatureScript::sig( unsigned char **sig )
{
	*sig = static_cast<unsigned char *>(_script.at(0).second);

	return OPDataSize( _script.at(0).first );
}




SignatureScript *SignatureScript::createSigScript( unsigned char* sig , unsigned int sigSize, EVP_PKEY *pkey )
{
	_script.clear();

	unsigned char _OPSize = 0;
	_OPSize = static_cast<unsigned char>( sigSize & 0xFF );

	unsigned char* rawPubKey = nullptr; unsigned int rawPubKeySize = 0;
	rawPubKeySize = cipher::ECDSAManager::toRawPubKey( &rawPubKey , pkey );
	unsigned char _rawPubKeySize = static_cast<unsigned char>( rawPubKeySize & 0xFF );


	_script.push_back( std::make_pair( _OPSize , sig ) );
	_script.push_back( std::make_pair( _rawPubKeySize , rawPubKey ) );

	// オブション	 その後の処理の手間を省く
	_scriptType = static_cast<int>(SCRIPT_TYPE::Sig);



	return this;

}



int SignatureScript::scriptType()
{
	return _scriptType;
}




SignatureScript *SignatureScript::createHashedPubKeyScript( EVP_PKEY *pkey )
{		
	// なぜか追加していなくても要素が最初から入っていることがある
	_script.clear();
	
	unsigned char* rawPubKey = NULL;    unsigned int rawPubKeySize = 0; 
	unsigned char* hashedRawPubKey = NULL ; unsigned int hashedRawPubKeySize = 20;  // <- magic number 
	unsigned char _OPSize = 0; 

	rawPubKeySize = cipher::ECDSAManager::toRawPubKey( &rawPubKey , pkey );
	hashedRawPubKey = hash::SHAHash( rawPubKey , rawPubKeySize , "sha1" );

	_OPSize = static_cast<unsigned char>( hashedRawPubKeySize & 0xFF );

	// std::cout << "文字列サイズ ->" << static_cast<unsigned char>(hashedPubKeySize & 0xFF) << "\n";

	_script.push_back( std::make_pair( _OPSize, hashedRawPubKey ));



	// オブション	 その後の処理の手間を省く
	_scriptType = static_cast<int>(SCRIPT_TYPE::HashedPubKey);



	return this;
}
																																	






}; // close tx namespace
