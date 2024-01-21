#ifndef BC4E83F6_03B7_4305_855B_8B3026429152
#define BC4E83F6_03B7_4305_855B_8B3026429152

#include <memory>


namespace block 
{
  struct Block;
  struct BlockHeader;
};




namespace miya_chain
{


struct PrevBlockHashAsKey
{
  unsigned char _prevBlockHash[32]; // アクシデントフォーク後は複数ブロックが見つかる可能性がある
  struct Hash;

  PrevBlockHashAsKey( std::shared_ptr<unsigned char> fromHash );
  PrevBlockHashAsKey( std::shared_ptr<block::BlockHeader> fromHeader );

  bool operator==(const PrevBlockHashAsKey& bh ) const;
  bool operator!=(const PrevBlockHashAsKey& bh ) const;

  std::shared_ptr<unsigned char> prevBlockHash() const;

  void __printHash() const;
};

struct PrevBlockHashAsKey::Hash {
    std::size_t operator()(const PrevBlockHashAsKey& key) const;
};



};






#endif //  BC4E83F6_03B7_4305_855B_8B3026429152
