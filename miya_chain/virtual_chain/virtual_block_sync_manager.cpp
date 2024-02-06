#include "virtual_block_sync_manager.h"

#include "./virtual_block.h"

#include "../block/block.h"
#include "../block/block_header.h"

#include "../utxo_set/utxo.h"
#include "../utxo_set/utxo_set.h"

#include "../miya_coin/local_strage_manager.h"

#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"





namespace miya_chain
{





VirtualBlockSyncManager::UnChaindedWindow::UnChaindedWindow( BlockLocalStrageManager& localStrageManager , unsigned short windowSize ) : _localStrageManager(localStrageManager)
{
  this->_windowSize = windowSize;
  this->syncArrivedAt();
}

void VirtualBlockSyncManager::UnChaindedWindow::add( std::shared_ptr<block::Block> target )
{
  if( target == nullptr ) return;

  for( auto itr : _segmentVector ) // windowから一致するブロックを探す
  {
	if( memcmp( itr.first.get() , target->blockHash().get() , 32  ) == 0 )
	{
	  itr.second.second = target;
	  this->syncArrivedAt(); // 受信時刻を更新
	}
  }
  return;
}

void VirtualBlockSyncManager::UnChaindedWindow::setup( std::vector<std::shared_ptr<VirtualBlock>> fromVBVector )  // ローカルにブロック本体が存在する場合は一旦それを採用する
{
  _segmentVector.clear();

  std::shared_ptr<unsigned char> blockHash = nullptr;
  std::shared_ptr<block::Block> readedBlock = nullptr;

  for( auto itr : fromVBVector )
  {
	blockHash = itr->blockHash();
	if( blockHash == nullptr ) break;
	readedBlock = _localStrageManager.readBlock( blockHash );
	
	if( readedBlock == nullptr )  // ブロック本体がローカルに保存して"ない"場合
	  _segmentVector.push_back( std::make_pair( blockHash , std::make_pair(false ,nullptr) ) );
	else // ブロック本体がローカルに保存されて"いる"場合
	  _segmentVector.push_back( std::make_pair( blockHash , std::make_pair(true, readedBlock) ) );
  }

  _windowSize = fromVBVector.size();
  return;
}


std::pair< bool , std::shared_ptr<block::Block> > VirtualBlockSyncManager::UnChaindedWindow::at( unsigned int index )
{
  if( index >= _segmentVector.size() ) return std::make_pair( false , nullptr );

  return _segmentVector.at(index).second;
}

std::vector< std::shared_ptr<unsigned char> > VirtualBlockSyncManager::UnChaindedWindow::unDownloadedBlockHashVector()
{
  std::vector< std::shared_ptr<unsigned char> > ret; 

  for( auto itr : _segmentVector ) // window内の要素からまだ本体が到着していないブロックのハッシュを取り出す
  {
	if( itr.second.second == nullptr ) ret.push_back( itr.first ); 
  }

  return ret;
}


unsigned short VirtualBlockSyncManager::UnChaindedWindow::size()
{
  return _windowSize;
}





VirtualBlockSyncManager::VirtualBlockSyncManager( std::vector< std::shared_ptr<block::BlockHeader> > virtualHeaderChainVector, BlockLocalStrageManager &localStrageManager  ) : _unChaindedWindow( localStrageManager )
{
  // 仮想ダウンロード専用チェーンの作成
  unsigned int index=0; 
  for( auto itr : virtualHeaderChainVector )
  {
	  std::shared_ptr<VirtualBlock> generatedVBlock = std::shared_ptr<VirtualBlock>( new VirtualBlock(itr) );
	  _chain.insert( { index , generatedVBlock} );
	  index++;
  }
 
  // 最終送信時間は現在にセットアップしておく
  _verifiedIndex = 0;
  _sendedAt = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - BLOCK_REQUEST_TIMEOUT_SECOND;
  return;
}

void VirtualBlockSyncManager::syncSendedAt()
{
  _sendedAt = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

unsigned int VirtualBlockSyncManager::sendElapsedTime() const
{
  return static_cast<unsigned int>( std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - _sendedAt );
}


MiyaChainMSG_INV VirtualBlockSyncManager::downloadCommand()
{
  // MiyaChainMSG_INV command;
 
}


void VirtualBlockSyncManager::sendRequestSyncedCommand()
{
  MiyaChainCommand requestCommand = this->downloadCommand();

  std::unique_ptr< SBSegment > requestSB;
  _toRequesterSBC->pushOne( std::move(requestSB) );

  this->syncSendedAt();
}


bool VirtualBlockSyncManager::downloadWindow( int allowedRetransmissionCount )
{
  // windowダウンロードシーケンス
  int retransmissionCount = 1; // 再送カウント
  while( !(_unChaindedWindow.isComplete()) && retransmissionCount <= allowedRetransmissionCount ) // windowの全ての要素がダウンロード済みでない間
  {
	std::cout << "ブロックダウンロードリクエストコマンドを送信します" << "\n";
	this->sendRequestSyncedCommand(); // ブロックダウンロードコマンドを送信
	retransmissionCount++;

	std::this_thread::sleep_for( std::chrono::seconds(BLOCK_REQUEST_TIMEOUT_SECOND) ); // 受信待ち時間
  }
  
  if( _unChaindedWindow.isComplete() ) return true; 
  return false;
}


void VirtualBlockSyncManager::add( std::shared_ptr<block::Block> target ) // 検証プロセスも兼ねる
{
  std::shared_ptr<VirtualBlock> findedBlock = nullptr;
  std::shared_ptr<unsigned char> targetBlockHash = target->blockHash();
  std::vector< std::shared_ptr<UTXO> > utxoVector;

  return;
}



void VirtualBlockSyncManager::start() 
{
  std::vector< std::shared_ptr<VirtualBlock> > windowTargetVBVector; // _chainから取り出したwindow作成用のVirtualBlockリスト
  std::thread requestSender([&]() 
  { // 基本的に検証プロセス
  
	bool failureFlag = false;
	unsigned int vbIndex = 0;
	bool blockIsInLocal;

	while( _verifiedIndex < _chain.size() ) // チェーンの最後尾まで検証が済むまで
	{
	  windowTargetVBVector.clear();
	  vbIndex = _verifiedIndex;
	  while( vbIndex <= _chain.size() ) // windowサイズに到着 or _chain最後尾までのVirtualBlockをベクタ化する
	  { 
		windowTargetVBVector.push_back( _chain.at(vbIndex) );
		vbIndex += 1;
	  }
	  if( windowTargetVBVector.empty() ) break; // _chain内の全てのブロックの検証が完了している場合は終了
	 
	  // Window分のブロックダウンロード処理
	  _unChaindedWindow.setup( windowTargetVBVector ); // UnChaindedWindowのセットアップ
	  if( !(this->downloadWindow()) ) // windowダウンロードに失敗した場合 // ブロッキングメソッド
	  {
		failureFlag = true;
		break;
	  }

	  for( int i=0; i<_unChaindedWindow.size(); i++ )
	  {
		std::shared_ptr<block::Block> targetBlock;
		auto segment = _unChaindedWindow.at(i);
		blockIsInLocal = segment.first;
		targetBlock = segment.second;
		// ここでブロックの検証を行う
		
		
		// 検証をパスしたブロックであれば	
		if( !(blockIsInLocal) ) // ローカルにあるブロックの場合は特に保存操作はしない
		  _localStrageManager->writeBlock( targetBlock );
	  }
	}


  });

  requestSender.detach(); // スレッド終了は自身で制御する
/*
  リクエストコマンド(メッセージ)を送信するタイミングは
  <イベント>
  timeout -> 
  window内のブロックが途中まで到着 -> 感知方法:
  
*/
}






// 帰ってくる or タイムアウト








};
