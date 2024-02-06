#ifndef A74365B0_34DD_4F8D_9C6E_F2D872B5916E
#define A74365B0_34DD_4F8D_9C6E_F2D872B5916E



#include <vector>
#include <unordered_set>
#include <variant>
#include <functional>
#include <chrono>
#include <thread>

#include "./virtual_header_subchain.h"


namespace block
{
  struct Block;
  struct BlockHeader;
};


class StreamBufferContainer;
struct SBSegment;


namespace miya_chain
{

class VirtualHeaderSubChain;





class VirtualHeaderSyncManager
{
private:
  std::unordered_set< VirtualHeaderSubChain, VirtualHeaderSubChain::Hash > _headerSubchainSet; // subchain unordered set
  
  const std::shared_ptr<block::BlockHeader> _startBlockHeader;
  uint32_t _updatedAt; // 最終更新(最後にサブチェーンの延長が発生した)タイムスタンプ
  const std::shared_ptr<unsigned char> _stopHash = nullptr;

  const BHPoolFinder _bhPoolFinder;
  const PBHPoolFinder _pbhPoolFinder;

  std::shared_ptr<StreamBufferContainer> _toRequesterSBC;

  // 管理下の仮想チェーンを全て延長する
public:
  VirtualHeaderSyncManager( std::shared_ptr<block::BlockHeader> startBlockHeader, std::shared_ptr<unsigned char> stopHash ,BHPoolFinder bhPoolFinder , PBHPoolFinder pbhPoolFinder , std::shared_ptr<StreamBufferContainer> toRequesterSBC );

  bool extend( bool collisionAction = false );
  void build( std::shared_ptr<block::BlockHeader> stopHeader ); // 新たに仮想チェーンを作成して,重複がなければ管理下に追加する

  std::shared_ptr<VirtualHeaderSubChain> stopedHeaderSubchain();
  
  unsigned short subchainCount();
  void __printSubChainSet();
};







};



#endif // A74365B0_34DD_4F8D_9C6E_F2D872B5916E
