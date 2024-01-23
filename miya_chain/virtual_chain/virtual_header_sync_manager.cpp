#include "virtual_header_sync_manager.h"

#include "./virtual_header_subchain.h"
#include "../block/block.h"

#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"




namespace miya_chain
{



VirtualHeaderSyncManager::VirtualHeaderSyncManager( std::shared_ptr<block::BlockHeader> startBlockHeader ,
												std::shared_ptr<unsigned char> stopHash,
												BHPoolFinder bhPoolFinder,
												PBHPoolFinder pbhPoolFinder,
												std::shared_ptr<StreamBufferContainer> toRequesterSBC
) : _startBlockHeader(startBlockHeader) , _bhPoolFinder(bhPoolFinder), _pbhPoolFinder(pbhPoolFinder) , _stopHash(stopHash) , _toRequesterSBC(toRequesterSBC)
{
  VirtualHeaderSubChain initialSubChain( _startBlockHeader, stopHash ,_bhPoolFinder, _pbhPoolFinder );


  _headerSubchainSet.insert( initialSubChain );
}

bool VirtualHeaderSyncManager::extend( bool collisionAction )
{
  bool chainStopFlag = false;
  std::vector< VirtualHeaderSubChain > extendedVector;
  for( auto && itr : _headerSubchainSet )
  {
	extendedVector.push_back( itr );
  }
  
  for( auto && itr : extendedVector )
  {
	if( itr.extend( collisionAction ) ) chainStopFlag = true;
  }
  
  _headerSubchainSet.clear();

  for( auto && itr : extendedVector )
  {
	_headerSubchainSet.insert( itr );
  }

  std::cout << "## サブチェーンマネージャーの延長処理が終了しました" << "\n";
  return chainStopFlag;
}


void VirtualHeaderSyncManager::build( std::shared_ptr<block::BlockHeader> stopHeader )
{
  std::shared_ptr<unsigned char> stopHash = stopHeader->headerHash();
  // 新たに仮想チェーンを作成
  VirtualHeaderSubChain newSubchain( _startBlockHeader , stopHash ,_bhPoolFinder, _pbhPoolFinder );
  newSubchain.build( stopHeader );
  newSubchain.extend(); // 作成した仮想チェーンを伸ばせるだけ伸ばす

  auto insertRet = _headerSubchainSet.insert( newSubchain ); // 既に存在するsubchainだった場合は管理下にせず破棄する
}

std::shared_ptr<VirtualHeaderSubChain> VirtualHeaderSyncManager::stopedHeaderSubchain()
{
  for( auto && itr : _headerSubchainSet ){
	if( itr.isChainStoped() ) return std::make_shared<VirtualHeaderSubChain>( itr );
  }
  return nullptr;
}

unsigned short VirtualHeaderSyncManager::subchainCount()
{
  return _headerSubchainSet.size();
}

void VirtualHeaderSyncManager::__printSubChainSet()
{
  std::cout << "SubChainCount :: " << _headerSubchainSet.size() << "\n";
  
  for( auto __ : _headerSubchainSet )
  {
	VirtualHeaderSubChain subChain = __;
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
