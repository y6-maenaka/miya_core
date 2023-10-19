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
	_body._value = be64toh(value);
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

	_body._pkScriptBytes = be64toh(static_cast<uint64_t>(exportedRawPkScriptLength)); // 書き出しと同時に代入は怖いので
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
	return be64toh(_body._value);
}




unsigned int TxOut::pkScriptBytes()
{
	return static_cast<unsigned int >(be64toh(_body._pkScriptBytes));
}



void to_json( json& from , const TxOut &to )
{

}



void from_json( const json &from , TxOut &to )
{
	to._body._value = be64toh(static_cast<int64_t>(from["value"])); // ビックエディアンに変換する

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





/*
unsigned int TxOut::exportRaw( unsigned char **ret )
{
	if( _pkScript == NULL ) return 0;

	*ret = new unsigned char[ exportRawSize() ];
	unsigned int pos = 0;
	unsigned char* rawPkScriptBuff; unsigned int rawPkScriptBuffSize = 0;

	// 先駆けて書き出し
	rawPkScriptBuffSize =_pkScript->exportRaw( &rawPkScriptBuff );

	memcpy( *ret , &_value , sizeof(_value) );                      pos += sizeof(_value);

	_pkScriptBytes = htonl( rawPkScriptBuffSize );
	memcpy( *ret + pos , &_pkScriptBytes , sizeof(_pkScriptBytes) );      pos += sizeof(_pkScriptBytes);
	memcpy( *ret  + pos , rawPkScriptBuff , _pkScript->exportRawSize() );  // pos += _pkScript->exportRawSize();

	return exportRawSize();
};



unsigned int TxOut::autoTakeInPkScript( unsigned char* from )
{
	unsigned int pos = 0;

	// 処理的にはTxInのものと同じ
	memcpy( &_pkScriptBytes , from , sizeof(_pkScriptBytes) ); pos += sizeof(_pkScriptBytes); // pkScriptSizeの取り込み
	pos += _pkScript->takeInScript( from + pos  , pkScriptBytes() ) ;

	return pos;

}



unsigned int TxOut::pkScriptBytes()
{
	return ntohl( _pkScriptBytes );
}


unsigned int TxOut::exportRawSize()
{
	return (sizeof( _value ) + sizeof( _pkScriptBytes ) + _pkScript->exportRawSize()) ;
}


TxOut::TxOut()
{
	PkScript *pkScript = new PkScript;
	_pkScript = pkScript;
}



TxOut::TxOut( unsigned int value ){
	_value = htonll( value );

	PkScript *pkScript = new PkScript;
	_pkScript = pkScript;
}
*/

}; // close tx namespace 
