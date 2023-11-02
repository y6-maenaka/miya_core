#include "coinbase.h"

#include "./coinbase_tx_in.h"
#include "../tx/tx_out.h"

#include "../../../miya_core/miya_core.h"
#include "../../../shared_components/hash/sha_hash.h"


namespace tx
{


Coinbase::Coinbase( unsigned int height , std::shared_ptr<unsigned char> text , unsigned int textLength , std::shared_ptr<unsigned char> pubkeyHash , std::shared_ptr<miya_core::MiyaCoreContext> mcContext )
{   
  _body._txIn = std::shared_ptr<CoinbaseTxIn>( new CoinbaseTxIn(height, text , textLength) ); // 宣言と同時に初期化する
	_body._txOut = std::shared_ptr<TxOut>( new TxOut );
	_pubkeyHash = pubkeyHash;

	_mcContext = mcContext;
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

	assert( !(_pubkeyHash == nullptr) );
	_body._txOut->pubKeyHash( _pubkeyHash );
	std::shared_ptr<unsigned char> rawTxOut; unsigned int rawTxOutLength;
	rawTxOutLength = _body._txOut->exportRaw( &rawTxOut );
	
	uint32_t tx_in_count = htonl(1);
	uint32_t tx_out_count = htonl(1);
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[sizeof(_body._version) + rawTxInLength + rawTxOutLength + sizeof(tx_in_count) + sizeof(tx_out_count)] );

	/*
	std::cout << "----------------------------" << "\n";
	std::cout << "[ Detail of RawCoinbase ]" << "\n";
	std::cout << "raw version length : " << sizeof(_body._version) << "\n";
	std::cout << "raw tx in count length : " << sizeof(tx_in_count) << "\n";
	std::cout << "raw tx in length : " << rawTxInLength << "\n";
	std::cout << "raw tx out count length : " << sizeof(tx_out_count) << "\n";
	std::cout << "raw tx out length : " << rawTxOutLength << "\n";
	std::cout << "----------------------------" << "\n";
	*/

	int currentPtr = 0;
	memcpy( (*retRaw).get() + currentPtr , &(_body._version) , sizeof(_body._version) ); currentPtr += sizeof(_body._version);
	memcpy( (*retRaw).get() + currentPtr , &tx_in_count , sizeof(tx_in_count) ); currentPtr += sizeof(tx_in_count);
	memcpy( (*retRaw).get() + currentPtr  , rawTxIn.get() , rawTxInLength ); currentPtr += rawTxInLength;
	memcpy( (*retRaw).get() + currentPtr , &tx_out_count,  sizeof(tx_out_count) ); currentPtr += sizeof(tx_out_count);
	memcpy( (*retRaw).get() + currentPtr , rawTxOut.get() , rawTxOutLength ); currentPtr += rawTxOutLength;

	return currentPtr;
}



size_t Coinbase::importRawSequentially( std::shared_ptr<unsigned char> from )
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

	return currentPtr;
}


size_t Coinbase::importRawSequentially( void *from )
{
	unsigned char* _from = static_cast<unsigned char *>(from);
	unsigned int currentPtr = 0;
	memcpy( &(_body._version) , _from , sizeof(_body._version) ); currentPtr += sizeof(_body._version);

	uint32_t tx_in_count;
	memcpy( &tx_in_count, _from + currentPtr , sizeof(tx_in_count) ); currentPtr += sizeof(tx_in_count);
	currentPtr += _body._txIn->importRaw( _from + currentPtr );

	uint32_t tx_out_count;
	memcpy( &tx_out_count, _from + currentPtr , sizeof(tx_out_count) ); currentPtr += sizeof(tx_out_count);
	currentPtr += _body._txOut->importRaw( _from + currentPtr );

	return currentPtr;
}




void Coinbase::add( std::shared_ptr<tx::TxOut> target )
{
	_body._txOut = target;
}


std::shared_ptr<CoinbaseTxIn> Coinbase::txIn()
{
	return _body._txIn;
}

std::shared_ptr<TxOut> Coinbase::txOut()
{
	return _body._txOut;
}


unsigned int Coinbase::calcTxID( std::shared_ptr<unsigned char> *ret )
{
	std::shared_ptr<unsigned char> exportedCoinbase; unsigned int exportedCoinbaseLength;
	exportedCoinbaseLength = this->exportRaw( &exportedCoinbase );
	
	return hash::SHAHash( exportedCoinbase, exportedCoinbaseLength , ret , "sha256" );
}


}
