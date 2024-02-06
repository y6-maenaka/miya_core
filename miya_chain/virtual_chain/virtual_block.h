#ifndef EBB4C6FB_9F70_428D_93C6_52054B6E3489
#define EBB4C6FB_9F70_428D_93C6_52054B6E3489

#include <memory>
#include <variant>
#include <iostream>
#include <string>

#include "../message/message.h"
#include "../message/command/inv/inv.h"


namespace block
{
  struct Block;
  struct BlockHeader;
}



namespace miya_chain
{



enum class VirtualBlockState : int
{
  RAW_BlockHeader = 1,
  STORED_Block
};





struct VirtualBlock
{

private:
  std::variant< std::shared_ptr<block::Block>, std::shared_ptr<block::BlockHeader> > _body;
  int _status;
  // bool _isInLocalStrage; // ブロック本体がローカルストレージに存在するか

public:
  VirtualBlock( std::shared_ptr<block::BlockHeader> from );
  int status() const;
  void status( int target );
  std::shared_ptr<unsigned char> blockHash();

  std::shared_ptr<block::Block> block();
  void block( std::shared_ptr<block::Block> from, bool isStoreToLocal );
  bool compare( std::shared_ptr<unsigned char> targetBlockHash );
  
  struct inv downloadInv();

};




};



#endif // EBB4C6FB_9F70_428D_93C6_52054B6E3489



