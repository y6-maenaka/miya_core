#include "coinbase.hpp"


namespace tx
{


coinbase::coinbase( unsigned int height , std::shared_ptr<unsigned char> text , unsigned int textLength , std::shared_ptr<unsigned char> pubkeyHash , std::shared_ptr<core::MiyaCoreContext> mcContext )
{
  _meta._tx_in = std::shared_ptr<coinbase_tx_in>( new coinbase_tx_in(height, text , textLength) ); // 宣言と同時に初期化する
	_meta._tx_out = std::shared_ptr<TxOut>( new TxOut );
	_pubkeyHash = pubkeyHash;

	_mcContext = mcContext;
}

coinbase::coinbase()
{
	_meta._tx_in = std::shared_ptr<coinbase_tx_in>( new coinbase_tx_in );
	_meta._tx_out = std::shared_ptr<TxOut>( new TxOut );
}

unsigned int coinbase::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	std::shared_ptr<unsigned char> rawTxIn; unsigned int rawTxInLength;
	rawTxInLength = _meta._tx_in->exportRaw( &rawTxIn );

	assert( !(_pubkeyHash == nullptr) );
	_meta._tx_out->pubKeyHash( _pubkeyHash );
	std::shared_ptr<unsigned char> rawTxOut; unsigned int rawTxOutLength;
	rawTxOutLength = _meta._tx_out->exportRaw( &rawTxOut );

	uint32_t tx_in_count = htonl(1);
	uint32_t tx_out_count = htonl(1);
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[sizeof(_meta._version) + rawTxInLength + rawTxOutLength + sizeof(tx_in_count) + sizeof(tx_out_count)] );

	/*
	std::cout << "----------------------------" << "\n";
	std::cout << "[ Detail of RawCoinbase ]" << "\n";
	std::cout << "raw version length : " << sizeof(_meta._version) << "\n";
	std::cout << "raw tx in count length : " << sizeof(tx_in_count) << "\n";
	std::cout << "raw tx in length : " << rawTxInLength << "\n";
	std::cout << "raw tx out count length : " << sizeof(tx_out_count) << "\n";
	std::cout << "raw tx out length : " << rawTxOutLength << "\n";
	std::cout << "----------------------------" << "\n";
	*/

	int currentPtr = 0;
	memcpy( (*retRaw).get() + currentPtr , &(_meta._version) , sizeof(_meta._version) ); currentPtr += sizeof(_meta._version);
	memcpy( (*retRaw).get() + currentPtr , &tx_in_count , sizeof(tx_in_count) ); currentPtr += sizeof(tx_in_count);
	memcpy( (*retRaw).get() + currentPtr  , rawTxIn.get() , rawTxInLength ); currentPtr += rawTxInLength;
	memcpy( (*retRaw).get() + currentPtr , &tx_out_count,  sizeof(tx_out_count) ); currentPtr += sizeof(tx_out_count);
	memcpy( (*retRaw).get() + currentPtr , rawTxOut.get() , rawTxOutLength ); currentPtr += rawTxOutLength;

	return currentPtr;
}

size_t coinbase::importRawSequentially( std::shared_ptr<unsigned char> from )
{
	unsigned int currentPtr = 0;
	memcpy( &(_meta._version) , from.get() , sizeof(_meta._version) ); currentPtr += sizeof(_meta._version);

	uint32_t tx_in_count;
	memcpy( &tx_in_count, from.get() + currentPtr , sizeof(tx_in_count) ); currentPtr += sizeof(tx_in_count);
	currentPtr += _meta._tx_in->importRaw( from.get() + currentPtr );


	uint32_t tx_out_count;
	memcpy( &tx_out_count, from.get() + currentPtr , sizeof(tx_out_count) ); currentPtr += sizeof(tx_out_count);
	currentPtr += _meta._tx_out->importRaw( from.get() + currentPtr );

	// std::cout << _meta._tx_in->scriptBytes() << "\n";
	// std::cout << _meta._tx_out->pkScriptBytes() << "\n";

	return currentPtr;
}


size_t coinbase::importRawSequentially( void *from )
{
	unsigned char* _from = static_cast<unsigned char *>(from);
	unsigned int currentPtr = 0;
	memcpy( &(_meta._version) , _from , sizeof(_meta._version) ); currentPtr += sizeof(_meta._version);

	uint32_t tx_in_count;
	memcpy( &tx_in_count, _from + currentPtr , sizeof(tx_in_count) ); currentPtr += sizeof(tx_in_count);
	currentPtr += _meta._tx_in->importRaw( _from + currentPtr );

	uint32_t tx_out_count;
	memcpy( &tx_out_count, _from + currentPtr , sizeof(tx_out_count) ); currentPtr += sizeof(tx_out_count);
	currentPtr += _meta._tx_out->importRaw( _from + currentPtr );

	return currentPtr;
}

void coinbase::add( std::shared_ptr<tx::TxOut> target )
{
	_meta._tx_out = target;
}

int coinbase::height()
{
	return _meta._tx_in->height();
}

std::shared_ptr<coinbase_tx_in> coinbase::txIn()
{
	return _meta._tx_in;
}

std::shared_ptr<TxOut> coinbase::txOut()
{
	return _meta._tx_out;
}

unsigned int coinbase::calcTxID( std::shared_ptr<unsigned char> *ret )
{
	std::shared_ptr<unsigned char> exportedCoinbase; unsigned int exportedCoinbaseLength;
	exportedCoinbaseLength = this->exportRaw( &exportedCoinbase );

	return hash::SHAHash( exportedCoinbase, exportedCoinbaseLength , ret , "sha256" );
}

std::vector<std::uint8_t> coinbase::export_to_binary() const
{
  std::vector<std::uint8_t> version_binary = convert_uint32_to_vector_uint8( _meta._version );
  std::vector<std::uint8_t> coinbase_tx_in_binary = _meta._tx_in->export_to_binary();
  std::vector<std::uint8_t> tx_out_binary = _meta._tx_out->export_to_binary();

  std::vector<std::uint8_t> ret; 
  ret.reserve( version_binary.size() + coinbase_tx_in_binary.size() + tx_out_binary.size() );
  
  ret.insert( ret.end(), version_binary.begin(), version_binary.end() );
  ret.insert( ret.end(), coinbase_tx_in_binary.begin(), coinbase_tx_in_binary.end() );
  ret.insert( ret.end(), tx_out_binary.begin(), tx_out_binary.end() );

  return ret;
}


};
