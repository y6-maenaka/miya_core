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





struct VirtualBlock
{

private:
  std::variant< std::shared_ptr<block::Block>, std::shared_ptr<block::BlockHeader> > _body;
  int _status;

  std::shared_ptr<unsigned char> blockHash();

public:
  VirtualBlock( std::shared_ptr<block::BlockHeader> from );
  
  struct inv downloadInv();

};




};



#endif // EBB4C6FB_9F70_428D_93C6_52054B6E3489



