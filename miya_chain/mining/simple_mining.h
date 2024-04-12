#ifndef F58F7A54_8BAB_48D3_B7BD_DB006039D446
#define F58F7A54_8BAB_48D3_B7BD_DB006039D446



#include <iostream>
#include <memory>
#include <bitset>
#include <array>
#include <unistd.h>


#include <stdio.h>
#include <stdlib.h>
#ifdef __linux__
	#include <endian.h>
#endif
#ifdef __APPLE__
	#include <machine/endian.h>
#endif 



#include "../block/block.h"
#include "../../share/hash/sha_hash.h"



namespace miya_chain
{






bool mbitcmp( unsigned char* rawNBitMask , unsigned char* hash );

std::array<uint8_t,32> generatenBitMask( uint32_t nBits );

// bool verifyBlockHeader( block::BlockHeader *target );

std::array<uint8_t,32> generatenBitMask( uint32_t nBits );

uint32_t simpleMining( uint32_t nBits , std::shared_ptr<block::BlockHeader> blockHeader , bool showHistory = true );


};





#endif // 

