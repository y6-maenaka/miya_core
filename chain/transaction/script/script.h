#ifndef C461E28B_5551_4B4A_893A_6A7E6AE8789F
#define C461E28B_5551_4B4A_893A_6A7E6AE8789F


#include <iostream>
#include <vector>
#include <variant>
#include <memory>

#include <unistd.h>

#include "../../../share/hash/sha_hash.h"
#include "../../../share/cipher/ecdsa_manager.h"


namespace tx{


class Script;
class ScriptStack;
class ValidationStack;
class ValidationOptions;


// 継承で作った方がいい
struct OP_COMMON{
	static constexpr unsigned char _code = 0x00; 
};

struct OP_DUP
{ 
	static constexpr unsigned char _code = 0x76; 
	static bool exe( ValidationStack *stack , ValidationOptions *optionsPtr = nullptr );
};

struct OP_HASH_160{ 
	static constexpr unsigned char _code = 0xa9; 
	static bool exe( ValidationStack *stack , ValidationOptions *optionsPtr = nullptr );
};

struct OP_EQUALVERIFY
{ 
	static constexpr unsigned char _code = 0x88; 
	static bool exe( ValidationStack *stack , ValidationOptions *optionsPtr = nullptr );
};

struct OP_CHECKSIG{ 
	static constexpr unsigned char _code = 0xac;
	static bool exe( ValidationStack *stack , ValidationOptions *optionsPtr = nullptr );
};

struct OP_DATA
{ 
	unsigned char _code = 0x00; 
	OP_DATA( unsigned char length ){ _code = length; };
	static bool exe( ValidationStack *stack , ValidationOptions *optionsPtr = nullptr );
};


constexpr OP_DUP _OP_DUP;
constexpr OP_HASH_160 _OP_HASH_160;
constexpr OP_EQUALVERIFY _OP_EQUALVERIFY;
constexpr OP_CHECKSIG _OP_CHECKSIG;




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
	unsigned char rawOPCode( OP_CODES opcode );

public:
	//void push_back( std::variant<_OP_DUP, _OP_HASH_160, _OP_EQUALVERIFY, _OP_CHECKSIG> OP_CODE , std::shared_ptr<unsigned char> data = nullptr );
	void pushBack( OP_CODES opcode , std::shared_ptr<unsigned char> data = nullptr );
	static unsigned int OP_DATALength( OP_CODES opcode );
	static unsigned int OP_DATALength( unsigned char opcode );

	unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw );
	int importRaw( unsigned char *fromRaw , unsigned int fromRawLength );

	int OPCount();
	void clear();
	std::pair< OP_CODES , std::shared_ptr<unsigned char> > at( int i );

	void print();
	size_t size();

	size_t at( size_t index ,std::shared_ptr<unsigned char> *ret );

	std::vector< std::pair< OP_CODES, std::shared_ptr<unsigned char> > >::iterator begin();
	std::vector< std::pair< OP_CODES, std::shared_ptr<unsigned char> > >::iterator end();
};


}; // close tx namespace


#endif // C461E28B_5551_4B4A_893A_6A7E6AE8789F
