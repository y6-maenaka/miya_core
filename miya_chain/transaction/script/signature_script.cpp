#include "signature_script.h"

#include "../../../shared_components/cipher/ecdsa_manager.h"
#include "../../../shared_components/hash/sha_hash.h"
#include "openssl/asn1.h"

namespace tx{





void SignatureScript::pkey( EVP_PKEY *pkey )
{
	_pkey = pkey;
}


void SignatureScript::sign( std::shared_ptr<unsigned char> sign, unsigned int signLength, bool isSigned )
{
	_signature._sign = sign;
	_signature._signLength = signLength;
	_signature._isSigned = isSigned;
}


bool SignatureScript::isSigned()
{
	return _signature._isSigned;
}




unsigned int SignatureScript::importRaw( unsigned char* fromRaw , unsigned int fromRawLength )
{
	if( fromRawLength <= (160/8)) return 0;

	// --- signature script
	// (1) signature
	// (2) pubkey


	// 署名の取り込み
	_signature._signLength = fromRawLength - (160/8);
	_signature._sign = std::make_shared<unsigned char>(_signature._signLength);
	memcpy( _signature._sign.get() , fromRaw , _signature._signLength );

	// 公開鍵の取り出し
	_pkey = cipher::ECDSAManager::toPkey( fromRaw + _signature._signLength , (160/8) );

	return _signature._signLength;
}


unsigned short SignatureScript::exportRawWithPubKeyHash( std::shared_ptr<unsigned char> *ret )
{
	if( _pkey == nullptr ) return 0;

	unsigned int rawPubKeyLength; std::shared_ptr<unsigned char> rawPueKey;
	rawPubKeyLength = cipher::ECDSAManager::toRawPubKey( _pkey , &rawPueKey ); // 生の公開鍵を書き出す

	unsigned int rawPubKeyHashLength; std::shared_ptr<unsigned char> rawPubKeyHash;
	rawPubKeyHashLength = hash::SHAHash( rawPueKey, rawPubKeyLength , &rawPubKeyHash , "sha1" ); // 160bitsでダイジェスト変換

	*ret = std::shared_ptr<unsigned char>( new unsigned char[rawPubKeyHashLength] );
	memcpy( (*ret).get() , rawPubKeyHash.get() , rawPubKeyHashLength );

	return rawPubKeyHashLength;
}



unsigned short SignatureScript::exportRawWithSignatureScript( std::shared_ptr<unsigned char> *ret )
{
	// 書き出す署名スクリプトは sign + 公開鍵

	if( _pkey == nullptr ) return 0;


	unsigned int rawPubKeyLength; std::shared_ptr<unsigned char> rawPueKey;
	rawPubKeyLength = cipher::ECDSAManager::toRawPubKey( _pkey , &rawPueKey ); // 生の公開鍵を書き出す
	
	*ret = std::shared_ptr<unsigned char>( new unsigned char[_signature._signLength + rawPubKeyLength] );  

	unsigned int formatPtr = 0;
	memcpy( (*ret).get(), _signature._sign.get() , _signature._signLength ); formatPtr += _signature._signLength;
	memcpy( (*ret).get() + formatPtr , rawPueKey.get() , rawPubKeyLength ); formatPtr += rawPubKeyLength;

	return formatPtr;
}



/*
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
*/







}; // close tx namespace
