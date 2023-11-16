#include "./IBD.h"
#include "./miya_chain_manager.h"

#include "../shared_components/stream_buffer/stream_buffer.h"
#include "../shared_components/stream_buffer/stream_buffer_container.h"

#include "./daemon/broker/broker.h"
#include "./daemon/requester/requester.h"

#include "./block/block.h"
#include "./transaction/coinbase/coinbase.h"

#include "./message/message.h"
#include "./message/command/command_set.h"

#include "../ekp2p/daemon/sender/sender.h"

#include "./utxo_set/utxo_set.h"
#include "./miya_coin/local_strage_manager.h"


namespace miya_chain
{



IBDBCB::IBDBCB()
{
	block = std::make_shared<block::Block>();
	status = static_cast<int>(IBDState::Empty);
}



void IBDBCB::header( std::shared_ptr<block::BlockHeader> target ) // 使わなくても良い
{
	block->header( target );
}

void IBDBCB::setErrorFlag()
{
	status = static_cast<int>(IBDState::Error);
}


std::shared_ptr<unsigned char> IBDBCB::blockHash() const
{
	std::shared_ptr<unsigned char> ret;
	block->blockHash( &ret );
	return ret;
}

std::shared_ptr<unsigned char> IBDBCB::prevBlockHash()
{
	std::shared_ptr<unsigned char> ret;
	ret = block->header()->previousBlockHeaderHash();
	return ret;
}

void IBDBCB::print()
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













IBDHeaderFilter::IBDHeaderFilter( IBDVirtualChain *virtualChain )
{
	_virtualChain = virtualChain;
	assert(_virtualChain != nullptr );

	// 受信ヘッダ検証用スレッド
	std::thread headerValidator([&]()
  {
			for(;;)
			{
				std::cout << "IBDHeaderFilter::Constructor headerValidator started" << "\n";
				std::unique_lock<std::mutex> lock(_validation._mtx);
				std::cout << "( 1 )" << "\n";
				_validation._cv.wait( lock , [&]{
						return !(_validation._waitQueue.empty()); // 空でなければ解除
				});
				std::cout << "(IBDHeaderFilter) Layer1検証プロセス起動" << "\n";

				for( auto itr : _validation._waitQueue ) // 検証プロセス
				{
					std::unordered_map<BlockHashAsKey, IBDBCB, BlockHashAsKey::Hash>::iterator frontCBSet;
					frontCBSet = _validation._waitQueue.front();
					_validation._waitQueue.erase( _validation._waitQueue.cbegin() ); // ここでeraseしたらauto itrバグらない？

					if( !(frontCBSet->second.block->header()->verify()) ){
						std::cout << "(IBDHeaderFilter) ブロックヘッダ検証失敗" << "\n";
						continue; // 検証失敗したら破棄
					}

					std::cout << "(IBDHeaderFilter) ブロックヘッダ検証成功" << "\n";
					_layer2.push( frontCBSet->first , frontCBSet->second  );
				}
				// layer2要素がからでなければvirtualChainに通知する
				if( this->sizeLayer2() <= 0 ) continue;

				std::function<struct IBDBCB( std::shared_ptr<unsigned char> )> popCallback = std::bind( &Layer2::findPop ,
																								std::ref(_layer2),
																								std::placeholders::_1
				);
				// 検証が完了したら、virtualChainのcallbackを呼び出してチェーンに取り込んでもらう
				_virtualChain->requestedExtendChain( popCallback );
				// std::bind() 引数の固定
			}

  });

  headerValidator.detach();
  std::cout << "IBDHeaderFilter::Constructor headerValidator detached" << "\n";
}



// ブロックのprevOutを比較・一致対象とし検索取り出しする
struct IBDBCB IBDHeaderFilter::Layer2::findPop( std::shared_ptr<unsigned char> blockHash )
{
	std::unique_lock<std::mutex> lock(_mtx);
	struct IBDBCB cb;
	struct BlockHashAsKey blockKey;

	// 結局全探索するしかない？ // 先頭から検索する方が良い
	bool flag = false;
	for( auto itr : _um )
	{
		if( itr.second.block->header()->cmpPrevBlockHash( blockHash ) ){
			blockKey = itr.first;
			cb = itr.second;
			flag = true;
		}
	}

	if( !flag ){
		cb.status = static_cast<int>(IBDState::Empty);
		return cb;
	}

	_um.erase( blockKey );
	return cb;
}


void IBDHeaderFilter::Layer2::push( BlockHashAsKey blockKey , struct IBDBCB cb )
{
	std::unique_lock<std::mutex> lock(_mtx);
	_um[blockKey] = cb;
}



void IBDHeaderFilter::Validation::push( std::unordered_map<BlockHashAsKey, struct IBDBCB, BlockHashAsKey::Hash>::iterator target )
{
	std::unique_lock<std::mutex> lock(_mtx);
	_waitQueue.push_back( target );
}


// 複数個一気にAddしてくれるとかなり処理効率が上がる // 引数ごとvectorにするか
void IBDHeaderFilter::add( std::shared_ptr<block::BlockHeader> header ) // これが1番のフィルター機能になる
{
	std::shared_ptr<unsigned char> blockHash;  header->headerHash( &blockHash );
    struct BlockHashAsKey key(blockHash);
    struct IBDBCB cb; cb.header( header );


    auto ret =  _layer1._um.insert( {key , cb} );
    if( ret.second )  // 要素が存在せず,新たに追加した場合
    {
		std::cout << "(IBDHeaderFilter) 新規追加" << "\n";
		_validation.push( ret.first );
        _validation._cv.notify_all(); // leyer2を監視するスレッドが寝ている可能性があるので
    }else{
		std::cout << "(IBDHeaderFilter) 重複要素" << "\n";
	}
    return;
}



void IBDHeaderFilter::add( std::vector< std::shared_ptr<block::BlockHeader> > headerVector )
{
	bool insertFlag = false;
	for( auto itr : headerVector )
	{
		std::shared_ptr<unsigned char> blockHash; itr->headerHash( &blockHash );
		struct BlockHashAsKey key( blockHash );
		struct IBDBCB cb; cb.header( itr );

		auto ret = _layer1._um.insert( {key, cb} );
		if( ret.second ){
			_validation.push( ret.first );
			insertFlag = true; // 少なくとも一つ追加したらnotify_allを呼び出す必要があるため
		}
	}

	if( insertFlag )
		_validation._cv.notify_all(); // すべて追加した後に通知する
}


std::unordered_map< BlockHashAsKey , struct IBDBCB , BlockHashAsKey::Hash > IBDHeaderFilter::layer1Map()
{
	return _layer1._um;
}


std::unordered_map< BlockHashAsKey , struct IBDBCB , BlockHashAsKey::Hash > IBDHeaderFilter::layer2Map()
{
	return _layer2._um;
}


size_t IBDHeaderFilter::sizeLayer1()
{
	return _layer1._um.size();
}



size_t IBDHeaderFilter::sizeLayer2()
{
	return _layer2._um.size();
}











IBDVirtualChain::IBDVirtualChain( std::shared_ptr<unsigned char> blockHash , struct IBDBCB initialCB )
{
	if( blockHash == nullptr || initialCB.status == static_cast<int>(IBDState::Empty) ) return;
	this->addForce( blockHash , initialCB );
	_undownloadedHead = _chainVector.begin();
}




void IBDVirtualChain::requestedExtendChain( std::function<struct IBDBCB( std::shared_ptr<unsigned char> )> findPopCallback )
{
	std::cout << "<IBDVitrualChain> requestedExtendChain::called()" << "\n";
	struct IBDBCB cb;

	// 仮想チェーンの最先端ハッシュを取得して,prevが一致するcbを取得する

	std::shared_ptr<unsigned char> chainHeadHash = this->back().second.blockHash(); // vitrualChainの最後尾を取得
	cb = findPopCallback( chainHeadHash );
	if( cb.status == static_cast<int>(IBDState::Empty) ) return;

	std::shared_ptr<unsigned char> blockHash;
	std::unique_lock<std::shared_mutex> lock(_mtx);
	cb.block->blockHash( &blockHash );
	_chainVector.push_back( std::pair( blockHash ,cb) ); // 仮想チェーンを伸ばす
}


std::shared_ptr<unsigned char> IBDVirtualChain::chainHead()
{
	if( _chainVector.empty() ) return nullptr;
	return _chainVector.back().first;
}

VirtualMiyaChain IBDVirtualChain::chainVector()
{
return _chainVector;
}



void IBDVirtualChain::add( std::shared_ptr<block::Block> target )  // 非効率,検索ロジックを変更する
{
	std::shared_ptr<unsigned char> blockHash;
	target->blockHash( &blockHash );

	for( auto itr : _chainVector ){
		if( memcmp( (itr.first).get( ), blockHash.get() , 32 ) == 0 ) itr.second.block = target;
	}
}


void IBDVirtualChain::addForce( std::shared_ptr<block::Block> target )
{
	if( target == nullptr ) return;
	std::shared_ptr<unsigned char> blockHash;
	target->blockHash( &blockHash );
	struct IBDBCB cb; cb.block = target;
	_chainVector.push_back( std::make_pair(blockHash , cb) );
}


void IBDVirtualChain::addForce( std::shared_ptr<unsigned char> blockHash ,struct IBDBCB cb )
{
	_chainVector.push_back( std::make_pair(blockHash , cb) );
}


const std::pair< std::shared_ptr<unsigned char> , struct IBDBCB >	 IBDVirtualChain::back()
{
	std::unique_lock<std::shared_mutex> lock(_mtx);
	return _chainVector.back();
}


size_t IBDVirtualChain::size()
{
	return _chainVector.size();
}



VirtualMiyaChain::iterator IBDVirtualChain::popUndownloadedHead()
{
	std::unique_lock<std::shared_mutex> lock(_mtx);
	VirtualMiyaChain::iterator retItr = _undownloadedHead;
	_undownloadedHead++;
	return retItr;
}


std::vector< VirtualMiyaChain::iterator > IBDVirtualChain::popUndownloadedHeadBatch( size_t count )
{
	std::unique_lock<std::shared_mutex> lock(_mtx);
	std::cout << "@@ 1 " << "\n";
	std::cout << "@@ 1.1 chainVector.size() :: " << _chainVector.size() << "\n";

	std::vector< VirtualMiyaChain::iterator > ret;
	for( int i=0; i<count; i++ )
	{	std::cout << "@@ 2" << "\n";
		if( _undownloadedHead == _chainVector.end() ) break;
		ret.push_back( _undownloadedHead );
		_undownloadedHead++;
	}
	return ret;
}






void IBDVirtualChain::blockDownload( IBDVirtualChain *virtualChain , std::shared_ptr<StreamBufferContainer> toRequesterSBC, std::shared_ptr<LightUTXOSet> utxoSet, std::shared_ptr<BlockLocalStrageManager> localStrageManager ) // スレッドとして起動する
{
	std::cout << "\x1b[31m" << "BlockDownloadAgentThreads Started" << "\x1b[39m" << "\n";

	auto formatGetdataCommand = ([&]( std::vector<VirtualMiyaChain::iterator> itrVector ) -> MiyaChainCommand
	{
		MiyaChainMSG_GETDATA getdata;
		MiyaChainCommand command;
		MiyaChainMSG_INV inv;

		for( auto itr : itrVector ){
			struct MiyaChainMSG_INV inv;
			inv.addBlock( itr->first );
			getdata.inv( inv );
		}
		command = getdata;

		return command;
	});


	struct IBDBCB target;

	bool receiveFlag = false;
	bool downloadFlag = false;
	size_t replayCount = 0;
	for(;;)
	{
		MiyaChainCommand command;
		std::vector<VirtualMiyaChain::iterator> itrVector = virtualChain->popUndownloadedHeadBatch( DEFAULT_DOWNLOAD_BLOCK_WINDOW_SIZE );

		Reply: // 同じ内容のメッセージを送信する
		receiveFlag = false	;
		assert( replayCount <= 3 ); // タイムアウトが3回以上発生したら強制終了 ※ IBDの進捗をクリアして最初からIBDする
		if( itrVector.empty() )	 std::this_thread::sleep_for( std::chrono::seconds(2) ); // 取得要素が空であれば任意時間待機する
		command = formatGetdataCommand( itrVector );


		std::unique_ptr<SBSegment> requestSB = std::make_unique<SBSegment>();
		requestSB->options.option1 = command;
		requestSB->options.option1 = MiyaChainMSG_GETDATA::command;
		requestSB->sendFlag( ekp2p::EKP2P_SEND_BROADCAST );
		requestSB->forwardingSBCID( DEFAULT_DAEMON_FORWARDING_SBC_ID_REQUESTER );
		toRequesterSBC->pushOne( std::move(requestSB) ); // リクエスト送信

		// ちょいと待つ
		// データが到着していたら検証シーケンスを開始する
		// 検証シーケンスが終了したら，for先頭に戻る :: データが届かない時の意再送は?
		std::unique_lock<std::shared_mutex>	lock(virtualChain->_mtx);
		virtualChain->_cv.wait_for( lock ,std::chrono::seconds(replayCount+1) , [&]{ // trueでwait解除
			for( auto itr : itrVector ){
				if( itr->second.status == static_cast<int>(IBDState::BlockBodyReceived) ) return true;
			}
			return false;
		}); // どれか一つでもデータが到着していたら待機を解除する

		// 検証プロセス
		for( auto itr = itrVector.begin(); itr != itrVector.end(); itr++ )
		{
			receiveFlag = true;
			if( (*itr)->second.status == static_cast<int>(IBDState::BlockBodyReceived) ){
				// 検証
				bool verifyFlag = (*itr)->second.block->verify( utxoSet );
				if( !verifyFlag ){
					(*itr)->second.status = static_cast<int>(IBDState::Error);
					std::cout << "<IBD> ブロック検証時エラー" << "\n";
					return; // 通常は発生し得ないエラー,assertでも良いが回復メソッドも実装する必要がある
				}
			}
			else if( (*itr)->second.status == static_cast<int>(IBDState::BlockNotfound) ){
				std::cout << "<IBD> ブロックが見つからない" << "\n";
				return; // 回復メソッドの実行
			}

			std::cout << "ブロック検証成功" << "\n";

			// 検証が完了したブロックをローカルストレージに保存する
			std::cout << "\x1b[33m" << "ブロック保存" << "\x1b[39m" << "\n";
			// 順に保存しなくてはいけない
			// localStrageManager->writeBlock( (*itr)->second.block );
			(*itr)->second.status = static_cast<int>(IBDState::BlockStored);
		}
		itrVector.clear();

		if( !(receiveFlag) && !(itrVector.empty()) )
		{
			replayCount++;
			goto Reply; // 再送する
		}

		// 繰り返し
	}


	std::cout << "blockDownload Agenet exeted" << "\n";
	return ;
}














bool MiyaChainManager::startIBD()
{

	std::shared_ptr<unsigned char> localChainHead = _chainState->chainHead(); // これで見つからない場合は,自身のチェーンを遡って更新する

	// 仮想チェーンの一番先頭(ベース)はローカルチェーンの先頭に設定する ※ステータスは保存済みんする
	struct IBDBCB initialiBCB;
	initialiBCB.block = _localStrageManager._strageManager->readBlock( localChainHead );
	initialiBCB.status = static_cast<int>(IBDState::BlockStored);
	IBDVirtualChain vitrualChain( localChainHead, initialiBCB ); // virtualChain宣言時に初期ブロックをセットしないと内部でheadItrが初期化されないので注意


	IBDHeaderFilter headerFilter( &vitrualChain ); // ヘッダーフィルターの宣言と検証スレッドの起動


	// Brokerが本コンポーネントのみにしかメッセージを転送しないように設定
	std::shared_ptr<StreamBufferContainer> incomingSBC = std::make_shared<StreamBufferContainer>();
	_brokerDaemon._broker->blockAll();
	_brokerDaemon._broker->allowedForwardingFilter[2] = true; // ブローカーメッセージ転送先を本コンポーネントだけにする(IBD以外のタスクは動かさない)
	_brokerDaemon._broker->forwardingDestination( incomingSBC , 2 ); // ''




	std::shared_ptr<MiyaChainMessage> responseMSG;
	std::unique_ptr<SBSegment> responseSB;

	unsigned short timeoutCount = 0;
	bool completeHeaderDownload = false;
	// ヘッダ収集終了のトリガーは  getdataに対しての応答がnotfoundだった場合
	while( !completeHeaderDownload && DEFAULT_IBD_MAX_TIMEOUT_COUNT > timeoutCount )
	{

		std::unique_ptr<SBSegment> requestSB = std::make_unique<SBSegment>(); // リクエストメッセージパック用
		MiyaChainCommand command;
		MiyaChainMSG_GETBLOCKS getbloksMSG;
		getbloksMSG.startHash( vitrualChain.chainHead() ); //　ダウンロードと検証が完了した最後尾のヘッダから検索


		// 展開用に一旦パックする
		command = getbloksMSG;
		requestSB->options.option1 = command;
		requestSB->options.option2 = MiyaChainMSG_GETBLOCKS::command;
		requestSB->sendFlag( ekp2p::EKP2P_SEND_BROADCAST ); // ブロードキャストで送信依頼
		requestSB->forwardingSBCID(DEFAULT_DAEMON_FORWARDING_SBC_ID_REQUESTER); // Sendする
		_requesterDaemon._toRequesterSBC->pushOne( std::move(requestSB) );  // Brokerに転送



		responseSB = incomingSBC->popOne( timeoutCount ); // タイムアウト付き
		if( responseSB == nullptr ){
			std::cout << "IBD header request timeouted" << "\n";
			timeoutCount += 1;
			continue;
		}  // データを受信できなかった場合


		do{
			responseMSG = MiyaChainBrocker::parseRawMiyaChainMessage( std::move(responseSB) );
			if( responseMSG->commandIndex() == static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_HEADERS) ) // 受信したMSGがgetbloksに対する応答(headers)だったら
			{
				MiyaChainMSG_HEADERS headers;
				std::vector< std::shared_ptr<block::BlockHeader> > headerVector; headerVector.clear();
				headers = (std::get<MiyaChainMSG_HEADERS>(responseMSG->payload()));
				headerVector = headers.headersVector(); // ヘッダの取り出し
				headerFilter.add( headerVector ); // バッチ追加 ※ 多少無理してもこっちを使う
			}

			if( responseMSG->commandIndex() == static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_NOTFOUND) )
			{ // 本来はこんな簡単な判断ではいけない　※要修正
				completeHeaderDownload = true; // ヘッダボディダウンロードシーケンスへ移行
			}
		} while( (responseSB = incomingSBC->popOne(0)) != nullptr ); //　一旦全てのレスポンスを受け取る
	}
	//  ここまででへっだーがすべて回収されたことになる



	// ブロック本体のダウンロードシーケンスを開始する
	// ワーカースレッドを起動する
	std::vector< std::thread > blockDownloadAgentThreads;
	for( int i=0; i< (vitrualChain.size() / 100) + 1; i++ )
	{
		blockDownloadAgentThreads.push_back( std::thread(IBDVirtualChain::blockDownload ,
															 &vitrualChain,
															  _requesterDaemon._toRequesterSBC ,
															  _utxoSet,
															  _localStrageManager._strageManager ) );

		blockDownloadAgentThreads.back().detach();
	}


	// ブロックダウンロードシーケンス
	for(;;)
	{
		responseSB = incomingSBC->popOne();
		responseMSG = MiyaChainBrocker::parseRawMiyaChainMessage( std::move(responseSB) );
		std::shared_ptr<block::Block> block;
		MiyaChainMSG_BLOCK blockCommand;
		if( responseMSG->commandIndex() == static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_BLOCK) )
		{
			blockCommand = std::get<MiyaChainMSG_BLOCK>(responseMSG->payload());
			block = blockCommand.block();
			vitrualChain.add( block );
		}

		// 簡易的にnotfoundが帰ってきたら終了する ※ 要修正
		if( responseMSG->commandIndex() == static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_NOTFOUND) ){
			break;
		}

	}


	// chainStateをアップデートする
	//std::shared_ptr<unsigned char> vitrualChainEndBlock = vitrualChain.back().second.blockHash(); // vitrualChainの最後尾を取得
	std::shared_ptr<block::Block> vitrualChainEndBlock = vitrualChain.back().second.block;
	std::shared_ptr<unsigned char> vitrualChainEndBlockHash; size_t vitrualChainEndBlockHashLength;
	vitrualChainEndBlockHashLength =  vitrualChainEndBlock->blockHash( &vitrualChainEndBlockHash );
	_chainState->update( vitrualChainEndBlockHash , static_cast<size_t>(vitrualChainEndBlock->coinbase()->height()) );

	// IBDが終了したら,占有しているリソースを解放する
	_brokerDaemon._broker->allowAll();
	_brokerDaemon._broker->forwardingDestination( nullptr , 2 ); // 本コンポーネントへの転送を終了する


	/* ヘッダーボディーダウンロードシーケンスへ */

	std::cout << "IBDManager 終了" << "\n";
}















};
