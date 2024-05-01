#include "block.hpp"
#include <chain/block/block.h>


namespace chain
{


std::shared_ptr<unsigned char> MiyaCoreMSG_BLOCK::blockHash()
{
    std::shared_ptr<unsigned char> blockHash;
    _block->blockHash( &blockHash );
    return blockHash;
}

std::shared_ptr<struct Block> MiyaCoreMSG_BLOCK::block()
{
    return _block;
}

std::vector<std::uint8_t> MiyaCoreMSG_BLOCK::export_to_binary() const
{

}


};
