#ifndef F58F7A54_8BAB_48D3_B7BD_DB006039D446
#define F58F7A54_8BAB_48D3_B7BD_DB006039D446



#include <iostream>
#include <memory>
#include <bitset>


namespace block
{
	struct Block;
};



namespace miya_chain
{

uint32_t simpleMining( uint32_t nBits , std::shared_ptr<block::Block> block );
bool mbitcmp( unsigned char *nonceMask , std::shared_ptr<unsigned char> hash );



};

#endif // 

