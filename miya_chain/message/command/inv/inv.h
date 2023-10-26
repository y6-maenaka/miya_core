#ifndef D958EFE4_86C9_487B_B8B9_71C83369D194
#define D958EFE4_86C9_487B_B8B9_71C83369D194


#include <iostream>
#include <vector>


// https://en.bitcoin.it/wiki/Protocol_documentation#inv

namespace miya_chain
{





enum class TypeID
{
	MSG_TX = 1,
	MSG_BLOCK,
};




struct inv
{
	struct _typeID; 
	unsigned char _hash[32];
};



struct MiyaChainMSG_INV
{
	uint32_t _count;
	std::vector<struct inv> _invVector;

public:
	static constexpr unsigned char command[12] = "inv";
};






};


#endif // D958EFE4_86C9_487B_B8B9_71C83369D194



