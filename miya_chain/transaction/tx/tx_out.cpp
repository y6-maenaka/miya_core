#include "tx_out.h"

#include "../script/pk_script.h"



namespace tx{


TxOut::TxOut()
{
	//_body._pkScript = std::make_shared<PkScript>();
	_body._pkScript = std::shared_ptr<PkScript>( new PkScript );
}



void TxOut::init( unsigned int value , std::shared_ptr<unsigned char> pubKeyHash )
{
	#ifdef __linux__
		_body._value = be64toh(value);
	#endif
	#ifdef __APPLE__
		_body._value = ntohll(value);
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

	exportedRawPkScriptLength = _body._pkScript->exportRawWithP2PKHPkScript( &exportedRawPkScript , _pubKeyHash );

	#ifdef __linux__
	_body._pkScriptBytes = be64toh(static_cast<uint64_t>(exportedRawPkScriptLength)); // 書き出しと同時に代入は怖いので
	#endif
	#ifdef __APPLE__
	_body._pkScriptBytes = ntohll(static_cast<uint64_t>(exportedRawPkScriptLength)); // 書き出しと同時に代入は怖いので
	#endif	
	//_body._pkScriptBytes = (exportedRawPkScriptLength); // 書き出しと同時に代入は怖いので

	*retRaw = std::shared_ptr<unsigned char>( new unsigned char [sizeof(_body._value) + sizeof(_body._pkScriptBytes) + pkScriptBytes()] );

	unsigned int formatPtr = 0;

	memcpy( (*retRaw).get() , &(_body._value), sizeof(_body._value) ); formatPtr+= sizeof(_body._value); // valueの書き出し

	memcpy( (*retRaw).get() + formatPtr , &(_body._pkScriptBytes) , sizeof(_body._pkScriptBytes) ); formatPtr+= sizeof(_body._pkScriptBytes); // _pkScriptBytesの書き出し

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

	memcpy( &(_body._value) , fromRaw , sizeof(_body._value) ); currentPtr += sizeof(_body._value); // valueの読み込み

	// pkScriptBytes の読み込み
	memcpy( &(_body._pkScriptBytes) , fromRaw + currentPtr , sizeof(_body._pkScriptBytes) ); currentPtr += sizeof(_body._pkScriptBytes);

	unsigned int importedPkScriptLength;
	importedPkScriptLength = _body._pkScript->importRaw( fromRaw + currentPtr , this->pkScriptBytes() ); currentPtr += importedPkScriptLength;

	_pubKeyHash = _body._pkScript->script()->at(2).second;

	return currentPtr;
}




unsigned short TxOut::value()
{
	// return ntohll(_body._value);
	#ifdef __linux__
		return be64toh(_body._value);
	#endif

	#ifdef __APPLE__
		return ntohll(_body._value);
	#endif
}

void TxOut::value( unsigned int value )
{
	_body._value = htonll(value);
}



unsigned int TxOut::pkScriptBytes()
{
	#ifdef __linux__
		return static_cast<unsigned int >(be64toh(_body._pkScriptBytes));
	#endif
	#ifdef __APPLE__
		return static_cast<unsigned int >(ntohll(_body._pkScriptBytes));
	#endif
}



void to_json( json& from , const TxOut &to )
{

}



void from_json( const json &from , TxOut &to )
{
	#ifdef __linux__
		to._body._value = be64toh(static_cast<int64_t>(from["value"])); // ビックエディアンに変換する
	#endif
	#ifdef __APPLE__
		to._body._value = ntohll(static_cast<int64_t>(from["value"])); // ビックエディアンに変換する
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







}; // close tx namespace 
