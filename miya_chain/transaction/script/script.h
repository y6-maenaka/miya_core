#ifndef C461E28B_5551_4B4A_893A_6A7E6AE8789F
#define C461E28B_5551_4B4A_893A_6A7E6AE8789F


#include <iostream>
#include <vector>
#include <variant>
#include <memory>

#include <unistd.h>


namespace tx{

// 継承で作った方がいい
struct OP_COMMON{ static constexpr unsigned char _code = 0x00; };
struct OP_DUP{ static constexpr unsigned char _code = 0x76; };
struct OP_HASH_160{ static constexpr unsigned char _code = 0xa9; };
struct OP_EQUALVERIFY{ static constexpr unsigned char _code = 0x88; };
struct OP_CHECKSIG{ static constexpr unsigned char _code = 0xac; };
struct OP_DATA
{ 
	unsigned char _code = 0x00; 
	OP_DATA( unsigned char length ){ _code = length; };
};


constexpr OP_DUP _OP_DUP;
constexpr OP_HASH_160 _OP_HASH_160;
constexpr OP_EQUALVERIFY _OP_EQUALVERIFY;
constexpr OP_CHECKSIG _OP_CHECKSIG;




/*
typedef unsigned char _OP_DUP;  constexpr _OP_DUP OP_DUP = 0x76; // 76
typedef unsigned char _OP_HASH_160;  constexpr _OP_HASH_160 OP_HASH_160 = 0xa9; // 169 
typedef unsigned char _OP_EQUALVERIFY;  constexpr _OP_EQUALVERIFY OP_EQUALVERIFY = 0x88; // 136
typedef unsigned char _OP_CHECKSIG;  constexpr _OP_CHECKSIG OP_CHECKSIG = 0xac; // 172
typedef unsigned char _OP_DATA; 
*/

using OP_CODES = std::variant < OP_COMMON , OP_DUP, OP_HASH_160, OP_EQUALVERIFY, OP_CHECKSIG, OP_DATA>;



struct GetRawCode {
template <typename T>
	unsigned char operator()(T& op) const {
		return op._code;
	}
};
				





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

	OP_CODES parseRawOPCode( unsigned char rawOPCode );

public:
	//void push_back( std::variant<_OP_DUP, _OP_HASH_160, _OP_EQUALVERIFY, _OP_CHECKSIG> OP_CODE , std::shared_ptr<unsigned char> data = nullptr );
	void pushBack( OP_CODES opcode , std::shared_ptr<unsigned char> data = nullptr );
	unsigned int OP_DATALength( OP_CODES opcode );
	unsigned int OP_DATALength( unsigned char opcode );
	
	unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw );
	int importRaw( unsigned char *fromRaw , unsigned int fromRawLength );

	int OPCount();
	std::pair< OP_CODES , std::shared_ptr<unsigned char> > at( int i );
};






}; // close tx namespace


#endif // C461E28B_5551_4B4A_893A_6A7E6AE8789F
