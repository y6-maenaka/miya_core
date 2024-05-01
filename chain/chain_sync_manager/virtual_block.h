#ifndef EBB4C6FB_9F70_428D_93C6_52054B6E3489
#define EBB4C6FB_9F70_428D_93C6_52054B6E3489


#include <memory>
#include <variant>
#include <iostream>
#include <string>

#include <node_gateway/message/message.hpp>
#include <node_gateway/message/command/inv/inv.hpp>

#include <chain/block/block.h>
#include <chain/block/block_header.h>


namespace block
{
  struct Block;
  struct BlockHeader;
}


namespace chain
{


enum class VirtualBlockState : int
{
  RAW_BlockHeader = 1,
  STORED_Block
};


struct VirtualBlock
{
private:
  std::variant< std::shared_ptr<Block>, std::shared_ptr<BlockHeader> > _body;
  int _status;
  // bool _isInLocalStrage; // ブロック本体がローカルストレージに存在するか

public:
  VirtualBlock( std::shared_ptr<BlockHeader> from );
  int status() const;
  void status( int target );
  std::shared_ptr<unsigned char> blockHash();

  std::shared_ptr<Block> block();
  void block( std::shared_ptr<Block> from, bool isStoreToLocal );
  bool compare( std::shared_ptr<unsigned char> targetBlockHash );
  
  struct inv downloadInv();
  
  void __print();
};


};


#endif // EBB4C6FB_9F70_428D_93C6_52054B6E3489
