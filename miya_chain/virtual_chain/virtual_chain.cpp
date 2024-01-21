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

#include "./virtual_subchain_manager.h"
#include "./virtual_subchain.h"



namespace miya_chain
{






VirtualChain::VirtualChain( BlockChainIterator initialForkPoint ) : _forkPoint(initialForkPoint)
{
	_forkPoint = initialForkPoint;

	// フィルターのセットアップと初期ヘッダーの登録
	_filter = std::shared_ptr<BDFilter>( new BDFilter() );
	std::shared_ptr<block::Block> forkPointBlock = _forkPoint.block();
	if( forkPointBlock == nullptr ) return;
	_filter->filter( forkPointBlock );

	BHPoolFinder bhPoolFinder = std::bind(
										  &BlockHeaderPool::findByBlockHash,
										  std::ref(_blockHeaderPool),
										  std::placeholders::_1 );

	PBHPoolFinder pbhPoolFinder = std::bind(
											&BlockHeaderPool::findByPrevBlockHash,
											std::ref(_blockHeaderPool),
											std::placeholders::_1 );

	_subChainManager = std::shared_ptr<VirtualSubChainManager>( new VirtualSubChainManager( initialForkPoint.header() , bhPoolFinder, pbhPoolFinder ) );
  
	return;
}


void VirtualChain::startBlockHashDownloader()
{
  assert( _subChainManager != nullptr );

  std::thread blockHashDownloader([&]()
  {
  });
}


void VirtualChain::startBlockHeaderDownloader()
{
  assert( _subChainManager != nullptr );

  std::thread blockHeaderDownloader([&]()
  {
	  std::mutex mtx;
	  std::condition_variable cv;
	  std::unique_lock<std::mutex> lock(mtx);
	  cv.wait_for( lock , std::chrono::seconds(5) , [&]{ // trueで解除
		return (_blockHashPool._pool.size() != 0 );
	  });
	return;
  });
  return;
}


void VirtualChain::send( MiyaChainCommand commandBody , auto command )
{
  std::unique_ptr<SBSegment> requestSB = std::make_unique<SBSegment>();
  requestSB->options.option1 = commandBody;
  requestSB->options.option2 = command;

  requestSB->sendFlag( ekp2p::EKP2P_SEND_BROADCAST );
  requestSB->forwardingSBCID(DEFAULT_DAEMON_FORWARDING_SBC_ID_REQUESTER);
  _toRequesterSBC->pushOne( std::move(requestSB) );
}


void VirtualChain::add( std::vector<std::shared_ptr<block::BlockHeader>> targetVector )
{
  std::vector< std::shared_ptr<block::BlockHeader> > duplicateHeaders;
  std::shared_ptr<struct BDBCB> filterCtx;
  std::pair< bool , short > poolCtx;

  // 1. 各headerの追加
  // 2. subchainの延長通知
  for( auto itr : targetVector )
  {
	filterCtx = _filter->filter( itr );
	if( filterCtx == nullptr ) continue; // フィルターに引っかかった場合はcontinue
	if( filterCtx->status() > static_cast<int>(BDState::BlockHeaderReceived) ) continue; // 既に対象要素に対する処理が終わっている場合はcontinueする


	poolCtx = this->_blockHeaderPool.push( itr );
	if( poolCtx.first && poolCtx.second > 1 ) // 追加成功 && 追加後の要素数が2以上 => 重複追加
	  duplicateHeaders.push_back( itr ); // 重複した要素は保存しておく

	filterCtx->status( static_cast<int>(BDState::BlockHeaderReceived) ); // headerPoolに追加したらFilter要素の状態を更新する
  }

  _blockHeaderPool.__printPoolSortWithPrevBlockHash();


  for( auto itr : duplicateHeaders ){
	std::cout << "新たにサブチェーンがビルドされます" << "\n";
	_subChainManager->build( itr ); // 重複ブロックが存在した場合は仮想チェーンを作成する
  }

  _subChainManager->extend(); // headerPoolに更新があった旨をsubchainに通知し,subchainの延長を試みる

  return;
}


void VirtualChain::__printState()
{
  std::cout << "====== < 仮想チェーン内部状態 > ====== " << "\n";

  _subChainManager->__printSubChainSet();
  
  std::cout << "-----------------------------------------" << "\n";
} 




/*
void VirtualChain::add( std::shared_ptr<block::BlockHeader> target ) // 新規に追加したブロックヘッダーを追加
{
  std::shared_ptr<struct BDBCB> filterRet;
  filterRet = _filter->filter( target );
  if( filterRet == nullptr ) return; // フィルターされていなければ無視する

  switch( filterRet->status() )
  {
	case static_cast<int>(BDState::BlockHashReceived) :
	{
	  std::pair<bool,short> pushCtx = _blockHeaderPool.push( target );
	  filterRet->status( static_cast<int>(BDState::BlockHeaderReceived) ); // ステータスの更新

	  if( pushCtx.first ) // 追加時に重複した場合はその要素までのチェーンを構築する
	  {
		std::function<std::vector<std::shared_ptr<block::BlockHeader>>(std::shared_ptr<unsigned char>)> findCallback = std::bind(
				&BlockHeaderPool::find,
				std::ref(_blockHeaderPool),
				std::placeholders::_1
			);
		_subChainManager->build( findCallback , target );
	  }
	  break;
	}
	default:
	{
	  break;
	}
  }

  return;
}
*/

















/*
void BDVirtualChain::IncompleteBlockVector::push( VirtualMiyaChain::iterator target )
{
	std::unique_lock<std::shared_mutex> lock(_mtx);
	_undownloadBlockVector.push_back( target );
}


std::vector< VirtualMiyaChain::iterator > BDVirtualChain::IncompleteBlockVector::pop( size_t size )
{
	std::unique_lock<std::shared_mutex> lock(_mtx);

	auto windowBegin = _undownloadBlockVector.begin(); // クリティカルセクションで保護する
	auto windowEnd = std::next( windowBegin , DEFAULT_DOWNLOAD_BLOCK_WINDOW_SIZE );
	if( windowEnd == _undownloadBlockVector.end() ) windowEnd--; // endは含まれないようにする

	_undownloadBlockVector.erase( windowBegin , windowEnd );

	std::vector<VirtualMiyaChain::iterator> itrVector( windowBegin , windowEnd );

	return itrVector;
}

size_t BDVirtualChain::IncompleteBlockVector::size()
{
	std::unique_lock<std::shared_mutex> lock(_mtx);
	return _undownloadBlockVector.size();
}









BDVirtualChain::BDVirtualChain( std::shared_ptr<LightUTXOSet> utxoSet , std::shared_ptr<BlockLocalStrageManager> localStrageManager ,std::shared_ptr<block::Block> startBlock, std::shared_ptr<unsigned char> stopHash )
{
	_bdFilter = std::make_shared<BDFilter>( this );
	std::this_thread::sleep_for(std::chrono::milliseconds(300));

	// 仮想チェーンの初期化 : 基準となるブロックを追加する これ以降のブロックが収集&検証される
	std::vector<struct BDBCB> initialBCBVector;
	struct BDBCB initialBCB;
	initialBCB.block = startBlock;
	initialBCB.status = static_cast<int>(BDState::BlockStored); // BlockStored状態であればフィルター止まりに制御できる
	_chainVector.push_back( std::make_pair( initialBCB.blockHash() , std::make_shared<struct BDBCB>(initialBCB)) ); // チェーンの先頭にも追加しておく

	initialBCBVector.push_back( initialBCB );
	_bdFilter->add( initialBCBVector ); // フィルターに追加する

	std::shared_ptr<unsigned char> startHash;
	startBlock->blockHash( &startHash );
	_currentStartHash = startHash;
	_stopHash = stopHash;

	_incompleteBlockVector._validateBlockHead = _chainVector.begin();
	assert( localStrageManager != nullptr );
	_localStrageManager = localStrageManager;

	assert( utxoSet != nullptr );
	_utxoSet = utxoSet;
	_utxoSet->toVirtual();

}



BDVirtualChain::BDVirtualChain( std::shared_ptr<LightUTXOSet> utxoSet , std::shared_ptr<BlockLocalStrageManager> localStrageManager , std::shared_ptr<block::Block> startBlock , std::vector<std::shared_ptr<block::BlockHeader>> headerVector )
{

	_bdFilter = std::make_shared<BDFilter>( this );
	std::this_thread::sleep_for(std::chrono::milliseconds(300));

	std::vector< struct BDBCB > initialBCBVector;
	struct BDBCB initialBCB;
	initialBCB.block = startBlock;
	initialBCB.status = static_cast<int>(BDState::BlockStored);
	_chainVector.push_back( std::make_pair( initialBCB.blockHash() , std::make_shared<struct BDBCB>(initialBCB)) );

	// 終端までのブロックヘッダを追加する　
	initialBCBVector.push_back( initialBCB );
	_bdFilter->add( initialBCBVector );


	// 初期で与えれれたヘッダの追加
	this->add( headerVector , static_cast<int>(BDState::BlockHeaderReceived) ); // 一応検証をする

	// 一応スタートハッシュ、ストップハッシュもセットしておく
	std::shared_ptr<unsigned char> startHash;
	std::shared_ptr<unsigned char> stopHash;
	startBlock->blockHash( &startHash );
	if( !(headerVector.empty()) )
		headerVector.back()->headerHash( &stopHash );
	else
		stopHash = nullptr;

	_currentStartHash = startHash;
	_stopHash = stopHash;

	assert( localStrageManager != nullptr );
	_localStrageManager = localStrageManager;

	assert( utxoSet != nullptr );
	_utxoSet = utxoSet;
	_utxoSet->toVirtual();

	_bdFilter->isHeaderDownloadClosing( true ); // ストップヘッダハッシュが
}



BDVirtualChain::~BDVirtualChain()
{
	_utxoSet->virtualAbort(); // 保存操作がなされていない変更分は破棄する
}


VirtualMiyaChain BDVirtualChain::chainVector()
{
	return _chainVector;
}


std::shared_ptr<unsigned char> BDVirtualChain::chainHeadHash()
{
	if( _chainVector.empty() ) return nullptr;
	return _chainVector.back().first;
}


const std::pair< std::shared_ptr<unsigned char> , std::shared_ptr<struct BDBCB> > BDVirtualChain::chainHead()
{
	std::unique_lock<std::shared_mutex> lock(_mtx);
	return _chainVector.back();
}


void BDVirtualChain::requestedExtendChain( std::function<struct BDBCB( std::shared_ptr<unsigned char> )> findPopCallback )
{
	std::cout << "<BDVitrualChain> requestedExtendChain::called()" << "\n";
	struct BDBCB popedCB;

	std::cout << "--- 1 ---" << "\n";
	// 仮想チェーンの最先端ハッシュを取得して,prevが一致するcbを取得する
	std::shared_ptr<unsigned char> chainHeadHash = this->chainHeadHash();
	printf("%p\n", _currentStartHash.get() );
	printf("%p\n", _stopHash.get() );
	if( _stopHash != nullptr )
		if( memcmp( chainHeadHash.get(), _stopHash.get() , 32 ) == 0  ) return; // 仮想チェーンがストップハッシュに達していたら何もしない

	std::cout << "--- 2 ---" << "\n";
	popedCB = findPopCallback( chainHeadHash ); //prevがchainHeadHashを指すブロックを取得する
	if( popedCB.status == static_cast<int>(BDState::Empty) ) return;

	std::cout << "--- 3 ---" << "\n";
	std::unique_lock<std::shared_mutex> lock(_mtx);

	std::shared_ptr<unsigned char> blockHash;
	blockHash = popedCB.blockHash();
	std::shared_ptr<struct BDBCB> newChainHeadCB = std::make_shared<struct BDBCB>(popedCB);
	_chainVector.push_back( std::make_pair( blockHash , newChainHeadCB )); // 仮想チェーンを伸ばす
	_incompleteBlockVector.push( _chainVector.end() - 1 ); // 未ダウンロードリストに追加する
	_bdFilter->updateBlockPtr( newChainHeadCB ); // フィルタに直ポインタを設定する
}



void BDVirtualChain::blockHeaderDownloader( std::shared_ptr<StreamBufferContainer> toRequesterSBC )
{
	assert( toRequesterSBC != nullptr );

	//　リクエストコマンド生成関数
	auto formatGetBlocksCommand = ([&]( std::shared_ptr<unsigned char> startHash , std::shared_ptr<unsigned char> stopHash ) -> MiyaChainCommand
	{
		MiyaChainMSG_GETBLOCKS command;
		MiyaChainCommand wrapCommand;

		command.startHash( startHash );
		command.endHash( stopHash );

		wrapCommand = command;

		return wrapCommand;
	});


	MiyaChainCommand requestCommand;

	std::unique_ptr<SBSegment> responseSB;

	unsigned short timeoutCount = 0;

	while( DEFAULT_BD_MAX_TIMEOUT_COUNT > timeoutCount && !(_bdFilter->isHeaderDownloadClosing())  )
	{
		std::unique_ptr<SBSegment> requestSB = std::make_unique<SBSegment>();
		std::shared_ptr<unsigned char> requestStartHash = _currentStartHash;
		requestCommand = formatGetBlocksCommand( requestStartHash , _stopHash );

		requestSB->options.option1 = requestCommand;
		requestSB->options.option2 = MiyaChainMSG_GETBLOCKS::command;
		requestSB->sendFlag( ekp2p::EKP2P_SEND_BROADCAST ); // ブロードキャストで送信依頼
		requestSB->forwardingSBCID(DEFAULT_DAEMON_FORWARDING_SBC_ID_REQUESTER);
		toRequesterSBC->pushOne( std::move(requestSB) );
		// ここまででリクエストメッセージの送信は終了

		std::this_thread::sleep_for(std::chrono::milliseconds(300)); // 仮想チェーンに上で受信したヘッダが繋がれるまで0.3秒待機 あまり良くない
		_currentStartHash = this->chainHeadHash(); // 仮想チェーンの情報を取得する

		if( _currentStartHash == _stopHash ) break; // 目的のヘッダーまで回収したらブロックヘッダリクエストルーチンは終了する

		if( _currentStartHash == requestStartHash ) // ブロックヘッダリクエストを送信したが,仮想チェーンの状態が変更されていない場合は,送信相手を変更,または少し時間を開けて再送する
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(900));
			_currentStartHash = this->chainHeadHash();
			// 本来はここでkademliaIteratorを操作して送信相手を変更する
			continue;
		}
	}

	// ここまででヘッダーのダウンロードは終了しているはず
	return;
}



void BDVirtualChain::blockDownloader( std::shared_ptr<StreamBufferContainer> toRequesterSBC )
{
	assert( toRequesterSBC != nullptr );

	auto formatGetdataCommand = ([&]( std::vector<VirtualMiyaChain::iterator> itrVector ) -> MiyaChainCommand
	{
		MiyaChainMSG_GETDATA command;
		MiyaChainCommand wrapCommand;

		struct MiyaChainMSG_INV inv;
		for( auto itr : itrVector ){
			inv.addBlock( itr->first );
		}
		command.inv( inv );
		wrapCommand = command;

		return wrapCommand;
	});

	int reRequestCount = 0;
	for(;;)
	{
		std::vector<VirtualMiyaChain::iterator> itrVector; itrVector.clear();
		std::unique_lock<std::shared_mutex> lock(_mtx);
		_cv.wait( lock ,[&]{
			return (itrVector = _incompleteBlockVector.pop()).empty();
		});
		lock.unlock();

		do// 再送は2回までとする
		{
			// メッセージコマンドの作成とリクエスト依頼
			MiyaChainCommand command = formatGetdataCommand( itrVector );
			std::unique_ptr<SBSegment> requestSB = std::make_unique<SBSegment>();
			requestSB->options.option1 = command;
			requestSB->options.option2 = MiyaChainMSG_GETDATA::command;
			requestSB->sendFlag( ekp2p::EKP2P_SEND_BROADCAST ); // 再送の場合は送信者イテレータをずらす
			requestSB->forwardingSBCID( DEFAULT_DAEMON_FORWARDING_SBC_ID_REQUESTER );
			toRequesterSBC->pushOne( std::move(requestSB) ); // リクエスト送信

			std::unique_lock<std::shared_mutex>	lock(_mtx);
			_cv.wait_for( lock ,std::chrono::seconds(reRequestCount+1) , [&]{ // trueでwait解除
				for( auto itr : itrVector ){ // タイムアウトするたび再送の間隔を伸ばす
					if( itr->second->status == static_cast<int>(BDState::BlockBodyReceived) ) return true;
				}
				return false;
			});


			for( auto itr = itrVector.begin() ; itr != itrVector.end() ;) // ブロックが到着したら管理対象から外す
			{
				if( (*itr)->second->status == static_cast<int>(BDState::BlockBodyReceived) ){
					auto eraseTarget = itr;
					itr = itrVector.erase(eraseTarget);
				}
			}

			reRequestCount += 1;
		}while( reRequestCount <= 2 );

		// ダウンロードに失敗したブロックは一旦戻す
		for( auto itr : itrVector )
			_incompleteBlockVector.push( itr );
	}
}





bool BDVirtualChain::mergeToLocalChain()
{
	if( !(_bdFilter->isHeaderDownloadClosing()) )  return false; // フィルタの受付を終了していないとマージはしない

	_utxoSet->virtualCommit(); // 仮想インデックスツリーをローカルインデックスツリーにマージする

	for( auto itr : _chainVector )
	{
		if( itr.second->status != static_cast<int>(BDState::BlockBodyValidated) ) continue;
		_localStrageManager->writeBlock( itr.second->block ); // ブロックを書き込む
	}

	return false;
}



bool BDVirtualChain::startSequentialAssemble( std::shared_ptr<StreamBufferContainer> toRequesterSBC )
{
	std::cout << "ブロックヘッダのダウンロードを開始します" << "\n";
	for( int i=0; i < DEFAULT_BLOCK_HEADER_DOWNLOAD_AGENT_COUNT; i++ )
	{
		std::thread headerDownloader([&](){
			this->blockHeaderDownloader( toRequesterSBC );
		});
		headerDownloader.join(); // ヘッダー受付が締め切られるか,stopHashに達すると勝手にスレッドは終了する
	}

	std::cout << "ブロック本体のダウンロードを開始します" << "\n";
	for( int i=0; i < DEFAULT_BLOCK_HEADER_DOWNLOAD_AGENT_COUNT; i++ )
	{
		std::thread blockDownloader([&](){
			this->blockDownloader( toRequesterSBC );
		});
		blockDownloader.join(); // ヘッダー受付が締め切られるか,stopHashに達すると勝手にスレッドは終了する
	}

	std::cout << "検証プロセスを実行します" << "\n";
	for( auto itr = _incompleteBlockVector._validateBlockHead ; itr != _chainVector.end() ;)
	{
		if( !(itr->second->block->verify(_utxoSet)) ) return false; // 実際にはここでfalutだとブロック本体の再送を要求する
		itr->second->status = static_cast<int>(BDState::BlockBodyValidated);
	}

	std::cout << "ローカルへ保存します" << "\n";
	return this->mergeToLocalChain(); // ローカルへ保存
}



bool BDVirtualChain::startParallelAssemble( std::shared_ptr<StreamBufferContainer> toRequesterSBC )
{
}





bool BDVirtualChain::add( std::vector< std::shared_ptr<block::BlockHeader> > headerVector , int defaultStatus )
{
	std::vector<struct BDBCB> targetVector;
	for( auto itr : headerVector ){
		struct BDBCB target;
		target.block->header(itr);
		if( defaultStatus == static_cast<int>(BDState::None) )
			target.status = static_cast<int>(BDState::BlockHeaderReceived);
		targetVector.push_back(target);
	}

	_bdFilter->add( targetVector );
}

bool BDVirtualChain::add( std::vector<std::shared_ptr<block::Block>> blockVector, int defaultStatus )
{
	std::vector<struct BDBCB> targetVector;
	for( auto itr : blockVector ){
		struct BDBCB target;
		target.block = itr;
		if( defaultStatus == static_cast<int>(BDState::None) )
			target.status = static_cast<int>(BDState::BlockBodyReceived);
		targetVector.push_back(target);
	}

	_bdFilter->add( targetVector );

}


void BDVirtualChain::printVirtualChain()
{
	std::unique_lock<std::shared_mutex> lock(_mtx);
	for( auto itr : _chainVector )
	{
		std::cout << "\n------------------------" << "\n";
		std::cout << "(Key) - ";
		for( int i=0; i<32; i++ ) printf("%02X", itr.first.get()[i] );
		std::cout << "\n";
		std::cout << "(PrevBlockHash) - ";
		for( int i=0; i<32; i++ ) printf("%02X", itr.second->prevBlockHash().get()[i] );
		std::cout << "\n";
		std::cout << "(BlockHash) - ";
		for( int i=0; i<32; i++ ) printf("%02X", itr.second->blockHash().get()[i] );
		std::cout << "\n";
		std::cout << "(Status) - " << itr.second->status << "\n";
		std::cout << "\n------------------------" << "\n";
		std::cout << "  || " << "\n";
		std::cout << "  || " << "\n";

	}
}


size_t BDVirtualChain::chainLength()
{
	return _incompleteBlockVector.size();
}







void BDVirtualChain::printFilter()
{
	return _bdFilter->printFilter();
}

void BDVirtualChain::printHeaderValidationPendingQueue()
{
	return _bdFilter->printHeaderValidationPendingQueue();
}

void BDVirtualChain::printMergePendingQueue()
{
	return _bdFilter->printMergePendingQueue();
}
*/



};
