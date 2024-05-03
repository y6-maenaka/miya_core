#include "script.h"


#include "script_validator.h"
#include "../../../share/hash/sha_hash.h"
#include "../../../share/cipher/ecdsa_manager.h"

namespace tx{


// std::visitでOP_CODESからコードを取得するのに必要
GetRawCode getRawCode; // st::visitの第一引数にはラムダ or 関数オブジェクトの関数型である必要があるため



bool OP_DUP::exe( ValidationStack *stack, ValidationOptions *optionsPtr )
{
	/* スタックのトップから要素を一つ取り出して複製してプッシュ */
	std::shared_ptr< std::pair<OP_CODES, std::shared_ptr<unsigned char>> > popedElem = stack->popBack(); 
	stack->pushBack( popedElem->first, popedElem->second );
	stack->pushBack( popedElem->first, popedElem->second );

	return true;
}

bool OP_HASH_160::exe( ValidationStack *stack , ValidationOptions *optionsPtr  )
{
	std::shared_ptr< std::pair<OP_CODES, std::shared_ptr<unsigned char>> > popedElem = stack->popBack(); 
	std::shared_ptr<unsigned char> md; unsigned int mdLength;
	mdLength = hash::SHAHash( popedElem->second, Script::OP_DATALength(popedElem->first) , &md , "sha1" );

	
	OP_DATA opData(0x14); // 固定長でいいでしょう
	stack->pushBack( opData, md );

	return true;
}

bool OP_EQUALVERIFY::exe( ValidationStack *stack ,ValidationOptions *optionsPtr  )
{
	std::shared_ptr< std::pair<OP_CODES, std::shared_ptr<unsigned char>> > popedElem_1 = stack->popBack(); 
	std::shared_ptr< std::pair<OP_CODES, std::shared_ptr<unsigned char>> > popedElem_2 = stack->popBack(); 

	if( memcmp( popedElem_1->second.get() , popedElem_2->second.get(), Script::OP_DATALength(popedElem_1->first) ) == 0 ) return true;

	return false;
};


bool OP_CHECKSIG::exe( ValidationStack *stack , ValidationOptions *optionsPtr ) // 検証にはトランザクションハッシュデータが必要がだが
{
	std::shared_ptr< std::pair<OP_CODES, std::shared_ptr<unsigned char>> > popedElem1 = stack->popBack(); // 正常であれば公開鍵が格納されている
	std::shared_ptr< std::pair<OP_CODES, std::shared_ptr<unsigned char>> > popedElem2 = stack->popBack();  // 正常であれば署名が格納されている

	EVP_PKEY *pubKey = NULL;
	pubKey = cipher::ECDSAManager::toPkey( popedElem1->second.get() , Script::OP_DATALength(popedElem1->first) );

	return cipher::ECDSAManager::verify( popedElem2->second, Script::OP_DATALength(popedElem2->first), optionsPtr->_txHash.first , optionsPtr->_txHash.second , pubKey ,"sha256" );
};


bool OP_DATA::exe( ValidationStack *stack , ValidationOptions *optionsPtr  )
{
	OP_DATA opdata( static_cast<unsigned char>(optionsPtr->_opData.second) );
	stack->pushBack( opdata , optionsPtr->_opData.first );

	return true;
};
















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



unsigned char Script::rawOPCode( OP_CODES opcode )
{
	if( std::holds_alternative<OP_DUP>(opcode) ) return (std::get<OP_DUP>(opcode))._code;
	if( std::holds_alternative<OP_HASH_160>(opcode) ) return (std::get<OP_HASH_160>(opcode))._code;
	if( std::holds_alternative<OP_EQUALVERIFY>(opcode) ) return (std::get<OP_EQUALVERIFY>(opcode))._code;
	if( std::holds_alternative<OP_CHECKSIG>(opcode) ) return (std::get<OP_CHECKSIG>(opcode))._code;
	if( std::holds_alternative<OP_DATA>(opcode) ) return (std::get<OP_DATA>(opcode))._code;

	else return 0x00;
}



unsigned short Script::exportScriptContentSize( OP_CODES opcode )
{

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
	unsigned char rawOPCode;
	for( auto itr : _script )
	{
		rawOPCode = this->rawOPCode( itr.first );
		memcpy( (*retRaw).get() + formatPtr ,  &rawOPCode , 1 ); formatPtr += 1;  // OP_CODEの書き出し

		if( std::holds_alternative<OP_DATA>(itr.first) )
		{
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



void Script::clear()
{
	_script.clear();
}



std::pair< OP_CODES , std::shared_ptr<unsigned char> > Script::at( int i )
{
	return _script.at(i);
}



void Script::print()
{
	for( auto itr : _script )
	{
		unsigned char op = rawOPCode(itr.first);
		printf("%02X", op );
		for( int i=0; i<Script::OP_DATALength(itr.first); i++){
			printf("%02X", itr.second.get()[i]);
		}
		std::cout << "\n\n";
	}
}

size_t Script::size()
{
	return _script.size();
}


size_t Script::at( size_t index , std::shared_ptr<unsigned char> *ret )
{
	try 
	{
		std::pair< OP_CODES , std::shared_ptr<unsigned char> > opSet = _script.at( index );
		size_t dataLength = Script::OP_DATALength(opSet.first);
		*ret = std::shared_ptr<unsigned char>(new unsigned char[dataLength] );
		memcpy( (*ret).get(), opSet.second.get() , dataLength );
		return dataLength;
    }
	 catch (const std::out_of_range& e) 
	{
		*ret = nullptr;
		return 0;
    }

}


std::vector< std::pair< OP_CODES, std::shared_ptr<unsigned char> > >::iterator Script::begin()
{
	return _script.begin();
}


std::vector< std::pair< OP_CODES, std::shared_ptr<unsigned char> > >::iterator Script::end()
{
	return _script.end();
}


}; // close tx namespace
