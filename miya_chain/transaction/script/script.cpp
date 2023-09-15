#include "script.h"


namespace tx{




void Script::pushBack( OP_CODES opcode , std::shared_ptr<unsigned char> data )
{
	_script.push_back( std::make_pair( opcode, data ) );
}




unsigned int OP_DATALength( OP_CODES opcode )
{
	unsigned int ret = 0;
	ret = static_cast<unsigned short>(std::get<static_cast<int>(OP_CODES_ID::COMMON)>(opcode));

	if( ret >= 1 && ret <= 95 ) return ret;
	return 0;
}



unsigned short Script::exportScriptContentSize( OP_CODES opcode )
{
	if( opcode.index() == static_cast<int>(OP_CODES_ID::OP_DATA) ) // opcodeがOP_DATAだった場合は格納しているデータのバイト長を返却する
		return static_cast<unsigned short>(std::get<static_cast<int>(OP_CODES_ID::OP_DATA)>(opcode)) + 1;

	return 1;
}







unsigned int Script::exportRawSize()
{
	unsigned ret = 0;

	for( auto itr : _script )
		ret += exportScriptContentSize(itr.first);

	return ret;
}


unsigned int Script::exportRaw( std::shared_ptr<unsigned char> retRaw )
{
	if( _script.size() <= 0 ) // OP_CODEが挿入されていない場合はリターン
	{
		retRaw = nullptr; return 0;
	}

	unsigned int retRawLength = exportRawSize(); // 全体書き出しサイズの取得
	retRaw = std::make_shared<unsigned char>( retRawLength );

	unsigned int formatPtr = 0;
	for( auto itr : _script )
	{
		memcpy( retRaw.get() + formatPtr , (itr.second).get() , exportScriptContentSize(itr.first) ); formatPtr+= exportScriptContentSize(itr.first);
	}
	
	return formatPtr;
}



int Script::importRaw( unsigned char *fromRaw , unsigned int fromRawLength )
{
	OP_CODES opcode;

	for( int i=0; i<fromRawLength; )
	{
		unsigned char rawOpcode;
		memcpy( &rawOpcode , fromRaw + i, 1);  i+=1; // OPCODEの取得
		opcode.emplace<static_cast<int>(OP_CODES_ID::COMMON)>(rawOpcode);
	
		if( OP_DATALength(opcode) > 0 ){
			pushBack( opcode , std::make_shared<unsigned char>(*(fromRaw+i)) ); i+=OP_DATALength(opcode);
		}
		else
			pushBack( opcode, std::make_shared<unsigned char>(rawOpcode) ); // ポインタは進めない
	}

	return _script.size();
}

/*
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
*/





}; // close tx namespace
