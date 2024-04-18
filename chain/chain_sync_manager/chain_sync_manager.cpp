#include "chain_sync_manager.h"

#include "./bd_filter.h"

#include "../chain_manager.h"

#include "../../share/stream_buffer/stream_buffer.h"
#include "../../share/stream_buffer/stream_buffer_container.h"

#include "../daemon/broker/broker.h"
#include "../daemon/requester/requester.h"

#include "../block/block.h"
#include "../block/block_header.h"
#include "../transaction/coinbase/coinbase.hpp"

#include "../../ekp2p/daemon/sender/sender.h"

#include "../utxo_set/utxo_set.h"
#include "../miya_coin/local_strage_manager.h"

#include "./virtual_header_sync_manager.h"
#include "./virtual_header_subchain.h"
#include "./virtual_block_sync_manager.h"



namespace chain
{



ChainSyncManager::ChainSyncManager( BlockChainIterator& initialForkPoint , std::shared_ptr<BlockLocalStrageManager> localStrageManager ,std::shared_ptr<StreamBufferContainer> toRequesterSBC ) : _forkPoint(initialForkPoint)
{
  _forkPoint = initialForkPoint;
  _toRequesterSBC = toRequesterSBC;
  _localStrageManager = localStrageManager;

  // フィルターのセットアップと初期ヘッダーの登録
  _filter = std::shared_ptr<BDFilter>( new BDFilter() );
  std::shared_ptr<Block> forkPointBlock = _forkPoint.block();
  if( forkPointBlock == nullptr ) return;
  _filter->filter( forkPointBlock );
 
  _status = static_cast<int>(ChainSyncManagerState::PENDING);
}


void ChainSyncManager::forward()
{
  std::thread vchainBuilder([&]()
  {
	std::cout << "< forward > Started ChainSyncManager Builder Thread" << "\n";

	FilterStateUpdator filterStateUpdator = std::bind(  // フィルター更新関数のセットアップ
							  &BDFilter::update,
							  std::ref(_filter),
							  std::placeholders::_1 ,
							  std::placeholders::_2
						  );
  
	std::vector< std::shared_ptr<BlockHeader> > longestHeaderChain;
	int forkPointDecrimentsCount = 0;
	do
	{
	  if( _forkPoint.isHead() ){
		std::cout << "フォークポイントがチェーン先頭に達しました" << "\n";
		_status = static_cast<int>(ChainSyncManagerState::HEADER_SYNC_MANAGER_FAILED);
		return;
	  }

	  _syncManager._headerSyncManager = std::shared_ptr<VirtualHeaderSyncManager>( new VirtualHeaderSyncManager( _forkPoint.header() , nullptr, filterStateUpdator, _toRequesterSBC) );
	  _status = static_cast<int>(ChainSyncManagerState::HEADER_SYNC_MANAGER_WORKING);
	  _syncManager._headerSyncManager->start();

	  std::vector< std::shared_ptr<VirtualHeaderSubChain> > headerSubchainVector = _syncManager._headerSyncManager->subchainVector(); // 全ての構築済みサブチェーンを収集する

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

	if( longestHeaderChain.size() <= 1 ){ // 初期ヘッダー以外のヘッダーが収集されていない場合
	  _status = static_cast<int>(ChainSyncManagerState::HEADER_SYNC_MANAGER_FAILED);
	  std::cout << "ヘッダーの同期に失敗しました" << "\n";
	}

	_status = static_cast<int>(ChainSyncManagerState::HEADER_SYNC_MANAGER_DONE);
	_syncManager._blockSyncManager = std::make_shared<VirtualBlockSyncManager>( longestHeaderChain, _localStrageManager , _toRequesterSBC ); // ブロック同期マネージャーの作成
	bool blockSyncCompleteFlag = _syncManager._blockSyncManager->start();

	if( blockSyncCompleteFlag )
	  _status = static_cast<int>(ChainSyncManagerState::BLOCK_SYNC_MANAGER_DONE);
	else
	  _status = static_cast<int>(ChainSyncManagerState::BLOCK_SYNC_MANAGER_FAILED);

	std::cout << "< forward > 仮想チェーン同期マネージャー(header&block)終了" << "\n";
  });
  vchainBuilder.detach();
  return;
}

void ChainSyncManager::backward( std::shared_ptr<BlockHeader> objectiveHeader )
{
  std::thread vchainBuilder([&]()
  {
	std::cout << "< backward > Started ChainSyncManager Builder Thread" << "\n";
	  
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
	  if( _forkPoint.isHead() ){
		std::cout << "フォークポイントがチェーン先頭に達しました" << "\n";
		_status = static_cast<int>(ChainSyncManagerState::HEADER_SYNC_MANAGER_FAILED);
		return;
	  }

	  _syncManager._headerSyncManager = std::shared_ptr<VirtualHeaderSyncManager>( new VirtualHeaderSyncManager( _forkPoint.header(), stopHash , filterStateUpdator , _toRequesterSBC ) );
	  _status = static_cast<int>(ChainSyncManagerState::HEADER_SYNC_MANAGER_WORKING); // 内部状態の更新
	  headerSyncCompleteFlag = _syncManager._headerSyncManager->start();

	  _forkPoint.operator--(); // ヘッダ収集の始点を下げる(内部状態が更新さない演算子オーバーロードの場合は,明示的にoperatorまで書かなければならないらしい)
	  forkPointDecrimentsCount++;
	} while( !(headerSyncCompleteFlag) || forkPointDecrimentsCount < ALLOWED_FORKPOINT_DECREMENTS );
 
	if( !(headerSyncCompleteFlag) ){ // ヘッダーの同期に失敗した場合
	  _status = static_cast<int>(ChainSyncManagerState::HEADER_SYNC_MANAGER_FAILED);
	  std::cout << "ヘッダーの同期に失敗しました" << "\n";
	  return;
	}
   
	std::cout << "ヘッダー同期シーケンスが終了しました" << "\n";
	_status = static_cast<int>(ChainSyncManagerState::HEADER_SYNC_MANAGER_DONE); // 内部状態の更新

	std::shared_ptr<VirtualHeaderSubChain> syncedHeaderSubchain = _syncManager._headerSyncManager->stopedHeaderSubchain();
	std::vector< std::shared_ptr<BlockHeader> > syncedHeaderVector;
	syncedHeaderVector = syncedHeaderSubchain->exportChainVector();

    _syncManager._blockSyncManager = std::make_shared<VirtualBlockSyncManager>( syncedHeaderVector , _localStrageManager , _toRequesterSBC );
    bool blockSyncCompleteFlag = _syncManager._blockSyncManager->start();
	
	if( blockSyncCompleteFlag )
	  _status = static_cast<int>(ChainSyncManagerState::BLOCK_SYNC_MANAGER_DONE);
	else
	  _status = static_cast<int>(ChainSyncManagerState::BLOCK_SYNC_MANAGER_FAILED);

	std::cout << "< backward > 仮想チェーン同期マネージャー(header&block)終了" << "\n";
  });

  vchainBuilder.detach();
  return;
}


void ChainSyncManager::add( std::vector<std::shared_ptr<BlockHeader>> targetVector )
{
  if( _status != static_cast<int>(ChainSyncManagerState::HEADER_SYNC_MANAGER_WORKING) ) return;

  std::shared_ptr<struct BDBCB> filterCtx;
  std::vector< std::shared_ptr<BlockHeader> > passedHeaderVector;

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


void ChainSyncManager::add( std::shared_ptr<Block> targetBlock )
{
  if( _status != static_cast<int>(ChainSyncManagerState::BLOCK_SYNC_MANAGER_WORKING) ) return;

  std::shared_ptr<struct BDBCB> filterCtx;
  filterCtx = _filter->filter( targetBlock );
  if( filterCtx == nullptr ) return;
  if( filterCtx->status() > static_cast<int>(BDState::BlockBodyReceived) ) return;

  return _syncManager._blockSyncManager->add( targetBlock );
}


void ChainSyncManager::__printState()
{
  std::cout << "====== < 仮想チェーン内部状態 > ====== " << "\n";

  _syncManager._headerSyncManager->__printSubChainSet();

  std::cout << "-----------------------------------------" << "\n";
}






};
