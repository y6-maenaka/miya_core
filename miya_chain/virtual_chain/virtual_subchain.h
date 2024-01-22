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
#include "./virtual_chain.h"

namespace block
{
  struct Block;
  struct BlockHeader;
}





namespace miya_chain
{



constexpr unsigned short STATE_HASH_LENGTH = (160 / 8);


class VirtualSubChain
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
  void update();
  const std::shared_ptr<unsigned char> _stopHash = nullptr;
 
  const BHPoolFinder _bhPoolFinder;
  const PBHPoolFinder _pbhPoolFinder;

public: 
  VirtualSubChain( std::shared_ptr<block::BlockHeader> startBlockHeader, std::shared_ptr<unsigned char> stopHash , BHPoolFinder bhPoolFinder , PBHPoolFinder pbhPoolFinder );

  size_t chainDigest( std::shared_ptr<unsigned char> *ret ) const;
  std::shared_ptr<unsigned char> chainDigest() const;
  std::shared_ptr<block::BlockHeader> latestBlockHeader(); // あまり使用しない

  bool operator==( const VirtualSubChain &sc ) const;
  bool operator!=( const VirtualSubChain &sc ) const;
  struct Hash;

  uint32_t updatedAt() const;
  void build( std::shared_ptr<block::BlockHeader> latestBlockHeader );
  void extend( int collisionAction = 0 ); // stopHashのチェーンに達したら延長を打ち切る
  MiyaChainCommand extendCommand(); // 本サブチェーンにつながるようなブロックリクエストコマンドを発行する
  
  std::vector< std::shared_ptr<block::BlockHeader> > exportChainVector(); // latestBlockHeaderから遡ってチェーンを生成する


  void __printChainDigest() const;
};



struct VirtualSubChain::Hash
{
  std::size_t operator()(const VirtualSubChain &sc ) const;
};




};

#endif // D2821037_69F4_4A6D_A17A_68F6E4EE92E5
