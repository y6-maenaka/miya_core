#ifndef A74365B0_34DD_4F8D_9C6E_F2D872B5916E
#define A74365B0_34DD_4F8D_9C6E_F2D872B5916E


#include <vector>
#include <unordered_set>
#include <variant>
#include <functional>
#include <chrono>
#include <thread>

#include "./bd_filter.h"
#include "./virtual_header_subchain.h"

#include <node_gateway/message/message.hpp>
#include <node_gateway/message/command/command_set.hpp>

#include <chain/chain_manager.h>
#include <chain/chain_sync_manager/virtual_header_subchain.h>
#include <chain/block/block.h>
#include <chain/chain_sync_manager/prev_block_hash_as_key.h>

#include <stream_buffer/stream_buffer.h>
#include <stream_buffer/stream_buffer_container.h>


namespace block
{
  struct Block;
  struct BlockHeader;
};


class StreamBufferContainer;
struct SBSegment;


namespace chain
{


enum class VirtualHeaderSyncManagerState : int
{
  WORKING = 0,
  PENDING,
  FINISHED,
  TIMEOUT = -1 ,
  ERROR = -2
};


class VirtualHeaderSubChain;

using BHPoolFinder = std::function< std::shared_ptr<BlockHeader>(std::shared_ptr<unsigned char>)>;
using PBHPoolFinder = std::function< std::vector<std::shared_ptr<BlockHeader>>(std::shared_ptr<unsigned char>)>;

constexpr unsigned short HEADER_REQUEST_TIMEOUT_SECOND = 3;
constexpr unsigned short REQUEST_COMMAND_SEND_INTERVAL = 3;


class VirtualHeaderSyncManager
{
private:
  std::unordered_set< VirtualHeaderSubChain, VirtualHeaderSubChain::Hash > _headerSubchainSet; // subchain unordered set

  const std::shared_ptr<BlockHeader> _startBlockHeader;
  uint32_t _updatedAt; // 最終更新(最後にサブチェーンの延長が発生した)タイムスタンプ
  const std::shared_ptr<unsigned char> _stopHash = nullptr;
  const FilterStateUpdator _filterStateUpdator;

  struct BlockHashPool // 一旦使用しない
  {
	std::shared_mutex _mtx;
	std::condition_variable _cv;

	std::unordered_set< BlockHashAsKey , BlockHashAsKey::Hash> _pool;

	void push( std::shared_ptr<unsigned char> target );
	std::shared_ptr<unsigned char> find( std::shared_ptr<unsigned char> blockHash );
  } _blockHashPool;

  struct BlockHeaderPool
  {
   private: // リソース効率は良くない
	std::unordered_map< BlockHashAsKey , std::shared_ptr<BlockHeader>, BlockHashAsKey::Hash > _pool_blockHsah;
	std::unordered_multimap< PrevBlockHashAsKey , std::shared_ptr<BlockHeader>, PrevBlockHashAsKey::Hash > _pool_prevBlockHash;

   public:
	std::pair<bool, short> push( std::shared_ptr<BlockHeader> target ); // (戻り値) : 重複 or 非重複

	std::shared_ptr<BlockHeader> findByBlockHash( std::shared_ptr<unsigned char> blockHash );
	std::vector< std::shared_ptr<BlockHeader> > findByPrevBlockHash( std::shared_ptr<unsigned char> prevBlockHash );
	 // 分岐によっては前ブロックを参照するブロックが複数あるケースも

	 void __printPoolSortWithBlockHash();
	 void __printPoolSortWithPrevBlockHash();
  } _blockHeaderPool;

  BHPoolFinder _bhPoolFinder;
  PBHPoolFinder _pbhPoolFinder;

  std::shared_ptr<StreamBufferContainer> _toRequesterSBC;
  int _status;

protected:
  std::pair< miya_core_command , const char* > downloadCommand();
  void sendRequestSyncedCommand();
  size_t activeSubchainCount();

  // 管理下の仮想チェーンを全て延長する
public:
  VirtualHeaderSyncManager( std::shared_ptr<BlockHeader> startBlockHeader, std::shared_ptr<unsigned char> stopHash , FilterStateUpdator filterStateUpdator , std::shared_ptr<StreamBufferContainer> toRequesterSBC );

  void add( std::vector< std::shared_ptr<BlockHeader> > targetVector );
  bool extend( bool collisionAction = false );
  void build( std::shared_ptr<BlockHeader> stopHeader ); // 新たに仮想チェーンを作成して,重複がなければ管理下に追加する

  std::vector< std::shared_ptr<VirtualHeaderSubChain> > subchainVector() const; // 全ての構築したサブチェーンを取り出す
  std::shared_ptr<VirtualHeaderSubChain> stopedHeaderSubchain(); // 延長停止した(収取目的が達成した)仮想サブチェーンを取得
  bool start();
  int status() const;

  unsigned short subchainCount();
  void __printSubChainSet();
};


/*
  最も長いor有効的なサブチェーンを選出するには
*/

};


#endif // A74365B0_34DD_4F8D_9C6E_F2D872B5916E
