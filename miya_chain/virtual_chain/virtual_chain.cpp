#include "virtual_chain.h"

#include "./bd_filter.h"

#include "../miya_chain_manager.h"

#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../daemon/broker/broker.h"
#include "../daemon/requester/requester.h"

#include "../block/block.h"
#include "../block/block_header.h"
#include "../transaction/coinbase/coinbase.h"

#include "../message/message.h"
#include "../message/command/command_set.h"

#include "../../ekp2p/daemon/sender/sender.h"

#include "../utxo_set/utxo_set.h"
#include "../miya_coin/local_strage_manager.h"

#include "./virtual_header_sync_manager.h"
#include "./virtual_header_subchain.h"
#include "./virtual_block_sync_manager.h"



namespace miya_chain
{






VirtualChain::VirtualChain( BlockChainIterator initialForkPoint , std::shared_ptr<StreamBufferContainer> toRequesterSBC ) : _forkPoint(initialForkPoint)
{
  _forkPoint = initialForkPoint;
  _toRequesterSBC = toRequesterSBC;

  // フィルターのセットアップと初期ヘッダーの登録
  _filter = std::shared_ptr<BDFilter>( new BDFilter() );
  std::shared_ptr<block::Block> forkPointBlock = _forkPoint.block();
  if( forkPointBlock == nullptr ) return;
  _filter->filter( forkPointBlock );
 
  _status = static_cast<int>(VirtualChainState::PENDING);
}

void VirtualChain::backward( std::shared_ptr<block::BlockHeader> objectiveHeader )
{
  std::thread vchainBuilder([&]()
  {
	std::cout << "Started VirtualChain Builder Thread" << "\n";
	  
	int headerSyncFlag = 0;
	std::shared_ptr<unsigned char> stopHash = objectiveHeader->headerHash();

	FilterStateUpdator filterStateUpdator = std::bind( // フィルターの更新関数
							  &BDFilter::update,
							  std::ref(_filter),
							  std::placeholders::_1 ,
							  std::placeholders::_2
						  );

	 while( !(headerSyncFlag) )  // 成功するまでフォークポイントを下げて検証する
	 {
		std::cout << "ヘッダー同期マネージャー In ループ" << "\n";
		_syncManager._headerSyncManager = std::shared_ptr<VirtualHeaderSyncManager>( new VirtualHeaderSyncManager( _forkPoint.header(), stopHash , filterStateUpdator , _toRequesterSBC ) );

		_status = static_cast<int>(VirtualChainState::HEADER_SYNC_MANAGER_WORKING); // 内部状態の更新
		headerSyncFlag = _syncManager._headerSyncManager->start();
		std::cout << "HeaderSyncManagerのブロッキングが終了しました" << "\n";

		break;
	}
	 _status = static_cast<int>(VirtualChainState::HEADER_SYNC_MANAGER_DONE); // 内部状態の更新

	 std::cout << "仮想チェーン同期マネージャー終了" << "\n";
  });

  vchainBuilder.detach(); // デタッチする
}


void VirtualChain::add( std::vector<std::shared_ptr<block::BlockHeader>> targetVector )
{
  if( _status != static_cast<int>(VirtualChainState::HEADER_SYNC_MANAGER_WORKING) ) return;

  std::shared_ptr<struct BDBCB> filterCtx;
  std::vector< std::shared_ptr<block::BlockHeader> > passedHeaderVector;

  for( auto itr : targetVector )
  {
	filterCtx = _filter->filter( itr );
	if( filterCtx == nullptr ) continue; // フィルターに引っかかった場合はcontinue
	if( filterCtx->status() > static_cast<int>(BDState::BlockHeaderReceived) ) continue; // 既に対象要素に対する処理が終わっている場合はcontinueする

	passedHeaderVector.push_back( itr );
	filterCtx->status( static_cast<int>(BDState::BlockHeaderReceived) ); // headerPoolに追加したらFilter要素の状態を更新する
  }

  _syncManager._headerSyncManager->add( passedHeaderVector );
  return;
}


void VirtualChain::add( std::shared_ptr<block::Block> targetBlock )
{
  if( _status != static_cast<int>(VirtualChainState::BLOCK_SYNC_MANAGER_WORKING) ) return;

  std::shared_ptr<struct BDBCB> filterCtx;
  filterCtx = _filter->filter( targetBlock );
  if( filterCtx == nullptr ) return;
  if( filterCtx->status() > static_cast<int>(BDState::BlockBodyReceived) ) return;

  return _syncManager._blockSyncManager->add( targetBlock );
}


void VirtualChain::__printState()
{
  std::cout << "====== < 仮想チェーン内部状態 > ====== " << "\n";

  _syncManager._headerSyncManager->__printSubChainSet();

  std::cout << "-----------------------------------------" << "\n";
}






};
