#ifndef D61A616F_9FF5_4380_9A34_3D1EDAC1D75F
#define D61A616F_9FF5_4380_9A34_3D1EDAC1D75F

#include <memory>

#include <node_gateway/message/message.hpp>
#include <node_gateway/message/command/getheaders.hpp>

// #include <chain/chain_sync_manager/chain_sync_manager.h>
// #include <chain/chain_sync_manager/bd_filter.h>
#include <chain/block/block_header.h>


namespace chain
{


struct BlockHashAsKey // filterのunordered_mapのキーとして使う
{
  unsigned char _blockHash[32];
  struct Hash;
 
  BlockHashAsKey( unsigned char* fromHash );
  BlockHashAsKey( std::shared_ptr<unsigned char> fromHash );
  BlockHashAsKey( std::shared_ptr<BlockHeader> fromHeader );

  bool operator==(const BlockHashAsKey& bh ) const;
  bool operator!=(const BlockHashAsKey& bh ) const;

  void printHash() const;
};

struct BlockHashAsKey::Hash {
    std::size_t operator()(const BlockHashAsKey& key) const;
};


};


#endif // D61A616F_9FF5_4380_9A34_3D1EDAC1D75F
