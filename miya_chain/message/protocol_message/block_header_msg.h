#ifndef C2A61C5E_C081_44D8_9F7E_84F7A5F2C905
#define C2A61C5E_C081_44D8_9F7E_84F7A5F2C905

#include <memory>


#include "../../block/block.h"



namespace miya_chain
{



struct BlockHeaderMessage
{
	unsigned char _blockHash[32];
	block::BlockHeader _blockHeader;
	uint16_t  _txCount;



// methods
	std::shared_ptr<unsigned char> exportRaw();
	unsigned char *blockHash();
	unsigned short txCount();
};







};


#endif // C2A61C5E_C081_44D8_9F7E_84F7A5F2C905



