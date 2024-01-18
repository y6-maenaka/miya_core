#include "virtual_subchain_manager.h"

#include "./virtual_subchain.h"
#include "../block/block.h"




namespace miya_chain
{



VirtualSubChainManager::VirtualSubChainManager( std::shared_ptr<block::BlockHeader> startBlockHeader ) : _startBlockHeader(startBlockHeader)
{
  return;
}

void VirtualSubChainManager::extend( std::function<std::vector<std::shared_ptr<block::BlockHeader>>(std::shared_ptr<unsigned char>)> popCallback )
{
  std::vector< VirtualSubChain > extendedVector;
  for( auto itr : _subchainSet )
	extendedVector.push_back( itr );

  for( auto itr : extendedVector )
  {
	itr.extend( popCallback );  // 仮想チェーンを延長する
  }

  // 延長したsubChainで入れ替える 延長した結果仮想チェーンが重複した場合は削除する
  _subchainSet.clear();
  for( auto itr : extendedVector )
	_subchainSet.insert( itr ); // 延長して同じ仮想チェーンになった場合は削除する
}


void VirtualSubChainManager::build( std::function<std::vector<std::shared_ptr<block::BlockHeader>>(std::shared_ptr<unsigned char>)> popCallback , std::shared_ptr<block::BlockHeader> stopHeader )
{
  // 新たに仮想チェーンを作成
  VirtualSubChain newSubchain( _startBlockHeader );
  newSubchain.extend( popCallback ); // 作成した仮想チェーンを伸ばせるだけ伸ばす

  _subchainSet.insert( newSubchain ); // 既に存在するsubchainだった場合は管理下にせず破棄する
}






};
