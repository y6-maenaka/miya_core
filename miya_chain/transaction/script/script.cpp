#include "script.h"


namespace tx{


// std::visitでOP_CODESからコードを取得するのに必要
GetRawCode getRawCode; // st::visitの第一引数にはラムダ or 関数オブジェクトの関数型である必要があるため



void Script::pushBack( OP_CODES opcode , std::shared_ptr<unsigned char> data )
{
	if( std::holds_alternative<OP_DATA>(opcode) )
		_script.push_back( std::make_pair( opcode, data ) );
	else
		_script.push_back( std::make_pair( opcode , nullptr ) );
}




unsigned int Script::OP_DATALength( OP_CODES opcode )
{
	//unsigned int ret = 0;
	//ret = static_cast<unsigned short>(std::get<static_cast<int>(OP_CODES_ID::COMMON)>(opcode));

	if( std::holds_alternative<OP_DATA>(opcode) )
		return static_cast<unsigned int>( std::get<OP_DATA>(opcode)._code);

	//if( ret >= 1 && ret <= 95 ) return ret;
	return 0;
}


unsigned int Script::OP_DATALength( unsigned char opcode )
{
	unsigned int ret = 0;
	ret = static_cast<unsigned short>(opcode);

	if( ret >= 1 && ret <= 95 ) return ret;
	return 0;
}


OP_CODES Script::parseRawOPCode( unsigned char rawOPCode ) // メモリ効率があまり良くない実装 要修正
{

	switch( static_cast<unsigned short>(rawOPCode) )
	{
		case static_cast<unsigned short>(_OP_DUP._code):
			return _OP_DUP;
		case static_cast<unsigned short>(_OP_HASH_160._code):
			return _OP_HASH_160;
		case static_cast<unsigned short>(_OP_EQUALVERIFY._code):
			return _OP_EQUALVERIFY;
		case static_cast<unsigned short>(_OP_CHECKSIG._code):
			return _OP_CHECKSIG;
		default:
			OP_DATA ret(rawOPCode);
			return ret;
	}
}



unsigned short Script::exportScriptContentSize( OP_CODES opcode )
{
	/*
	if( opcode.index() == static_cast<int>(OP_CODES_ID::OP_DATA) ) // opcodeがOP_DATAだった場合は格納しているデータのバイト長を返却する
		//return static_cast<unsigned short>(std::get<static_cast<int>(OP_CODES_ID::OP_DATA)>(opcode)) + 1;
		return static_cast<unsigned short>(std::get<OP_DATA>(opcode)._length);
	*/

	if( std::holds_alternative<OP_DATA>(opcode) )
		return this->OP_DATALength(opcode) + 1;

	return 1;
}




unsigned int Script::exportRawSize()
{
	unsigned ret = 0;

	for( auto itr : _script )
		ret += exportScriptContentSize(itr.first);

	return ret;
}


unsigned int Script::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	if( _script.size() <= 0 ) // OP_CODEが挿入されていない場合はリターン
	{
		retRaw = nullptr; return 0;
	}


	unsigned int retRawLength = exportRawSize(); // 全体書き出しサイズの取得
	//*retRaw = std::make_shared<unsigned char>( retRawLength );
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[retRawLength] );


	unsigned int formatPtr = 0;
	unsigned char temp;
	for( auto itr : _script )
	{
		temp = std::visit( getRawCode , itr.first );
		memcpy( (*retRaw).get() + formatPtr ,  &temp , 1 ); formatPtr += 1; 

		if( std::holds_alternative<OP_DATA>(itr.first) ){
			
			memcpy( (*retRaw).get() + formatPtr , itr.second.get() , OP_DATALength(itr.first) ); formatPtr += OP_DATALength(itr.first);
		}

	}
	
	return formatPtr;
}



int Script::importRaw( unsigned char *fromRaw , unsigned int fromRawLength )
{
	// 先頭の1bytesを取得する
	int i=0;
	unsigned short opDataLength; 
	OP_CODES opcode; unsigned char rawOPCode;
	unsigned short OPDataLength;

	while( i < fromRawLength ) // あってる？
	{
		rawOPCode = fromRaw[i];
		opcode = parseRawOPCode( rawOPCode );
	

		if( (OPDataLength =  OP_DATALength(opcode)) > 0 ){ // データが挿入されている場合　
			i+=1; // OP_CODE文進める
			std::shared_ptr<unsigned char> opdata = std::shared_ptr<unsigned char>( new unsigned char[OPDataLength] );
			memcpy( opdata.get() , fromRaw + i , OPDataLength ); // 怖いのコピーしたものを格納する
			this->pushBack( opcode , opdata );
			i += OPDataLength;
		}else{
			this->pushBack( opcode );
			i+=1;
		}
	}
	
	return i;
}



int Script::OPCount()
{
	return _script.size();
}



std::pair< OP_CODES , std::shared_ptr<unsigned char> > Script::at( int i )
{
	return _script.at(i);
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
