#ifndef FA2712E0_8314_41BD_818A_32C8A01AC25F
#define FA2712E0_8314_41BD_818A_32C8A01AC25F


#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <functional>
#include <cassert>
#include <chrono>
#include <iostream>
#include <variant>

#include <unistd.h>
#include <map>

#include "./BDBCB.h"
#include "../block_chain_iterator/block_chain_iterator.h"
#include "./bd_filter.h"

#include "./block_hash_as_key.h"
#include "./prev_block_hash_as_key.h"

#include "../message/message.h"


namespace block
{
struct Block;
struct BlockHeader;
};

class StreamBufferContainer;
struct SBSegment;




namespace miya_chain
{


class BDFilter;
class BDVirtualChain;
class LightUTXOSet;
class BlockLocalStrageManager;
class VirtualSubChainManager;
class VirtualHeaderSyncManager;
class VirtualBlockSyncManager;



constexpr unsigned int DEFAULT_BLOCK_HEADER_DOWNLOAD_AGENT_COUNT = 1; // メインスレッドと切り離してブロックヘッダーの受信を担当するスレッド個数
constexpr unsigned int DEFAULT_BLOCK_DOWNLOAD_AGENT_COUNT = 3; // ブロックダウンロードエージェントの起動数
constexpr unsigned int DEFAULT_DOWNLOAD_BLOCK_WINDOW_SIZE = 10; // 一つのブロックダウンロードエージェントが一回で担当するブロック数
constexpr unsigned int DEFAULT_BD_MAX_TIMEOUT_COUNT = 4;

// first: blockHash , second: ブロック管理ブロック
using VirtualMiyaChain = std::vector< std::pair< std::shared_ptr<unsigned char>, std::shared_ptr<struct BDBCB>> >;
using BHPoolFinder = std::function< std::shared_ptr<block::BlockHeader>(std::shared_ptr<unsigned char>)>;
using PBHPoolFinder = std::function< std::vector<std::shared_ptr<block::BlockHeader>>(std::shared_ptr<unsigned char>)>;


enum class VirtualChainState : int
{
  WORKING = 0,
  PENDING,
  FINISHED,
  HEADER_SYNC_MANAGER_WORKING,
  HEADER_SYNC_MANAGER_DONE,
  BLOCK_SYNC_MANAGER_WORKING,
  BLOCK_SYNC_MANAGER_DONE,
  TIMEOUT = -1 ,
  ERROR = -2
};







class VirtualChain
{
private:
  std::variant< std::shared_ptr<block::Block>, std::shared_ptr<block::BlockHeader> > _objectiveBlock; // 目的ブロック(このブロックまで仮想チェーンを構築する)
  BlockChainIterator &_forkPoint; // フォーク分岐点 下がることもある
  std::shared_ptr< LightUTXOSet > _utxoSet;
  std::shared_ptr<StreamBufferContainer> _toRequesterSBC;
  int _status;

  std::shared_ptr< BDFilter > _filter; // フィルター本体

  struct
  {
	   std::shared_ptr< VirtualHeaderSyncManager > _headerSyncManager;
	    std::shared_ptr< VirtualBlockSyncManager > _blockSyncManager;
  } _syncManager;


public:
  VirtualChain( BlockChainIterator initialForkPoint , std::shared_ptr<StreamBufferContainer> toRequesterSBC );

  void forward(); // 目的(最終)を指定せず(自動的に最終を取得)して,それに達するように仮想チェーンを構築する ※ IBDなど
  void backward( std::shared_ptr<block::BlockHeader> objectiveHeader ); // 目的(最終)を指定して,それに達するように仮想チェーンを構築する ※ 他ノードが新たにブロックを発掘した時など

  // void add( std::shared_ptr<block::BlockHeader> target ); // 外部ノードから到着したブロックヘッダー
  void add( std::vector<std::shared_ptr<block::BlockHeader>> targetVector ); // headersレスポンスなどが帰ってくる場合
  void add( std::vector<std::shared_ptr<block::Block>> targetVector );
  void add( std::shared_ptr<block::Block> targetBlock );

  void __printState();
};












};



#endif
