#ifndef D2821037_69F4_4A6D_A17A_68F6E4EE92E5
#define D2821037_69F4_4A6D_A17A_68F6E4EE92E5


#include <memory>
#include <vector>
#include <variant>
#include <string>
#include <shared_mutex>
#include <mutex>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <chrono>

#include "../message/message.h"
#include "../message/command/getblocks.h"
#include "./chain_sync_manager.h"

namespace block
{
  struct Block;
  struct BlockHeader;
}





namespace miya_chain
{



constexpr unsigned short STATE_HASH_LENGTH = (160 / 8);
constexpr unsigned short SUBCHAIN_LIFETIME = 7;


class VirtualHeaderSubChain
{

private:
  // std::shared_mutex _mtx;
  // std::vector< std::shared_ptr<block::BlockHeader> >  _chain;
  // std::vector< std::variant< std::shared_ptr<block::Block> , std::shared_ptr<block::BlockHeader> > > _chain; // 仮想チェーンにはブロックヘッダもしくはブロックが格納される
  struct Context
  {
	const std::shared_ptr<block::BlockHeader> _startBlockHeader;
	std::shared_ptr<block::BlockHeader> _latestBlockHeader;
  
	Context( std::shared_ptr<block::BlockHeader> startBlockHeader );
	std::shared_ptr<unsigned char> latestBlockHash();
  } _context;

  uint32_t _updatedAt; // チェーン最後尾が更新された時の時間
  bool _isStoped = false;
  const std::shared_ptr<unsigned char> _stopHash = nullptr;

  void syncUpdatedAt();
 
  const BHPoolFinder _bhPoolFinder;
  const PBHPoolFinder _pbhPoolFinder;

public: 
  VirtualHeaderSubChain( std::shared_ptr<block::BlockHeader> startBlockHeader, std::shared_ptr<unsigned char> stopHash , BHPoolFinder bhPoolFinder , PBHPoolFinder pbhPoolFinder );

  size_t chainDigest( std::shared_ptr<unsigned char> *ret ) const;
  std::shared_ptr<unsigned char> chainDigest() const;
  std::shared_ptr<block::BlockHeader> latestBlockHeader(); // あまり使用しない

  bool operator==( const VirtualHeaderSubChain &sc ) const;
  bool operator!=( const VirtualHeaderSubChain &sc ) const;
  struct Hash;

  bool isStoped() const;  // チェーンがStopHashに到達しているか否か
  bool isActive();

  uint32_t updatedAt() const;
  void build( std::shared_ptr<block::BlockHeader> latestBlockHeader );
  bool extend( int collisionAction = 0 ); // stopHashのチェーンに達したら延長を打ち切る
  MiyaChainMSG_GETBLOCKS extendCommand(); // 本サブチェーンにつながるようなブロックリクエストコマンドを発行する

  std::vector< std::shared_ptr<block::BlockHeader> > exportChainVector(); // latestBlockHeaderから遡ってチェーンを生成する

  void __printChainDigest() const;
};



struct VirtualHeaderSubChain::Hash
{
  std::size_t operator()(const VirtualHeaderSubChain &sc ) const;
};




};

#endif // D2821037_69F4_4A6D_A17A_68F6E4EE92E5
