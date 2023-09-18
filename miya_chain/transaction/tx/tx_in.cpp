#include "tx_in.h"

#include "./common.h"
#include "../script/signature_script.h"
#include "../../../shared_components/cipher/ecdsa_manager.h"


namespace tx{




PrevOut::PrevOut()
{
	//_body._txID = std::make_shared<unsigned char>(256/8);
	_body._txID = std::shared_ptr<unsigned char>( new unsigned char[32] );
}


PrevOut::PrevOut( std::shared_ptr<unsigned char> fromRaw )
{
	_body._txID = std::shared_ptr<unsigned char>( new unsigned char[32] );
	this->importRaw( fromRaw );
}




std::shared_ptr<unsigned char> PrevOut::txID()
{
	return _body._txID;
}

void PrevOut::txID( std::shared_ptr<unsigned char> target )
{
	_body._txID = target;
}


void PrevOut::txID( const unsigned char *target )
{
	std::copy( target , target + 32 , _body._txID.get() );
}

unsigned short PrevOut::index()
{
	return static_cast<unsigned short>(ntohl(_body._index));
}

void PrevOut::index( int target )
{
	_body._index = htonl(target);
}


unsigned int PrevOut::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[ 32 + sizeof(_body._index)] ); // あとで修正する
	memcpy( (*retRaw).get() , _body._txID.get() , 32 );
	memcpy( (*retRaw).get() + 32, &(_body._index) , sizeof(_body._index) );

	return 32 + sizeof(_body._index);
}




int PrevOut::importRaw( std::shared_ptr<unsigned char> fromRaw )
{
	if( fromRaw == nullptr ) return -1;

	memcpy( _body._txID.get() , fromRaw.get() , 256/8 ); // あとでマジックナンバーは修正する
	memcpy( &(_body._index) , fromRaw.get() + (256/8) , sizeof(_body._index) );

	return ( (256/8) + sizeof(_body._index) ); // 流石に雑すぎる?
}




int PrevOut::importRaw( unsigned char* fromRaw )
{
	if( _body._txID == nullptr ) return -1;
	// _body._txID = std::make_shared<unsigned char>(*fromRaw);
	memcpy( &_body._index, fromRaw + (256/8) , sizeof(_body._index) );

	return ( (256/8) + sizeof(_body._index) ); // 流石に雑すぎる?
}









TxIn::TxIn()
{
	_body._prevOut = std::shared_ptr<PrevOut>( new PrevOut );
	_body._signatureScript = std::shared_ptr<SignatureScript>( new SignatureScript );
}




TxIn::Body::Body()
{
	_prevOut = nullptr;
	_script_bytes = 0;
	_signatureScript = nullptr;
	//_sequence = 0;
	memset( &_sequence, 0xff , sizeof(_sequence) );
}





std::shared_ptr<PrevOut> TxIn::prevOut()
{
	return _body._prevOut;
}


unsigned int TxIn::scriptBytes()
{
	return ntohl(_body._script_bytes);
}


void TxIn::scriptBytes( unsigned int bytes )
{
	_body._script_bytes = htonl(bytes);
}




// トランザクション全体署名用に空で書き出すメソッド
unsigned int TxIn::exportRawWithEmpty( std::shared_ptr<unsigned char> *retRaw )
{


	/*
	  空で書き出す際に必要な要素は⭐️のみ
		⭐️std::shared_ptr<PrevOut> _prevOut;
		uint32_t _script_bytes; // unLockingScriptのバイト長
		std::shared_ptr<SignatureScript> _signatureScript; // unlockingScriptの本体
		⭐️uint32_t _sequence;
	 */
	unsigned int formatPtr = 0;
	std::shared_ptr<unsigned char> rawPrevOut; unsigned int rawPrevOutSize;
	rawPrevOutSize = _body._prevOut->exportRaw( &rawPrevOut );


	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[ rawPrevOutSize+ sizeof( _body._sequence)] );

	memcpy( (*retRaw).get() , rawPrevOut.get() , rawPrevOutSize ); formatPtr+= rawPrevOutSize;
	memcpy( (*retRaw).get() + formatPtr , &(_body._sequence) , sizeof(_body._sequence) );  formatPtr += sizeof(_body._sequence);

	rawPrevOut.reset(); // 一応削除しておく
	return formatPtr;
}





unsigned int TxIn::exportRawWithPubKeyHash( std::shared_ptr<unsigned char> *retRaw )
{
	if( _pkey == nullptr ) return 0;

	// 生の公開鍵を取得する
	_body._signatureScript->pkey( _pkey ); // 署名スクリプトに公開鍵をセット

	unsigned int formatPtr = 0;

	// PrevOutの書き出し
	std::shared_ptr<unsigned char> rawPrevOut; unsigned int rawPrevOutSize;
	rawPrevOutSize = _body._prevOut->exportRaw( &rawPrevOut );

	/* 自身の公開鍵ハッシュの書き出し */
	std::shared_ptr<unsigned char> exportedPubKeyHash; unsigned int exportedPubKeyHashLength = 0;
	exportedPubKeyHashLength = _body._signatureScript->exportRawWithPubKeyHash( &exportedPubKeyHash );


	//_body._script_bytes = htonl(exportedPubKeyHashLength); // スクリプト長のセット
	this->scriptBytes(exportedPubKeyHashLength);


	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[sizeof(struct PrevOut) + sizeof(_body._sequence) + sizeof(_body._script_bytes) + _body._script_bytes] );
	memcpy( (*retRaw).get() , rawPrevOut.get() , rawPrevOutSize ); formatPtr+= rawPrevOutSize;
	memcpy( (*retRaw).get() + formatPtr , &(_body._script_bytes) , sizeof(_body._script_bytes) ); formatPtr+= sizeof(_body._script_bytes);
	memcpy( (*retRaw).get() + formatPtr , exportedPubKeyHash.get() , exportedPubKeyHashLength ); formatPtr+= exportedPubKeyHashLength;
	memcpy( (*retRaw).get() + formatPtr , &(_body._sequence) , sizeof(_body._sequence) );  formatPtr += sizeof(_body._sequence);


	//rawPrevOut.reset();
	//exportedPubKeyHash.reset(); // 念の為解放しておく

	return formatPtr;
}




unsigned int TxIn::exportRawWithSignatureScript( std::shared_ptr<unsigned char> *retRaw )
{
	if( !(isSigned()) ) return 0; // 正式な署名が行われていなければリターン


	// PrevOutの書き出し
	std::shared_ptr<unsigned char> rawPrevOut; unsigned int rawPrevOutSize;
	rawPrevOutSize = _body._prevOut->exportRaw( &rawPrevOut );

	/* 署名スクリプトの書き出し */
	std::shared_ptr<unsigned char> exportedSignatureScript; unsigned int exportedSignatureScriptLength = 0;
	exportedSignatureScriptLength = _body._signatureScript->exportRawWithSignatureScript( &exportedSignatureScript );
	this->scriptBytes(exportedSignatureScriptLength); // スクリプト長のセット


	unsigned int formatPtr = 0;
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[ rawPrevOutSize + sizeof(_body._sequence) + sizeof(_body._script_bytes) + this->scriptBytes() ] );
	memcpy( (*retRaw).get() , rawPrevOut.get() , rawPrevOutSize ); formatPtr+= rawPrevOutSize;
	memcpy( (*retRaw).get() + formatPtr , &(_body._script_bytes) , sizeof(_body._script_bytes) ); formatPtr+= sizeof(_body._script_bytes);
	memcpy( (*retRaw).get() + formatPtr , exportedSignatureScript.get() , exportedSignatureScriptLength ); formatPtr+= exportedSignatureScriptLength;
	memcpy( (*retRaw).get() + formatPtr , &(_body._sequence) , sizeof(_body._sequence) );  formatPtr += sizeof(_body._sequence);

	rawPrevOut.reset();
	exportedSignatureScript.reset(); // 念の為解放しておく

	return formatPtr;
}




bool TxIn::isSigned()
{
	return _body._signatureScript->isSigned();
}


void TxIn::sign( std::shared_ptr<unsigned char> sign , unsigned int signLength , bool isSigned )
{
	_body._signatureScript->sign( sign , signLength, isSigned );
}




void TxIn::pkey( EVP_PKEY *pkey )
{
	_pkey = pkey;
}



EVP_PKEY *TxIn::pkey()
{
	return _pkey;
}



/*
int TxIn::importRaw( std::shared_ptr<unsigned char> fromRaw )
{
	unsigned int currentPtr = 0;
	_body._prevOut = std::make_shared<PrevOut>( fromRaw ); // prevOutの取り込み
	memcpy( &_body._script_bytes , fromRaw.get() + currentPtr , sizeof(_body._script_bytes) );
}
*/


int TxIn::importRaw( unsigned char *fromRaw ) // ポインタの先頭が揃っていることを確認
{
	unsigned int currentPtr = 0;
	unsigned int prevOutLength;
	_body._prevOut = std::shared_ptr<PrevOut>();
	prevOutLength = _body._prevOut->importRaw( fromRaw );  currentPtr += prevOutLength;// prevOutの取り込み

	memcpy( &_body._script_bytes , fromRaw + currentPtr , sizeof(_body._script_bytes) ); currentPtr += sizeof(_body._script_bytes);  // script_bytesの取り込み

	unsigned int signatureScriptLength = 0;
	_body._signatureScript = std::make_shared<SignatureScript>();
	signatureScriptLength = _body._signatureScript->importRaw( fromRaw + currentPtr, static_cast<unsigned int>(_body._script_bytes) ); currentPtr += signatureScriptLength;

	memcpy( &_body._sequence , fromRaw + currentPtr  , sizeof(_body._sequence) ); currentPtr += sizeof(_body._sequence);

	return currentPtr;
}





void from_json( const json &from , TxIn &to )
{

	std::string sUTXOTxID = from["utxo_tx_id"].get<std::string>();

	//std::string sUTXOTxID = "e7a4cf8a4a6f5b12c087adfc4d0be4a8108b5c13c6a4c856ab2aecd6e9731f2d";


	std::vector<unsigned char> hexBinaryVector;
	for( size_t i = 0; i<64; i+=2 ) // あとで修正
	{
		std::string subString = sUTXOTxID.substr(i,2);
		unsigned char byte = static_cast<unsigned char>(std::stoul(subString,nullptr,16));
		hexBinaryVector.push_back(byte);
	}


	unsigned char *hexBinary = new unsigned char[hexBinaryVector.size()];
	std::copy( hexBinaryVector.begin() , hexBinaryVector.end(), hexBinary );
	to.prevOut()->txID(reinterpret_cast<const unsigned char*>(hexBinary));


	to.prevOut()->index( static_cast<int>(from["index"]) );

}



/*
PrevOut::PrevOut(){


	// dummy
	const char* id = "aaaaaaaaaabbbbbbbbbb";
	_txID = (unsigned char *)id;
	_index = htonl( 10 );
	return;
}



unsigned int PrevOut::exportRaw( unsigned char **ret )
{

	unsigned int pos = 0;
	*ret = new unsigned char[ exportRawSize() ];

	memcpy( *ret , _txID , TX_ID_SIZE );       pos += TX_ID_SIZE;
	memcpy( *ret + pos , &_index , sizeof(_index) );  // pos += sizeof(_index);

	return exportRawSize();
}


unsigned int PrevOut::exportRawSize()
{
	return TX_ID_SIZE + sizeof( _index ) ;
}
























TxIn::TxIn() : _sig(nullptr) , _sigSize(0)
{
	memset( &_sequence , 0xff , sizeof(_sequence) );
	//_signatureScript = NULL;

	SignatureScript *signatureScript = new SignatureScript;
	this->_signatureScript = signatureScript;


	return;
}



unsigned char* TxIn::sig()
{
	return _sig;
}


void TxIn::sig( unsigned char *sig )
{
	_sig = sig;
}




unsigned int TxIn::sigSize(){
	return _sigSize;
}



PrevOut *TxIn::prevOut()
{
	return _prevOut;
}


void TxIn::prevOut( unsigned char *prevOut , unsigned int prevOutSize )
{
	memcpy( _prevOut, prevOut , prevOutSize );
}




unsigned int TxIn::autoTakeInSignatureScript( unsigned char* from )
{
	unsigned int pos = 0;

	// 確かにsignatureScriptからfromがスタートしていないと合わなくなる
	memcpy( &_script_bytes , from , sizeof(_script_bytes) ); pos += sizeof(_script_bytes); // OK

	pos += _signatureScript->takeInScript( from + pos  , scriptBytes()) ;

	pos += sizeof(_sequence); // あとでちゃんと書き直す


	 //ここで取り出す？

	_pkey = _signatureScript->pubKey();
	//_pkey = cipher::ECDSAManager::_myPkey;
	_sigSize = _signatureScript->sig( &_sig );


	return pos;
}



unsigned int TxIn::scriptBytes()
{
	return ntohl( _script_bytes );
}


void TxIn::sigSize( unsigned int sigSize )
{
	_sigSize = sigSize;
}





EVP_PKEY* TxIn::pkey()
{
	return _pkey;
}



void TxIn::pkey( EVP_PKEY* pkey )
{
	_pkey = pkey;
}





SignatureScript *TxIn::signatureScript()
{
	return _signatureScript;
}




void TxIn::signatureScript( SignatureScript *signatureScript )
{
	_signatureScript = signatureScript;
}



unsigned int TxIn::exportEmptyRaw( unsigned char **ret )
{
	// signature_scriptを空にして書き出し
	*ret = new unsigned char[ exportEmptyRawSize() ];

	unsigned char* prevOutBuff;   unsigned int prevOutBuffSize = 0;

	// dummyで仮置き
	_prevOut = new PrevOut;

	prevOutBuffSize = _prevOut->exportRaw( &prevOutBuff );

	unsigned int pos = 0;
	memcpy( *ret , prevOutBuff, prevOutBuffSize ); pos += prevOutBuffSize;
	//memcpy( *ret + pos , &_script_bytes , sizeof( _script_bytes) ); pos += sizeof(_script_bytes);
	memcpy( *ret + pos , &_sequence , sizeof(_sequence) );   // pos += sizeof(_sequence);

	return exportEmptyRawSize();
}





unsigned int TxIn::exportEmptyRawSize()
{
	// signature_scriptを空にして書き出し
	return ( _prevOut->exportRawSize() + sizeof(_sequence) );
}






unsigned int TxIn::exportRaw( unsigned char **ret )
{
	*ret = new unsigned char[ exportRawSize() ];


	 //= PrevOut の書き出し ==================================================
	unsigned char* prevOutBuff = NULL; unsigned int prevOutBuffSize = 0;

	// dummyで仮置き
	_prevOut = new PrevOut;

	prevOutBuffSize = _prevOut->exportRaw( &prevOutBuff );
	// =====================================================================


	 = SignatureScript の書き出し ==================================================
	unsigned char *signatureScriptBuff = NULL; unsigned int signatureScriptBuffSize = 0;
	signatureScriptBuffSize =  _signatureScript->exportRaw( &signatureScriptBuff );
	_script_bytes = htonl( signatureScriptBuffSize ); // スクリプトサイズのセット
	 =====================================================================


	unsigned int pos = 0;
	memcpy( *ret , prevOutBuff , prevOutBuffSize );  pos += prevOutBuffSize;
	memcpy( *ret + pos , &_script_bytes , sizeof(_script_bytes) ); pos += sizeof(_script_bytes);
	memcpy( *ret + pos , signatureScriptBuff , signatureScriptBuffSize );  pos += signatureScriptBuffSize;
	memcpy( *ret + pos , &_sequence , sizeof(_sequence) );  pos += sizeof(_sequence);

	return exportRawSize();
}





unsigned int TxIn::exportRawSize()
{

	return _prevOut->exportRawSize() + sizeof(_script_bytes) + _signatureScript->exportRawSize() +  sizeof(_sequence);
}



std::shared_ptr<unsigned char> TxIn::inTxID()
{
	return std::make_shared<unsigned char>( *(_prevOut->_txID) );
}


unsigned short TxIn::inIndex()
{
	return static_cast<unsigned short>(_prevOut->_index);
}

*/





}; // close tx namespace
