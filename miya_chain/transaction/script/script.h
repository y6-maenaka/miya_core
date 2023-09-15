#ifndef C461E28B_5551_4B4A_893A_6A7E6AE8789F
#define C461E28B_5551_4B4A_893A_6A7E6AE8789F


#include <iostream>
#include <vector>
#include <variant>
#include <memory>


namespace tx{





typedef unsigned char _OP_DUP;  constexpr _OP_DUP OP_DUP = 0x4c; // 76
typedef unsigned char _OP_HASH_160;  constexpr _OP_HASH_160 OP_HASH_160 = 0xa9; // 169 
typedef unsigned char _OP_EQUALVERIFY;  constexpr _OP_EQUALVERIFY OP_EQUALVERIFY = 0x88; // 136
typedef unsigned char _OP_CHECKSIG;  constexpr _OP_CHECKSIG OP_CHECKSIG = 0xac; // 172
typedef unsigned char _OP_DATA; 


using OP_CODES = std::variant<unsigned char,_OP_DUP, _OP_HASH_160, _OP_EQUALVERIFY, _OP_CHECKSIG, _OP_DATA>;



enum class OP_CODES_ID : int
{
	COMMON = 0,
	OP_DUP = 1 ,
	OP_HASH_160,
	OP_EQUALVERIFY,
	OP_CHECKSIG,
	OP_DATA
};


/*
class Script{


protected:
	unsigned short OPDataSize( unsigned char target );
	std::vector< std::pair< unsigned char , void *> > _script; // protectedメンバーへ
	// 

public:
																														 

	Script(){};

	// decode
	Script( unsigned char* rawScript , unsigned int rawScriptSize );
	int scriptCnt(); // getter

	// encode( export )
	unsigned int exportRaw( unsigned char** ret  ); // return retSize with unsigned int 
	unsigned int exportRawSize();

	
	int takeInScript( unsigned char* from , unsigned int fromSize );
};
*/



class Script
{
private:
	std::vector< std::pair< OP_CODES, std::shared_ptr<unsigned char> > > _script;


	unsigned int exportRawSize();

	/* ------ OP_CODE,DATA単体を書き出す ----------------------------------------------------  */	
	unsigned short exportScriptContentSize( OP_CODES opcode ); // 要素のサイズ取得
	unsigned short exportScriptContent( OP_CODES opcode ,std::shared_ptr<unsigned char> ret ); // 要素の書き出し
	/* ----------------------------------------------------------  */	


public:
	//void push_back( std::variant<_OP_DUP, _OP_HASH_160, _OP_EQUALVERIFY, _OP_CHECKSIG> OP_CODE , std::shared_ptr<unsigned char> data = nullptr );
	void pushBack( OP_CODES opcode , std::shared_ptr<unsigned char> data = nullptr );
	unsigned int OP_DATALength( OP_CODES opcode );
	
	unsigned int exportRaw( std::shared_ptr<unsigned char> retRaw );
	int importRaw( unsigned char *fromRaw , unsigned int fromRawLength );
};






}; // close tx namespace


#endif // C461E28B_5551_4B4A_893A_6A7E6AE8789F
