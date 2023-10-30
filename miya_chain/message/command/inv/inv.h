#ifndef D958EFE4_86C9_487B_B8B9_71C83369D194
#define D958EFE4_86C9_487B_B8B9_71C83369D194


#include <iostream>
#include <vector>
#include <memory>


// https://en.bitcoin.it/wiki/Protocol_documentation#inv

namespace miya_chain
{





enum class TypeID 
{
	MSG_TX = 1,
	MSG_BLOCK,
};







// アクセスフリー
struct inv 
{
	unsigned short _typeID = 0; 
	unsigned char _hash[32];

	void hash( std::shared_ptr<unsigned char> target );
	unsigned char* hash();
};






struct MiyaChainMSG_INV
{
	struct 
	{
		uint32_t _count;
		std::vector<struct inv> _invVector;
	} _body;

protected:
	void add( struct inv target );

public:
	static constexpr char command[12] = "inv";

	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );

	/* Getter */ 
	size_t count();
	
	/* Setter */
	void count( size_t count );
	void addTx( std::shared_ptr<unsigned char> hash );
	void addBlock( std::shared_ptr<unsigned char> hash );

	std::vector<struct inv> invVector();
};






};


#endif // D958EFE4_86C9_487B_B8B9_71C83369D194



