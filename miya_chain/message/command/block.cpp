#include "block.h"

#include "../../block/block_header.h"
#include "../../block/block.h"


namespace miya_chain
{



std::shared_ptr<unsigned char> MiyaChainMSG_BLOCK::blockHash()
{
    std::shared_ptr<unsigned char> blockHash;
    _block->blockHash( &blockHash );
    return blockHash;
}

std::shared_ptr<block::Block> MiyaChainMSG_BLOCK::block()
{
    return _block;
}




};