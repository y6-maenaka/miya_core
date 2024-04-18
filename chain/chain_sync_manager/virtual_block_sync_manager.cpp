#include "virtual_block_sync_manager.h"


namespace chain
{


VirtualBlockSyncManager::UnChaindedWindow::UnChaindedWindow( unsigned short windowSize )
{
  this->_windowSize = windowSize;
}

void VirtualBlockSyncManager::UnChaindedWindow::add( std::shared_ptr<Block> target )
{
  if( target == nullptr ) return;

  for( auto &itr : _segmentVector ) // windowから一致するブロックを探す
  {
	if( memcmp( itr.first.get() , target->blockHash().get() , 32  ) == 0 )
	{
	  itr.second.second = target;
	}
  }
  return;
}

std::pair< bool , std::shared_ptr<Block> > VirtualBlockSyncManager::UnChaindedWindow::at( unsigned int index )
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

bool VirtualBlockSyncManager::UnChaindedWindow::isComplete()
{
  for( auto itr : _segmentVector )
	if( itr.second.second == nullptr ) return false;
  return true;
}


unsigned short VirtualBlockSyncManager::UnChaindedWindow::size()
{
  return _windowSize;
}

void VirtualBlockSyncManager::UnChaindedWindow::__print()
{
  for( int i=0; i<_segmentVector.size(); i++ )
  {
	std::cout << " Index :: ("<< i << ")" << "\n";
	std::cout << " BlockHash :: ";
	for( int j=0; j<32; j++ ){
	  printf("%02X", _segmentVector.at(i).first.get()[j] );
	} std::cout << "\n";
	std::cout << " 本体存在有無 :: " <<  (( _segmentVector.at(i).second.second != nullptr ) ? "True" : "False") << "\n";
	std::cout << "-------------------------------" << "\n";

  }
}

VirtualBlockSyncManager::VirtualBlockSyncManager( std::vector< std::shared_ptr<BlockHeader> > virtualHeaderChainVector, std::shared_ptr<BlockLocalStrageManager> localStrageManager , std::shared_ptr<StreamBufferContainer> toRequesterSBC )
{
  _localStrageManager = localStrageManager;
  _toRequesterSBC = toRequesterSBC;
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


std::pair< MiyaChainCommand , const char* > VirtualBlockSyncManager::downloadCommand()
{
  MiyaChainMSG_INV inv;
  MiyaChainMSG_GETDATA getdataCommand;
  for( auto itr : _unChaindedWindow._segmentVector )
  {
	if( itr.second.second == nullptr ) // ダウンロードされていないブロックを対象に
	  inv.addBlock( itr.first );
  }

  getdataCommand.inv( inv );
  MiyaChainCommand wrapCommand = getdataCommand;


  std::cout << "++++++++++++++++++++++" << "\n";
  std::cout << "作成済みブロック同期コマンド" << "\n";
  inv.__print();
  std::cout << "++++++++++++++++++++++" << "\n";

  return std::make_pair( wrapCommand , MiyaChainMSG_INV::command );
}

void VirtualBlockSyncManager::sendRequestSyncedCommand()
{
  /*
  std::pair< MiyaChainCommand , const char* > requestCommand = this->downloadCommand();

  std::unique_ptr< SBSegment > requestSB = std::make_unique<SBSegment>();
  requestSB->options.option1 = requestCommand.first;
  requestSB->options.option2 = requestCommand.second;

  requestSB->sendFlag( ekp2p::EKP2P_SEND_BROADCAST );
  requestSB->forwardingSBCID(DEFAULT_DAEMON_FORWARDING_SBC_ID_REQUESTER);

  _toRequesterSBC->pushOne( std::move(requestSB) );

  this->syncSendedAt();
  */
}


bool VirtualBlockSyncManager::downloadWindow( int allowedRetransmissionCount )
{
  // windowダウンロードシーケンス
  int retransmissionCount = 1; // 再送カウント
  while( !(_unChaindedWindow.isComplete()) && retransmissionCount <= allowedRetransmissionCount ) // windowの全ての要素がダウンロード済みでない間
  {
	std::cout << "ダウンロード状態 :: " << (_unChaindedWindow.isComplete()) << "\n";
	std::cout << "ブロックダウンロードリクエストコマンドを送信します" << "\n";
	this->sendRequestSyncedCommand(); // ブロックダウンロードコマンドを送信
	retransmissionCount++;

	std::this_thread::sleep_for( std::chrono::seconds(BLOCK_REQUEST_TIMEOUT_SECOND) ); // 受信待ち時間
  }
  std::cout << "ウィンドウダウンロードシーケンス終了" << "\n";
  sleep(1);

  if( _unChaindedWindow.isComplete() ) return true;
  return false;
}


void VirtualBlockSyncManager::setupWindow( std::vector<std::shared_ptr<VirtualBlock>> fromVBVector )
{
  _unChaindedWindow._segmentVector.clear(); // 現在のウィンドウをクリアする

  std::shared_ptr<unsigned char> blockHash = nullptr;
  std::shared_ptr<Block> readedBlock = nullptr;

  for( auto itr : fromVBVector )
  {
	  blockHash = itr->blockHash();
	  if( blockHash == nullptr ) break; // 基本的にありえない
	  readedBlock = _localStrageManager->readBlock( blockHash ); // ローカルにブロックが存在するかを検証する

	  if( readedBlock == nullptr )
		_unChaindedWindow._segmentVector.push_back( std::make_pair( blockHash , std::make_pair(false, nullptr)) ); // ローカルにブロックが存在しない場合
	  else // ローカルにブロックが存在した場合はフラグを立てる
		_unChaindedWindow._segmentVector.push_back( std::make_pair( blockHash, std::make_pair(true, readedBlock) ) );
  }

  _unChaindedWindow._windowSize = fromVBVector.size();
  return;
}


void VirtualBlockSyncManager::add( std::shared_ptr<Block> target ) // 検証プロセスも兼ねる
{
  std::shared_ptr<VirtualBlock> findedBlock = nullptr;
  std::shared_ptr<unsigned char> targetBlockHash = target->blockHash();

  return _unChaindedWindow.add( target );
}



bool VirtualBlockSyncManager::start()
{
  std::vector< std::shared_ptr<VirtualBlock> > windowTargetVBVector; // _chainから取り出したwindow作成用のVirtualBlockリスト

  bool failureFlag = false;
  unsigned int vbIndex = 0;
  bool blockIsInLocal;

  while( vbIndex < _chain.size() ) // チェーンの最後尾まで検証が済むまで
  {
	windowTargetVBVector.clear();
	vbIndex = _verifiedIndex;
	while( vbIndex < _chain.size() ) // windowサイズに到着 or _chain最後尾までのVirtualBlockをベクタ化する
	{
	  _chain.at(vbIndex)->__print();
	  windowTargetVBVector.push_back( _chain.at(vbIndex) );
	  vbIndex += 1;
	}

	std::cout << "## windowTargetVBVectorのサイズ :: " << windowTargetVBVector.size() << "\n";

	if( windowTargetVBVector.empty() ) break; // _chain内の全てのブロックの検証が完了している場合は終了

	// Window分のブロックダウンロード処理
	this->setupWindow( windowTargetVBVector ); // UnChaindedWindowのセットアップ
	_unChaindedWindow.__print();

	if( !(this->downloadWindow()) ) // windowダウンロードに失敗した場合 // downloadWindowは成功/失敗までブロッキングする
	{
	  failureFlag = true;
	  break;
	}
	std::cout << "ウィンドウブロックのダウンロードが完了しました" << "\n";

	for( int i=0; i<_unChaindedWindow.size(); i++ )
	{
	  std::shared_ptr<Block> targetBlock;
	  auto segment = _unChaindedWindow.at(i);
	  blockIsInLocal = segment.first;
	  targetBlock = segment.second;
	  // ここでブロックの検証を行う


	  // 検証をパスしたブロックであれば
	  if( !(blockIsInLocal) ){ // ローカルにあるブロックの場合は特に保存操作はしない
		_localStrageManager->writeBlock( targetBlock );
		std::cout << "\x1b[31m" << "ブロックがローカルに書き込まれました" << "\x1b[39m" << "\n";
	  }
	}

  }

  std::cout << "BlockSyncスレッドが終了しました" << "\n";
  return true;
}






// 帰ってくる or タイムアウト








};
