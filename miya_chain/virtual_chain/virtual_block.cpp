#include "virtual_block.h"

#include "../block/block.h"
#include "../block/block_header.h"


namespace miya_chain
{



VirtualBlock::VirtualBlock( std::shared_ptr<block::BlockHeader> from )
{
  _body = from;
  return;
}

std::shared_ptr<unsigned char> VirtualBlock::blockHash()
{
  if( std::holds_alternative<std::shared_ptr<block::Block>>(_body) ) return std::get<std::shared_ptr<block::Block>>(_body)->blockHash();
  if( std::holds_alternative<std::shared_ptr<block::BlockHeader>>(_body) ) return std::get<std::shared_ptr<block::BlockHeader>>(_body)->headerHash();
}

struct inv VirtualBlock::downloadInv()
{

  struct inv ret = inv( std::string("BLOCK") , this->blockHash() );
  return ret;
}




};
