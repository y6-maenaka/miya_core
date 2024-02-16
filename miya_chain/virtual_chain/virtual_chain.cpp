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






VirtualChain::VirtualChain( BlockChainIterator& initialForkPoint , std::shared_ptr<BlockLocalStrageManager> localStrageManager ,std::shared_ptr<StreamBufferContainer> toRequesterSBC ) : _forkPoint(initialForkPoint)
{
  _forkPoint = initialForkPoint;
  _toRequesterSBC = toRequesterSBC;
  _localStrageManager = localStrageManager;

  // フィルターのセットアップと初期ヘッダーの登録
  _filter = std::shared_ptr<BDFilter>( new BDFilter() );
  std::shared_ptr<block::Block> forkPointBlock = _forkPoint.block();
  if( forkPointBlock == nullptr ) return;
  _filter->filter( forkPointBlock );
 
  _status = static_cast<int>(VirtualChainState::PENDING);
}


void VirtualChain::forward()
{
  std::thread vchainBuilder([&]()
  {
	std::cout << "< forward > Started VirtualChain Builder Thread" << "\n";


	FilterStateUpdator filterStateUpdator = std::bind( 
							  &BDFilter::update,
							  std::ref(_filter),
							  std::placeholders::_1 ,
							  std::placeholders::_2
						  );
  
	std::vector< std::shared_ptr<block::BlockHeader> > longestHeaderChain;
	int forkPointDecrimentsCount = 0;
	do
	{
	  _syncManager._headerSyncManager = std::shared_ptr<VirtualHeaderSyncManager>( new VirtualHeaderSyncManager( _forkPoint.header() , nullptr, filterStateUpdator, _toRequesterSBC) );
	  _syncManager._headerSyncManager->start();
	  _status = static_cast<int>(VirtualChainState::HEADER_SYNC_MANAGER_WORKING);

	  std::vector< std::shared_ptr<VirtualHeaderSubChain> > headerSubchainVector;
	  headerSubchainVector = _syncManager._headerSyncManager->subchainVector(); 

	  for( auto itr : headerSubchainVector ) // 構築された仮想ヘッダーチェーンで最も長いものを採用する
	  { 
		auto headerChain = itr->exportChainVector();
		if( headerChain.size() > longestHeaderChain.size() ) longestHeaderChain = headerChain;
	  }
	
	  if( longestHeaderChain.size() > 1 ) break; // (初期ヘッダー + ダウンロードしたヘッダー) が構築されていれば処理を進める
	  
	  _forkPoint.operator--();
	  forkPointDecrimentsCount++;
	  longestHeaderChain.clear(); headerSubchainVector.clear(); // 必ず

	} while( forkPointDecrimentsCount < ALLOWED_FORKPOINT_DECREMENTS );

	if( longestHeaderChain.size() <= 1 ){
	  _status = static_cast<int>(VirtualChainState::HEADER_SYNC_MANAGER_FAILED);
	  std::cout << "ヘッダーの同期に失敗しました" << "\n";
	}

	_status = static_cast<int>(VirtualChainState::HEADER_SYNC_MANAGER_DONE);
	_syncManager._blockSyncManager = std::make_shared<VirtualBlockSyncManager>( longestHeaderChain, _localStrageManager , _toRequesterSBC );
	bool blockSyncCompleteFlag = _syncManager._blockSyncManager->start();

	if( blockSyncCompleteFlag )
	  _status = static_cast<int>(VirtualChainState::BLOCK_SYNC_MANAGER_DONE);
	else
	  _status = static_cast<int>(VirtualChainState::BLOCK_SYNC_MANAGER_FAILED);

	std::cout << "< forward > 仮想チェーン同期マネージャー(header&block)終了" << "\n";
  });
  vchainBuilder.detach();
  return;
}

void VirtualChain::backward( std::shared_ptr<block::BlockHeader> objectiveHeader )
{
  std::thread vchainBuilder([&]()
  {
	std::cout << "< backward > Started VirtualChain Builder Thread" << "\n";
	  
	bool headerSyncCompleteFlag = false;
	std::shared_ptr<unsigned char> stopHash = objectiveHeader->headerHash();

	FilterStateUpdator filterStateUpdator = std::bind( // フィルターの更新関数
							  &BDFilter::update,
							  std::ref(_filter),
							  std::placeholders::_1 ,
							  std::placeholders::_2
						  );
  
	int forkPointDecrimentsCount = 0;
	do  // 成功するまでフォークポイントを下げて検証する
	{
		std::cout << "ヘッダー同期マネージャー In ループ" << "\n";
		_syncManager._headerSyncManager = std::shared_ptr<VirtualHeaderSyncManager>( new VirtualHeaderSyncManager( _forkPoint.header(), stopHash , filterStateUpdator , _toRequesterSBC ) );

		_status = static_cast<int>(VirtualChainState::HEADER_SYNC_MANAGER_WORKING); // 内部状態の更新
		headerSyncCompleteFlag = _syncManager._headerSyncManager->start();

		_forkPoint.operator--(); // ヘッダ収集の始点を下げる(内部状態が更新さない演算子オーバーロードの場合は,明示的にoperatorまで書かなければならないらしい)
  		forkPointDecrimentsCount++;
	} while( !(headerSyncCompleteFlag) || forkPointDecrimentsCount < ALLOWED_FORKPOINT_DECREMENTS );
 
	if( !(headerSyncCompleteFlag) ){ // ヘッダーの同期に失敗した場合
	  _status = static_cast<int>(VirtualChainState::HEADER_SYNC_MANAGER_FAILED);
	  std::cout << "ヘッダーの同期に失敗しました" << "\n";
	  return;
	}
   
	std::cout << "ヘッダー同期シーケンスが終了しました" << "\n";
	_status = static_cast<int>(VirtualChainState::HEADER_SYNC_MANAGER_DONE); // 内部状態の更新

	std::shared_ptr<VirtualHeaderSubChain> syncedHeaderSubchain = _syncManager._headerSyncManager->stopedHeaderSubchain();
	std::vector< std::shared_ptr<block::BlockHeader> > syncedHeaderVector;
	syncedHeaderVector = syncedHeaderSubchain->exportChainVector();

    _syncManager._blockSyncManager = std::make_shared<VirtualBlockSyncManager>( syncedHeaderVector , _localStrageManager , _toRequesterSBC );
    bool blockSyncCompleteFlag = _syncManager._blockSyncManager->start();
	
	if( blockSyncCompleteFlag )
	  _status = static_cast<int>(VirtualChainState::BLOCK_SYNC_MANAGER_DONE);
	else
	  _status = static_cast<int>(VirtualChainState::BLOCK_SYNC_MANAGER_FAILED);

	std::cout << "< backward > 仮想チェーン同期マネージャー(header&block)終了" << "\n";
  });

  vchainBuilder.detach();
  return;
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
