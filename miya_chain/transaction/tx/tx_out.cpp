#include "tx_out.h"

#include "../script/pk_script.h"

namespace tx{




unsigned short TxOut::exportRaw( std::shared_ptr<unsigned char> retRaw )
{
	retRaw = std::make_shared<unsigned char>( sizeof(_body._value) + sizeof(_body._pkScriptBytes) + _body._pkScriptBytes );

	unsigned int formatPtr = 0;
	memcpy( retRaw.get() , &(_body._value), sizeof(_body._value) ); formatPtr+= sizeof(_body._value);
	memcpy( retRaw.get() , &(_body._pkScriptBytes) , sizeof(_body._pkScriptBytes) ); formatPtr+= sizeof(_body._pkScriptBytes);

	
	std::shared_ptr<unsigned char> exportedPkScript; unsigned int exportedPkScriptLength = 0;

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
