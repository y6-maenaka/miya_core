#ifndef A74365B0_34DD_4F8D_9C6E_F2D872B5916E
#define A74365B0_34DD_4F8D_9C6E_F2D872B5916E



#include <vector>
#include <unordered_set>
#include <variant>
#include <functional>

#include "./virtual_subchain.h"


namespace block
{
  struct Block;
  struct BlockHeader;
};




namespace miya_chain
{

class VirtualSubChain;





class VirtualSubChainManager
{
private:
  std::unordered_set< VirtualSubChain, VirtualSubChain::Hash > _subchainSet; // subchain unordered set
  
  const std::shared_ptr<block::BlockHeader> _startBlockHeader;
  uint32_t _updatedAt; // 最終更新(最後にサブチェーンの延長が発生した)タイムスタンプ

  const BHPoolFinder _bhPoolFinder;
  const PBHPoolFinder _pbhPoolFinder;

  // 管理下の仮想チェーンを全て延長する
public:
  VirtualSubChainManager( std::shared_ptr<block::BlockHeader> startBlockHeader , BHPoolFinder bhPoolFinder , PBHPoolFinder pbhPoolFinder );

  void extend( bool collisionAction = false );
  void build( std::shared_ptr<block::BlockHeader> stopHeader ); // 新たに仮想チェーンを作成して,重複がなければ管理下に追加する

  
  void __printSubChainSet();
};







};



#endif // A74365B0_34DD_4F8D_9C6E_F2D872B5916E
