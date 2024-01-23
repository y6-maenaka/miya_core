#include "virtual_block_sync_manager.h"

#include "./virtual_block.h"

#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"


namespace miya_chain
{





VirtualBlockSyncManager::Window::Window( unsigned int windowHeadIndex ) : _windowHeadIndex(windowHeadIndex)
{
  return;
}





VirtualBlockSyncManager::VirtualBlockSyncManager( std::vector< std::shared_ptr<block::BlockHeader> > virtualHeaderChainVector ) 
{
  // 仮想ダウンロード専用チェーンの作成
  unsigned int index=0; 
  for( auto itr : virtualHeaderChainVector )
  {
	std::shared_ptr<VirtualBlock> generatedVBlock = std::shared_ptr<VirtualBlock>( new VirtualBlock(itr) );
	_chain.insert( { index , generatedVBlock} );
	index++;
  }

  return;
}


MiyaChainCommand VirtualBlockSyncManager::downloadCommand()
{
}


void VirtualBlockSyncManager::add( std::shared_ptr<block::Block> target )
{
  return;
}


bool VirtualBlockSyncManager::start()
{
  while( (_window._windowHeadIndex + 1) >= _chain.size() )   // 終了条件 : 処理済みのウィンドウセグメントがchain+1(個数+1)まで進んだ場合
  {

	MiyaChainCommand requestCommand = this->downloadCommand();

	std::unique_ptr< SBSegment > requestSB;
	_toRequesterSBC->pushOne( std::move(requestSB) );

  }

    return false;
}

// 帰ってくる or タイムアウト








};
