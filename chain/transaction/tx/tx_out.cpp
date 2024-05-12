#include "tx_out.h"


namespace tx{


TxOut::TxOut()
{
	//_meta._pkScript = std::make_shared<PkScript>();
	_meta._pkScript = std::shared_ptr<PkScript>( new PkScript );
}

void TxOut::init( unsigned int value , std::shared_ptr<unsigned char> pubKeyHash )
{
	#ifdef __linux__
		_meta._value = be64toh(value);
	#endif
	#ifdef __APPLE__
		_meta._value = ntohll(value);
	#endif
	_pubKeyHash	= pubKeyHash;
}


unsigned short TxOut::exportRaw( std::shared_ptr<unsigned char> *retRaw ) // これが正常に書き出せていない
{
	if( _pubKeyHash == nullptr )
	{
		retRaw = nullptr; return 0;
	}

	// 先ず,pkScriptを書き出す バイト長などを格納しなければならない為
	std::shared_ptr<unsigned char> exportedRawPkScript; unsigned int exportedRawPkScriptLength = 0;

	exportedRawPkScriptLength = _meta._pkScript->exportRawWithP2PKHPkScript( &exportedRawPkScript , _pubKeyHash );

	#ifdef __linux__
	_meta._pkScriptBytes = be64toh(static_cast<uint64_t>(exportedRawPkScriptLength)); // 書き出しと同時に代入は怖いので
	#endif
	#ifdef __APPLE__
	_meta._pkScriptBytes = ntohll(static_cast<uint64_t>(exportedRawPkScriptLength)); // 書き出しと同時に代入は怖いので
	#endif	
	//_meta._pkScriptBytes = (exportedRawPkScriptLength); // 書き出しと同時に代入は怖いので

	*retRaw = std::shared_ptr<unsigned char>( new unsigned char [sizeof(_meta._value) + sizeof(_meta._pkScriptBytes) + pkScriptBytes()] );

	unsigned int formatPtr = 0;

	memcpy( (*retRaw).get() , &(_meta._value), sizeof(_meta._value) ); formatPtr+= sizeof(_meta._value); // valueの書き出し

	memcpy( (*retRaw).get() + formatPtr , &(_meta._pkScriptBytes) , sizeof(_meta._pkScriptBytes) ); formatPtr+= sizeof(_meta._pkScriptBytes); // _pkScriptBytesの書き出し

	memcpy( (*retRaw).get() + formatPtr , exportedRawPkScript.get() , pkScriptBytes() ); formatPtr+= pkScriptBytes();

	exportedRawPkScript.reset(); // 一応解放しておく
	

	return formatPtr;
}


std::shared_ptr<unsigned char> TxOut::pubKeyHash()
{
	return _pubKeyHash;
}


void TxOut::pubKeyHash( std::shared_ptr<unsigned char> pubKeyHash )
{
	_pubKeyHash = pubKeyHash;
}




int TxOut::importRaw( unsigned char *fromRaw )
{
	unsigned int currentPtr = 0;

	memcpy( &(_meta._value) , fromRaw , sizeof(_meta._value) ); currentPtr += sizeof(_meta._value); // valueの読み込み

	// pkScriptBytes の読み込み
	memcpy( &(_meta._pkScriptBytes) , fromRaw + currentPtr , sizeof(_meta._pkScriptBytes) ); currentPtr += sizeof(_meta._pkScriptBytes);

	unsigned int importedPkScriptLength;
	importedPkScriptLength = _meta._pkScript->importRaw( fromRaw + currentPtr , this->pkScriptBytes() ); currentPtr += importedPkScriptLength;

	_pubKeyHash = _meta._pkScript->script()->at(2).second;

	return currentPtr;
}




unsigned short TxOut::value()
{
	// return ntohll(_meta._value);
	#ifdef __linux__
		return be64toh(_meta._value);
	#endif

	#ifdef __APPLE__
		return ntohll(_meta._value);
	#endif
}

void TxOut::value( unsigned int value )
{
	_meta._value = htonll(value);
}

unsigned int TxOut::pkScriptBytes() const
{
	#ifdef __linux__
		return static_cast<unsigned int >(be64toh(_meta._pkScriptBytes));
	#endif
	#ifdef __APPLE__
		return static_cast<unsigned int >(ntohll(_meta._pkScriptBytes));
	#endif
}


void to_json( json& from , const TxOut &to )
{

}

void from_json( const json &from , TxOut &to )
{
	#ifdef __linux__
		to._meta._value = be64toh(static_cast<int64_t>(from["value"])); // ビックエディアンに変換する
	#endif
	#ifdef __APPLE__
		to._meta._value = ntohll(static_cast<int64_t>(from["value"])); // ビックエディアンに変換する
	#endif

	//to._pubKeyHash = from["address"];
	std::string sAddress = from["address"].get<std::string>();

	std::vector<unsigned char> hexBinaryVector;
	for( size_t i=0; i<40; i+=2 ) // 20[bytes]*2 あとで拡張性を持たせて修正する
	{
		std::string subString = sAddress.substr(i,2);
		unsigned char byte = static_cast<unsigned char>(std::stoul(subString,nullptr,16));
		hexBinaryVector.push_back(byte);
	}

	//const unsigned char *ccAddress = reinterpret_cast<const unsigned char*>(sAddress.c_str());

	//to._pubKeyHash = std::make_shared<unsigned char>(20);
	to._pubKeyHash = std::shared_ptr<unsigned char>(new unsigned char[20] , [](unsigned char *ptr){
			delete[] ptr;
			});
	std::copy( hexBinaryVector.begin(), hexBinaryVector.end(), to._pubKeyHash.get() );
}


std::vector<std::uint8_t> TxOut::export_to_binary() // const
{ 
  // 仮の実装
  unsigned short tx_out_binary_length; 
  std::shared_ptr<unsigned char> tx_out_binary;
  tx_out_binary_length = this->exportRaw( &tx_out_binary );
  
  std::vector<std::uint8_t> ret;
  ret.resize( tx_out_binary_length ); 
  std::memcpy( ret.data(), tx_out_binary.get(), tx_out_binary_length );
  return ret;
}


}; // close tx namespace 
