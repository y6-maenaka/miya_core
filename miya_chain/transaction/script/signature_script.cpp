#include "signature_script.h"

#include "../../../shared_components/cipher/ecdsa_manager.h"
#include "../../../shared_components/hash/sha_hash.h"
#include "openssl/asn1.h"

namespace tx{



SignatureScript::SignatureScript()
{
	_script = std::shared_ptr<Script>( new Script );
}



void SignatureScript::pkey( EVP_PKEY *pkey )
{
	_pkey = pkey;
}


EVP_PKEY *SignatureScript::pkey()
{
	return _pkey;
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
	//_signature._signLength = fromRawLength - (160/8); // 公開鍵ハッシュを取り除いたサイズ
	//_signature._sign = std::shared_ptr<unsigned char>( new unsigned char[_signature._signLength]);
	//memcpy( _signature._sign.get() , fromRaw , _signature._signLength );

	// 公開鍵の取り出し
	//_pkey = cipher::ECDSAManager::toPkey( fromRaw + _signature._signLength , (160/8) );
	_script->importRaw( fromRaw , fromRawLength ); // 生の署名スクリプトを取り込む


	std::pair< OP_CODES , std::shared_ptr<unsigned char> > rawSignPair = _script->at(0); // 署名の取り出し
	_signature._sign = rawSignPair.second;
	_signature._signLength = _script->OP_DATALength(rawSignPair.first);

	std::pair< OP_CODES , std::shared_ptr<unsigned char> > rawPubKeyPair = _script->at(1); // マジックナンバーだがP2PKHではこのインデックスしか取りえないので問題ないかも
	_pkey = cipher::ECDSAManager::toPkey( rawPubKeyPair.second.get() , _script->OP_DATALength(rawPubKeyPair.first) );

	if( _pkey == nullptr )
		std::cout << "[ Warning ] pkey is nullptr" << "\n";


	// ここから公開鍵を取り出す

	return fromRawLength;
}


unsigned short SignatureScript::exportRawWithPubKeyHash( std::shared_ptr<unsigned char> *ret )
{
	if( _pkey == nullptr ) return 0;
	if( _script->OPCount() > 0 ) _script->clear();

	unsigned int rawPubKeyLength; std::shared_ptr<unsigned char> rawPueKey;
	rawPubKeyLength = cipher::ECDSAManager::toRawPubKey( _pkey , &rawPueKey ); // 生の公開鍵を書き出す

	unsigned int rawPubKeyHashLength; std::shared_ptr<unsigned char> rawPubKeyHash;
	rawPubKeyHashLength = hash::SHAHash( rawPueKey, rawPubKeyLength , &rawPubKeyHash , "sha1" ); // 160bitsでダイジェスト変換

	*ret = std::shared_ptr<unsigned char>( new unsigned char[rawPubKeyHashLength] );
	memcpy( (*ret).get() , rawPubKeyHash.get() , rawPubKeyHashLength );

	/*
	std::cout << "\x1b[36m";
	std::cout << "(raw) " << rawPubKeyLength << "::";
	for( int i=0; i<rawPubKeyLength; i++ )
	{
		printf("%02X", rawPueKey.get()[i]);
	} std::cout << "\n";

	std::cout << "(hashed) :: ";
	for( int i=0; i<20; i++ )
	{
		printf("%02X", rawPubKeyHash.get()[i]);
	} std::cout << "\n";
	std::cout <<"\x1b[39m";
	*/


	return rawPubKeyHashLength;
}



unsigned short SignatureScript::exportRawWithSignatureScript( std::shared_ptr<unsigned char> *ret )
{
	// 書き出す署名スクリプトは sign + 公開鍵

	if( _pkey == nullptr ) return 0;
	if( _script->OPCount() > 0 ) _script->clear();



	
	OP_DATA _OP_DATA_SIGN(static_cast<unsigned char>( _signature._signLength & 0xFF ));
	_script->pushBack( _OP_DATA_SIGN , _signature._sign ); // 署名をデータとして追加


	unsigned short _rawPubKeyLength; std::shared_ptr<unsigned char> rawPubKey; 
	_rawPubKeyLength = cipher::ECDSAManager::toRawPubKey( _pkey,  &rawPubKey );

	OP_DATA _OP_DATA_PUBKEY( static_cast<unsigned char>( _rawPubKeyLength & 0xFF ));
	_script->pushBack( _OP_DATA_PUBKEY , rawPubKey ); // 生公開鍵をデータとして追加

	unsigned retLength = _script->exportRaw( ret );

	return retLength;
	//return _script->exportRaw( ret );
	
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
