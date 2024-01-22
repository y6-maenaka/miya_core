#include "virtual_subchain_manager.h"

#include "./virtual_subchain.h"
#include "../block/block.h"




namespace miya_chain
{



VirtualSubChainManager::VirtualSubChainManager( std::shared_ptr<block::BlockHeader> startBlockHeader ,
												std::shared_ptr<unsigned char> stopHash,
												BHPoolFinder bhPoolFinder,
												PBHPoolFinder pbhPoolFinder
) : _startBlockHeader(startBlockHeader) , _bhPoolFinder(bhPoolFinder), _pbhPoolFinder(pbhPoolFinder) , _stopHash(stopHash)
{
  VirtualSubChain initialSubChain( _startBlockHeader, stopHash ,_bhPoolFinder, _pbhPoolFinder );


  _subchainSet.insert( initialSubChain );
}

void VirtualSubChainManager::extend( bool collisionAction )
{
  std::vector< VirtualSubChain > extendedVector;
  for( auto && itr : _subchainSet )
  {
	extendedVector.push_back( itr );
  }
  
  for( auto && itr : extendedVector )
  {
	itr.extend( collisionAction );
  }
  
  _subchainSet.clear();

  for( auto && itr : extendedVector )
  {
	_subchainSet.insert( itr );
  }

  std::cout << "## サブチェーンマネージャーの延長処理が終了しました" << "\n";
}


void VirtualSubChainManager::build( std::shared_ptr<block::BlockHeader> stopHeader )
{
  std::shared_ptr<unsigned char> stopHash = stopHeader->headerHash();
  // 新たに仮想チェーンを作成
  VirtualSubChain newSubchain( _startBlockHeader , stopHash ,_bhPoolFinder, _pbhPoolFinder );
  newSubchain.build( stopHeader );
  newSubchain.extend(); // 作成した仮想チェーンを伸ばせるだけ伸ばす

  auto insertRet = _subchainSet.insert( newSubchain ); // 既に存在するsubchainだった場合は管理下にせず破棄する
}


unsigned short VirtualSubChainManager::subchainCount()
{
  return _subchainSet.size();
}

void VirtualSubChainManager::__printSubChainSet()
{
  std::cout << "SubChainCount :: " << _subchainSet.size() << "\n";
  
  for( auto __ : _subchainSet )
  {
	VirtualSubChain subChain = __;
	auto _header = subChain.latestBlockHeader();
	subChain.__printChainDigest();

	auto exportedHeaderVector = subChain.exportChainVector();

	std::cout << "仮想サブチェーンサイズ :: " << exportedHeaderVector.size() << "\n";
	for( auto itr : exportedHeaderVector ){
	  itr->print();
	  std::cout << "---------------------------" << "\n";
	}
  }



}



};
