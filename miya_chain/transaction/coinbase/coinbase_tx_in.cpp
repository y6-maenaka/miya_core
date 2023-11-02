#include "coinbase_tx_in.h"


#include "../script/script.h"
#include "../tx/tx_out.h"
#include "../tx/prev_out.h"



namespace tx
{




CoinbaseTxIn::CoinbaseTxIn()
{
	_body._prevOut = std::shared_ptr<PrevOut>( new PrevOut );
	_body._script = std::shared_ptr<Script>( new Script );
}




CoinbaseTxIn::CoinbaseTxIn( uint32_t height , std::shared_ptr<unsigned char> text , unsigned int textLength )
{

	_body._prevOut = std::shared_ptr<PrevOut>( new PrevOut );
	_body._script = std::shared_ptr<Script>( new Script );


	memset( &(_body._sequence), 0x00 , sizeof(_body._sequence) ); // coinbasenのシーケンスは0

	// PrevOutの初期化 // 実際はprevOutのコンストラクタで初期化している
	_body._prevOut->txID(nullptr);
	_body._prevOut->index( UINT32_MAX );


 // heightをscriptにプッシュ	
 // 任意の文字をscriptにプッシュ
	OP_DATA	heigthData(0x04);
	uint32_t heightWithLissleEndian = htonl(height);
	std::shared_ptr<unsigned char> cHeigth = std::shared_ptr<unsigned char>( new unsigned char[sizeof(height)] ); memcpy( cHeigth.get() , &heightWithLissleEndian, sizeof(height) );
	_body._script->pushBack( heigthData, cHeigth );

	if( textLength - sizeof(height) <= 100 ){ //  heightと合わせて100バイトまで
		OP_DATA msgData(static_cast<unsigned char>(textLength));
		_body._script->pushBack( msgData, text );
	} 


	return;
}


std::shared_ptr<PrevOut> CoinbaseTxIn::prevOut()
{
	return _body._prevOut;
}

	

unsigned short CoinbaseTxIn::scriptBytes()
{
	return ntohl(_body._script_bytes);
}




unsigned int CoinbaseTxIn::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	std::shared_ptr<unsigned char> rawPrevOut; unsigned int rawPrevOutLength;
	rawPrevOutLength = _body._prevOut->exportRaw( &rawPrevOut );

	std::shared_ptr<unsigned char> rawScript; unsigned int rawScriptLength;
	rawScriptLength = _body._script->exportRaw( &rawScript );
	_body._script_bytes = htonl(rawScriptLength);


	int formatPtr = 0;
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[rawPrevOutLength + sizeof(_body._script_bytes) + this->scriptBytes() + sizeof(_body._sequence)] );

	memcpy( (*retRaw).get() , rawPrevOut.get() , rawPrevOutLength ); formatPtr += rawPrevOutLength;
	memcpy( (*retRaw).get() + formatPtr, &(_body._script_bytes), sizeof(_body._script_bytes) ); formatPtr += sizeof(_body._script_bytes);
	memcpy( (*retRaw).get() + formatPtr, rawScript.get() , rawScriptLength ); formatPtr += rawScriptLength;
	memcpy( (*retRaw).get() + formatPtr, &(_body._sequence) , sizeof(_body._sequence) ); formatPtr += sizeof(_body._sequence);

	return formatPtr;
}






unsigned int CoinbaseTxIn::importRaw( unsigned char *fromRaw )
{
	int currentPtr = 0;
	unsigned int prevOutLength;

	prevOutLength = _body._prevOut->importRaw( fromRaw );  currentPtr += prevOutLength; // prevOutの取り込み
	memcpy( &_body._script_bytes , fromRaw + currentPtr , sizeof(_body._script_bytes) ); currentPtr += sizeof(_body._script_bytes); // スクリプトバイト長の取り込み

	unsigned int scriptLength = 0;
	scriptLength = _body._script->importRaw( fromRaw + currentPtr , this->scriptBytes() ); currentPtr += scriptLength; // スクリプトの取り込み

	// sequence の取り込み
	memcpy( &(_body._sequence) , fromRaw + currentPtr, sizeof(_body._sequence) ); currentPtr += sizeof(_body._sequence);

	return currentPtr;
}







};
