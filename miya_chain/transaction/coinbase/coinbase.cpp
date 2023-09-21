#include "coinbase.h"

#include "./coinbase_tx_in.h"
#include "../tx/tx_out.h"



namespace tx
{


Coinbase::Coinbase( unsigned int height , std::shared_ptr<unsigned char> text , unsigned int textLength )
{   
  _body._txIn = std::shared_ptr<CoinbaseTxIn>( new CoinbaseTxIn(height, text , textLength) ); // 宣言と同時に初期化する

}


Coinbase::Coinbase()
{
	_body._txIn = std::shared_ptr<CoinbaseTxIn>( new CoinbaseTxIn );
	_body._txOut = std::shared_ptr<TxOut>( new TxOut );
}


unsigned int Coinbase::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	std::shared_ptr<unsigned char> rawTxIn; unsigned int rawTxInLength;
	rawTxInLength = _body._txIn->exportRaw( &rawTxIn );

	std::shared_ptr<unsigned char> rawTxOut; unsigned int rawTxOutLength;
	rawTxOutLength = _body._txOut->exportRaw( &rawTxOut );


	uint32_t tx_in_count = htonl(1);
	uint32_t tx_out_count = htonl(1);
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[sizeof(_body._version) + rawTxInLength + rawTxOutLength + sizeof(tx_in_count) + sizeof(tx_out_count)] );

	int currentPtr = 0;
	memcpy( (*retRaw).get() + currentPtr , &(_body._version) , sizeof(_body._version) ); currentPtr += sizeof(_body._version);
	memcpy( (*retRaw).get() + currentPtr , &tx_in_count , sizeof(tx_in_count) ); currentPtr += sizeof(tx_in_count);
	memcpy( (*retRaw).get() + currentPtr  , rawTxIn.get() , rawTxInLength ); currentPtr += rawTxInLength;
	memcpy( (*retRaw).get() + currentPtr , &tx_out_count,  sizeof(tx_out_count) ); currentPtr += sizeof(tx_out_count);
	memcpy( (*retRaw).get() + currentPtr , rawTxOut.get() , rawTxOutLength ); currentPtr += rawTxOutLength;

	return currentPtr;
}



void Coinbase::importRaw( std::shared_ptr<unsigned char> from , unsigned int fromLength )
{
	unsigned int currentPtr = 0;
	memcpy( &(_body._version) , from.get() , sizeof(_body._version) ); currentPtr += sizeof(_body._version);


	uint32_t tx_in_count;
	memcpy( &tx_in_count, from.get() + currentPtr , sizeof(tx_in_count) ); currentPtr += sizeof(tx_in_count);
	currentPtr += _body._txIn->importRaw( from.get() + currentPtr );


	uint32_t tx_out_count;
	memcpy( &tx_out_count, from.get() + currentPtr , sizeof(tx_out_count) ); currentPtr += sizeof(tx_out_count);
	currentPtr += _body._txOut->importRaw( from.get() + currentPtr );

	// std::cout << _body._txIn->scriptBytes() << "\n";
	// std::cout << _body._txOut->pkScriptBytes() << "\n";

	return;
}


void Coinbase::add( std::shared_ptr<tx::TxOut> target )
{
	_body._txOut = target;
}


}
