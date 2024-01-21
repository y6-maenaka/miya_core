#ifndef D61A616F_9FF5_4380_9A34_3D1EDAC1D75F
#define D61A616F_9FF5_4380_9A34_3D1EDAC1D75F

#include <memory>

#include "../message/message.h"
#include "../message/command/getheaders.h"

namespace miya_chain
{



struct BlockHashAsKey // filterのunordered_mapのキーとして使う
{
  unsigned char _blockHash[32];
  struct Hash;
 
  BlockHashAsKey( unsigned char* fromHash );
  BlockHashAsKey( std::shared_ptr<unsigned char> fromHash );
  BlockHashAsKey( std::shared_ptr<block::BlockHeader> fromHeader );

  bool operator==(const BlockHashAsKey& bh ) const;
  bool operator!=(const BlockHashAsKey& bh ) const;

  void printHash() const;
};

struct BlockHashAsKey::Hash {
    std::size_t operator()(const BlockHashAsKey& key) const;
};





};



#endif // D61A616F_9FF5_4380_9A34_3D1EDAC1D75F
