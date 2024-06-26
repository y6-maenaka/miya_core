#include "virtual_header_sync_manager.h"


namespace chain
{



VirtualHeaderSyncManager::VirtualHeaderSyncManager( std::shared_ptr<BlockHeader> startBlockHeader ,
												std::shared_ptr<unsigned char> stopHash,
												FilterStateUpdator filterStateUpdator,
												std::shared_ptr<StreamBufferContainer> toRequesterSBC
) : _startBlockHeader(startBlockHeader) , _stopHash(stopHash) , _filterStateUpdator(filterStateUpdator) ,_toRequesterSBC(toRequesterSBC)
{

 // Block Sync Managerのセットアップ
  _bhPoolFinder = std::bind(
							&BlockHeaderPool::findByBlockHash,
							std::ref(_blockHeaderPool),
							std::placeholders::_1 );

  _pbhPoolFinder = std::bind(
							&BlockHeaderPool::findByPrevBlockHash,
							std::ref(_blockHeaderPool),
							std::placeholders::_1 );


  VirtualHeaderSubChain initialSubChain( _startBlockHeader, stopHash ,_bhPoolFinder, _pbhPoolFinder );
  _headerSubchainSet.insert( initialSubChain );

  std::cout << "+++++++++++++++++++++++++++++++++++++++" << "\n";
  std::cout << "Subchain Count :: " << _headerSubchainSet.size() << "\n";
  std::cout << "isActive? -> " << initialSubChain.isActive() << "\n";
  std::cout << "ActiveSubChainCount :: " << this->activeSubchainCount() << "\n";
  std::cout << "+++++++++++++++++++++++++++++++++++++++" << "\n";

  _status = static_cast<int>( VirtualHeaderSyncManagerState::PENDING );
}

void VirtualHeaderSyncManager::sendRequestSyncedCommand()
{
  /*
  std::vector< MiyaCoreMSG_GETBLOCKS > extendCommandVector;
  for( auto itr : _headerSubchainSet )
  {
	if( itr.isActive() )
	  extendCommandVector.push_back( itr.extendCommand() );
  }

  for( auto itr : extendCommandVector )
  {
	miya_core_command command(itr);

	std::unique_ptr< SBSegment > requestSB = std::make_unique<SBSegment>();
	requestSB->options.option1 = itr;
	requestSB->options.option2 = MiyaCoreMSG_GETBLOCKS::command;

	requestSB->sendFlag( ekp2p::EKP2P_SEND_BROADCAST );
	requestSB->forwardingSBCID(DEFAULT_DAEMON_FORWARDING_SBC_ID_REQUESTER);

	std::cout << "......................." << "\n";
	std::cout << "ヘッダー同期コマンド" << "\n";
	itr.__print();
	std::cout << "......................." << "\n";

	_toRequesterSBC->pushOne( std::move(requestSB) ); // 送信モジュールに流す
  }

  return;
  */
}

size_t VirtualHeaderSyncManager::activeSubchainCount()
{
  int count=0;
  for( auto itr : _headerSubchainSet )
	count += ( itr.isActive() ) ? 1 : 0;

  return count;
}

void VirtualHeaderSyncManager::add( std::vector< std::shared_ptr<BlockHeader> > targetVector )
{
  std::cout << "新たにヘッダーが追加されます" << "\n";

  std::pair< bool , short > poolCtx; // 要素追加可否とその際のpool内部状態を取得
  std::vector< std::shared_ptr<BlockHeader> > duplicateHeaders; // 重複要素
  for( auto itr : targetVector )
  {
	poolCtx = this->_blockHeaderPool.push( itr );
	if( poolCtx.first && poolCtx.second > 1 ) // 追加成功 && 追加後の要素数が2以上 => 重複追加
	  duplicateHeaders.push_back( itr ); // 重複した要素は保存しておく
  }

 for( auto itr : duplicateHeaders )
  {
	std::cout << "新たにサブチェーンがビルドされます" << "\n";
	this->build( itr ); // 重複ブロックが存在した場合は仮想チェーンを作成する
  }

  if( this->extend() ) // headerPoolに更新があった旨をsubchainに通知し,subchainの延長を試みる
	_status = static_cast<int>(VirtualHeaderSyncManagerState::FINISHED); // 延長操作によりstopに達したらフラグを立てて終了する
}


bool VirtualHeaderSyncManager::extend( bool collisionAction ) // あまり良い方法ではない
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

  if( chainStopFlag )
	_status = static_cast<int>(VirtualHeaderSyncManagerState::FINISHED);

  std::cout << "## サブチェーンマネージャーの延長処理が終了しました" << "\n";
  return chainStopFlag;
}


void VirtualHeaderSyncManager::build( std::shared_ptr<BlockHeader> stopHeader )
{
  std::shared_ptr<unsigned char> stopHash = stopHeader->headerHash();
  // 新たに仮想チェーンを作成
  VirtualHeaderSubChain newSubchain( _startBlockHeader , stopHash ,_bhPoolFinder, _pbhPoolFinder );
  newSubchain.build( stopHeader );
  newSubchain.extend(); // 作成した仮想チェーンを伸ばせるだけ伸ばす 必ずextendしなければならない

  auto insertRet = _headerSubchainSet.insert( newSubchain ); // 既に存在するsubchainだった場合は管理下にせず破棄する
}

std::vector< std::shared_ptr<VirtualHeaderSubChain> > VirtualHeaderSyncManager::subchainVector() const
{
  std::vector< std::shared_ptr<VirtualHeaderSubChain> > ret;
  for( auto itr : _headerSubchainSet )
	ret.push_back( std::make_shared<VirtualHeaderSubChain>(itr) );

  return ret;
}

std::shared_ptr<VirtualHeaderSubChain> VirtualHeaderSyncManager::stopedHeaderSubchain()
{
  for( auto && itr : _headerSubchainSet ){
	if( itr.isStoped() ) return std::make_shared<VirtualHeaderSubChain>( itr );
  }
  return nullptr;
}


bool VirtualHeaderSyncManager::start()
{
  std::cout << "VirtualHeaderSyncManager::start thread started()" << "\n";

  std::cout << "(initial) ActiveSubChainCount :: " << this->activeSubchainCount() << "\n";
  std::cout << "(initial) HeaderSyncManager State :: " << this->status() << "\n";

  int sendCount = 0;
  while( this->activeSubchainCount() > 0 || _status == static_cast<int>(VirtualHeaderSyncManagerState::FINISHED) )
  {
    std::cout << "(" << sendCount << ")" << " : 送信カウント" << "\n";
    this->sendRequestSyncedCommand();

    std::this_thread::sleep_for( std::chrono::seconds(HEADER_REQUEST_TIMEOUT_SECOND) ); // 受信待ち時間
    sendCount++; // 操作には影響しない アクティブなサブチェーンがなくなあると失敗と判定する
  }

  std::cout << "VirtualHeaderSyncManagerダウンロードシーケンスが終了しました" << "\n";
  return _status == static_cast<int>(VirtualHeaderSyncManagerState::FINISHED);
}

int VirtualHeaderSyncManager::status() const
{
  return _status;
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
