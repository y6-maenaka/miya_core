#include "script.h"


namespace tx{



int Script::scriptCnt()
{
	return _script.size();
}





unsigned int Script::exportRaw( unsigned char **ret  )
{
	unsigned int pos = 0;
	*ret = new unsigned char[ exportRawSize() ];

	for( auto itr = _script.cbegin(); itr != _script.cend(); itr++ )
	{
		memcpy( *ret + pos , &(itr->first) , 1 );  pos += 1; // OP_CODE のエクスポート

		if( OPDataSize(itr->first) > 1 ){
			memcpy( *ret + pos , itr->second , OPDataSize(itr->first) );    pos += OPDataSize( itr->first ); // OP_DATA の書き出し
		}

	}

	return exportRawSize();
};






unsigned int Script::exportRawSize()
{

	unsigned int size = 0;

	for( auto itr = _script.cbegin() ; itr != _script.cend() ; itr++ ){
		if( OPDataSize(itr->first) >= 1 ) size += OPDataSize( itr->first );
		size += 1 ;
	}

	return size;
};






unsigned short Script::OPDataSize( unsigned char target )
{
	unsigned short ret = static_cast<unsigned short>(target);

	if( ret >= 1 && ret <= 95 ) return ret; // OP_SIZE_CODE + OP_DATA
	// if( ret >= 1 && ret <= 75 ) return ret; 本来のサイズ　DERエンコードしていないためRawPubKeyが収まらない
									
	return 0;
}




int Script::takeInScript( unsigned char* from , unsigned int fromSize )
{

	unsigned int pos = 0;
	unsigned char OPCode; 
	unsigned short _OPDataSize = 0; // メソッドメイト被るため

	// 本当はScriptにscript_bytesを持たせて ここでscript_bytesも一気に処理する方がまとまる
	
	
	while( pos < fromSize ){
		OPCode = from[pos];  pos += 1;
		
		_OPDataSize = OPDataSize( OPCode );

		if( _OPDataSize >= 1 )
		{	
			unsigned char* OPData = new unsigned char[ _OPDataSize ];
			memcpy( OPData , from + pos , _OPDataSize ); pos += _OPDataSize;

			_script.push_back( std::make_pair(OPCode, OPData ) );
			continue;
			//push_back
		}
		_script.push_back( std::make_pair( OPCode, &OPCode) );
		// push_back
	}



	// ここが確実にScriptの最後でないといけない
	return pos;

};



}; // close tx namespace
