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
  // const std::variant< std::shared_ptr<block::Block>, std::shared_ptr<block::BlockHeader> > _startBlock;
  const std::shared_ptr<block::BlockHeader> _startBlockHeader;
  uint32_t _updatedAt; // 最終更新(最後にサブチェーンの延長が発生した)タイムスタンプ

  // 管理下の仮想チェーンを全て延長する
public:
  VirtualSubChainManager( std::shared_ptr<block::BlockHeader> startBlockHeader );

  void extend( std::function<std::vector<std::shared_ptr<block::BlockHeader>>(std::shared_ptr<unsigned char>)> popCallback );
  void build( std::function<std::vector<std::shared_ptr<block::BlockHeader>>(std::shared_ptr<unsigned char>)> popCallback , std::shared_ptr<block::BlockHeader> stopHeader ); // 新たに仮想チェーンを作成して,重複がなければ管理下に追加する
};







};



#endif // A74365B0_34DD_4F8D_9C6E_F2D872B5916E
