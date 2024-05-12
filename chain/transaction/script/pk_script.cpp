#include "pk_script.h"


namespace tx{



PkScript::PkScript()
{
	_script = std::make_shared<Script>();

}





/*
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




unsigned int PkScript::exportRawWithP2PKHScript( std::shared_ptr<unsigned char> destAddress )
{
	destAddress = std::make_shared<unsigned char>(20);

}
*/


//using OP_CODES = std::variant<_OP_DUP, _OP_HASH_160, _OP_EQUALVERIFY, _OP_CHECKSIG, _OP_DATA>;



unsigned int PkScript::exportRawWithP2PKHPkScript( std::shared_ptr<unsigned char> *retRaw , std::shared_ptr<unsigned char> pubKeyHash )
{
	if( pubKeyHash == nullptr ) return 0;
	if( _script->OPCount() > 0 ) _script->clear();


	//OP_CODES pushOPCODE;

	/*
	std::cout << "( check 1 )" << "\n";
	// OP_DUPの追加
	pushOPCODE.emplace<static_cast<int>(OP_CODES_ID::OP_DUP)>(_OP_DUP);

	std::cout << "( check 1-1 )" << "\n";

	_script->pushBack( pushOPCODE , std::make_shared<unsigned char>(_OP_DUP._code) );

	std::cout << "( check 2 )" << "\n";

	// OP_HASH_160の追加
	pushOPCODE.emplace<static_cast<int>(OP_CODES_ID::OP_HASH_160)>(OP_HASH_160);
	_script->pushBack( pushOPCODE , std::make_shared<unsigned char>(OP_HASH_160) );

	// OP_DATA(PUBKEY_HASH)の追加
	pushOPCODE.emplace<static_cast<int>(OP_CODES_ID::OP_DATA)>(0x14); // 0x14 -> 20 [ bytse ]
	_script->pushBack( pushOPCODE , pubKeyHash );

	// OP_EQUALVERIFYの追加
	pushOPCODE.emplace<static_cast<int>(OP_CODES_ID::OP_EQUALVERIFY)>(OP_EQUALVERIFY);
	_script->pushBack( pushOPCODE , std::make_shared<unsigned char>(OP_EQUALVERIFY) );

	// OP_CHECKSIGの追加
	pushOPCODE.emplace<static_cast<int>(OP_CODES_ID::OP_CHECKSIG)>(OP_CHECKSIG);
	_script->pushBack( pushOPCODE , std::make_shared<unsigned char>(OP_CHECKSIG) );
	*/

	// OP_DUPの追加
	_script->pushBack( _OP_DUP );
	// OP_HASH_160の追加
	_script->pushBack( _OP_HASH_160 );
	// OP_DATA(pubkeyHash)の追加
	OP_DATA _OP_DATA(0x14);
	_script->pushBack( _OP_DATA , pubKeyHash );
	//OP_EQUALVERIFYの追加
	_script->pushBack( _OP_EQUALVERIFY );
	//OP_CHECKSIGの追加
	_script->pushBack( _OP_CHECKSIG );


	unsigned int retRawLength = _script->exportRaw( retRaw ); // 全体の書き出し


	return retRawLength;
}




unsigned int PkScript::importRaw( unsigned char *fromRaw, unsigned int fromRawLength )
{
	return _script->importRaw( fromRaw , fromRawLength );
}





}; // close tx namespace
