#include "virtual_block.h"

#include "../block/block.h"
#include "../block/block_header.h"


namespace miya_chain
{



VirtualBlock::VirtualBlock( std::shared_ptr<block::BlockHeader> from )
{
  _body = from;
  _status = static_cast<int>(VirtualBlockState::RAW_BlockHeader);
  return;
}

std::shared_ptr<unsigned char> VirtualBlock::blockHash()
{
  if( std::holds_alternative<std::shared_ptr<block::Block>>(_body) ) return std::get<std::shared_ptr<block::Block>>(_body)->blockHash();
  if( std::holds_alternative<std::shared_ptr<block::BlockHeader>>(_body) ) return std::get<std::shared_ptr<block::BlockHeader>>(_body)->headerHash();
}

int VirtualBlock::status() const
{
  return _status;
}

void VirtualBlock::status( int target )
{
  _status = target;
}

std::shared_ptr<block::Block> VirtualBlock::block()
{
  if( std::holds_alternative<std::shared_ptr<block::Block>>(_body) ) return std::get<std::shared_ptr<block::Block>>( _body );
  return nullptr;
}

void VirtualBlock::block( std::shared_ptr<block::Block> from, bool isStoreToLocal )
{
  _body = from;

  if( isStoreToLocal )
  {
	return;
  }
}

bool VirtualBlock::compare( std::shared_ptr<unsigned char> targetBlockHash ) 
{
  if( std::holds_alternative<std::shared_ptr<block::Block>>(_body) )
	return std::get<std::shared_ptr<block::Block>>(_body)->compare(targetBlockHash);
  
  return false;
}


struct inv VirtualBlock::downloadInv()
{
 return inv( std::string("BLOCK") , this->blockHash() );
}

void VirtualBlock::__print()
{
  std::cout << " ==== [ VirtualBlock ] ==== "  << "\n";
  std::shared_ptr<unsigned char> blockHash = this->blockHash();
  for( int i=0; i<32; i++ ){
	printf("%02X", blockHash.get()[i]);
  } std::cout << "\n";
  std::cout << "==============================" << "\n";
}




};
