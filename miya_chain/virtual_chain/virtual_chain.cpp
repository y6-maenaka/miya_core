#include "virtual_chain.h"

#include "../miya_chain_manager.h"

#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../daemon/broker/broker.h"
#include "../daemon/requester/requester.h"

#include "../block/block.h"
#include "../transaction/coinbase/coinbase.h"

#include "../message/message.h"
#include "../message/command/command_set.h"

#include "../../ekp2p/daemon/sender/sender.h"

#include "../utxo_set/utxo_set.h"
#include "../miya_coin/local_strage_manager.h"


namespace miya_chain
{



BDBCB::BDBCB()
{
	block = std::make_shared<block::Block>();
	status = static_cast<int>(BDState::Empty);
}


void BDBCB::header( std::shared_ptr<block::BlockHeader> target ) // 使わなくても良い
{
	block->header( target );
}


std::shared_ptr<unsigned char> BDBCB::blockHash() const
{
	std::shared_ptr<unsigned char> ret;
	block->blockHash( &ret );
	return ret;
}

std::shared_ptr<unsigned char> BDBCB::prevBlockHash() const
{
	return block->header()->previousBlockHeaderHash();
}

void BDBCB::print()
{
	std::cout << "blockHash :: ";
	std::shared_ptr<unsigned char> blockHash;
	block->blockHash( &blockHash );
	for( int i=0; i<32; i++){
		printf("%02X", blockHash.get()[i]);
	} std::cout << "\n";

	std::cout << "height :: " << static_cast<size_t>(height) << "\n";

	std::cout << "status :: " << status << "\n";

}







void BDVirtualChain::UndownloadedBlockVector::push( VirtualMiyaChain::iterator target )
{
	std::unique_lock<std::shared_mutex> lock(_mtx);
	_unVector.push_back( target );
}


std::vector< VirtualMiyaChain::iterator > BDVirtualChain::UndownloadedBlockVector::pop( size_t size )
{
	std::unique_lock<std::shared_mutex> lock(_mtx);

	auto windowBegin = _unVector.begin(); // クリティカルセクションで保護する
	auto windowEnd = std::next( windowBegin , DEFAULT_DOWNLOAD_BLOCK_WINDOW_SIZE );
	if( windowEnd == _unVector.end() ) windowEnd--; // endは含まれないようにする

	_unVector.erase( windowBegin , windowEnd );

	std::vector<VirtualMiyaChain::iterator> itrVector( windowBegin , windowEnd );

	return itrVector;
}

size_t BDVirtualChain::UndownloadedBlockVector::size()
{
	std::unique_lock<std::shared_mutex> lock(_mtx);
	return _unVector.size();
}



BDVirtualChain::BDVirtualChain( std::shared_ptr<block::Block> startBlock, std::shared_ptr<unsigned char> stopHash )
{
	std::cout << "--- 1 ---" << "\n";
	_bdFilter = std::make_shared<BDFilter>( this );
	std::this_thread::sleep_for(std::chrono::milliseconds(300)); 

	// 仮想チェーンの初期化 : 基準となるブロックを追加する これ以降のブロックが収集&検証される
	struct BDBCB initialBCB;
	initialBCB.block = startBlock;
	initialBCB.status = static_cast<int>(BDState::BlockStored); // BlockStored状態であればフィルター止まりに制御できる
	std::cout << "--- 2 ---" << "\n";
	_bdFilter->add( initialBCB ); // フィルターに追加する
	_chainVector.push_back( std::make_pair( initialBCB.blockHash() , std::make_shared<struct BDBCB>(initialBCB)) ); // チェーンの先頭にも追加しておく

	std::cout << "--- 3 ---" << "\n";
	std::shared_ptr<unsigned char> startHash;
	startBlock->blockHash( &startHash );
	_currentStartHash = startHash;
	_stopHash = stopHash;
	std::cout << "--- 4 ---" << "\n";
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

	// 仮想チェーンの最先端ハッシュを取得して,prevが一致するcbを取得する
	std::shared_ptr<unsigned char> chainHeadHash = this->chainHeadHash();
	popedCB = findPopCallback( chainHeadHash ); //prevがchainHeadHashを指すブロックを取得する
	if( popedCB.status == static_cast<int>(BDState::Empty) ) return;

	std::unique_lock<std::shared_mutex> lock(_mtx);

	std::shared_ptr<unsigned char> blockHash;
	blockHash = popedCB.blockHash();
	std::shared_ptr<struct BDBCB> newChainHeadCB = std::make_shared<struct BDBCB>(popedCB);
	_chainVector.push_back( std::make_pair( blockHash/* ブロックハッシュ */ , newChainHeadCB )); // 仮想チェーンを伸ばす
	_undownloadedBlockVector.push( _chainVector.end() - 1 ); // 未ダウンロードリストに追加する
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

	while( DEFAULT_BD_MAX_TIMEOUT_COUNT > timeoutCount && !(_bdFilter->isClosing())  )
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

		if( _bdFilter->isClosing() ) break; // フィルターの更新が締め切られていたら終了する
		if( _currentStartHash == _stopHash ) break; // 目的のヘッダーまで回収したらブロックヘッダリクエストルーチンは終了する

		if( _currentStartHash == requestStartHash ) // ブロックヘッダリクエストを送信したが,仮想チェーンの状態が変更されていない場合は,送信相手を変更,または少し時間を開けて再送する
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(900));
			_currentStartHash = this->chainHeadHash();
			/* 本来はここでkademliaIteratorを操作して送信相手を変更する */
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
			return (itrVector = _undownloadedBlockVector.pop()).empty();
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
			_undownloadedBlockVector.push( itr );

	}

	

}



bool BDVirtualChain::add( std::shared_ptr<block::BlockHeader> header )
{
	if( _bdFilter->isClosing() ) return false;
	_bdFilter->add( header );
}

bool BDVirtualChain::add( std::vector< std::shared_ptr<block::BlockHeader> > headerVector )
{
	if( _bdFilter->isClosing() ) return false;
	_bdFilter->add( headerVector );
}


bool BDVirtualChain::add( std::shared_ptr<block::Block> block )
{
	_bdFilter->add( block );
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
	return _undownloadedBlockVector.size();
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




};
