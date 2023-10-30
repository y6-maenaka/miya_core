#include "./IBD.h"
#include "./miya_core_manager.h"

#include "../shared_components/stream_buffer/stream_buffer.h"
#include "../shared_components/stream_buffer/stream_buffer_container.h"

#include "./daemon/broker/broker.h"
#include "./daemon/requester/requester.h"

#include "./block/block.h"

#include "./message/message.h"
#include "./message/command/command_set.h"

#include "../ekp2p/daemon/sender/sender.h"

#include "./miya_coin/local_strage_manager.h"


namespace miya_chain
{



IBDBCB::IBDBCB()
{
	block = std::make_shared<block::Block>();
	status = static_cast<int>(IBDState::Init);
}



void IBDBCB::header( std::shared_ptr<block::BlockHeader> target ) // 使わなくても良い
{
	block->header( target );
}









IBDHeaderFilter::IBDHeaderFilter( IBDVirtualChain *virtualChain )
{

	_virtualChain = virtualChain;
	assert(_virtualChain != nullptr ); 

 
	std::thread headerValidator([&]()
  {
			std::cout << "IBDHeaderFilter::Constructor headerValidator started" << "\n";

			std::unique_lock<std::mutex> lock(_validation._mtx);
			_validation._cv.wait( lock , [&]{
					return !(_layer2._um.empty()); // 空でなければ解除
			});

			for( auto itr : _layer2._um ) // 検証プロセス
			{
			}

			// 検証が完了したら、virtualChainのcallbackを呼び出してチェーンに取り込んでもらう
			// _virtualChain.extendChain();
  });

  headerValidator.detach();
  std::cout << "IBDHeaderFilter::Constructor headerValidator detached" << "\n";
}



// 複数個一気にAddしてくれるとかなり処理効率が上がる // 引数ごとvectorにするか
void IBDHeaderFilter::add( std::shared_ptr<block::BlockHeader> header )
{
	std::shared_ptr<unsigned char> blockHash;  header->headerHash( &blockHash );
    struct BlockHashAsKey key(blockHash);
    struct IBDBCB cb; cb.header( header );

    auto ret =  _layer1._um.insert( {key , cb} );
    if( ret.second )  // 要素が存在せず,新たに追加した場合
    { 
        _validation._waitQueue.push_back( ret.first );
        _validation._cv.notify_all();
    }

    return;
}











const std::shared_ptr<unsigned char> IBDVirtualChain::chainHead()
{
	return _chainVector.back().first;
}

 const VirtualMiyaChain IBDVirtualChain::chainVector()
 {
	return _chainVector;
 }


VirtualMiyaChain::const_iterator IBDVirtualChain::popUndownloadedHead()
{
	std::unique_lock<std::shared_mutex> lock(_mtx);
	VirtualMiyaChain::const_iterator retItr = _undownloadedHead;
	_undownloadedHead++;
	return retItr;
}


std::vector< VirtualMiyaChain::const_iterator > IBDVirtualChain::popUndownloadedHeadBatch( size_t count )
{
	std::unique_lock<std::shared_mutex> lock(_mtx);

	std::vector< VirtualMiyaChain::const_iterator > ret;
	for( int i=0; i<count; i++ )
	{
		if( _undownloadedHead == _chainVector.cend() ) break;
		ret.push_back( _undownloadedHead );
		_undownloadedHead++;
	}
	return ret;
}



void IBDVirtualChain::downloadDone( VirtualMiyaChain::const_iterator itr )
{
	// IBDCBのステータスを変更する	
}



void IBDVirtualChain::blockDownload( IBDVirtualChain *virtualChain , std::shared_ptr<StreamBufferContainer> toRequesterSBC ) // スレッドとして起動する
{

	auto formatGetdataCommand = ([&]( std::vector<VirtualMiyaChain::const_iterator> itrVector ) -> MiyaChainCommand
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

	bool downloadFlag = false;
	size_t replayCount = 0;
	for(;;)
	{
		MiyaChainCommand command;
		std::vector<VirtualMiyaChain::const_iterator> itrVector = virtualChain->popUndownloadedHeadBatch( DEFAULT_DOWNLOAD_BLOCK_WINDOW_SIZE );

		Reply: // 同じ内容のメッセージを送信する
		assert( replayCount <= 3 ); // タイムアウトが3回以上発生したら強制終了 ※ IBDの進捗をクリアして最初からIBDする
		if( itrVector.empty() )	 return; // このエージェントは終了する
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
			if( (*itr)->second.status == static_cast<int>(IBDState::BlockBodyReceived) ){
				// 検証
				// 保存
				itrVector.erase( itr ); // 検証が完了したら削除する
			}
		}

		if( !(itrVector.empty()) )
		{ 
			replayCount++;
			goto Reply; // 再送する
		}
		else{
			return; // 晴れてIBD終了
		}

	}


	std::cout << "blockDownload Agenet exeted" << "\n";
	return ;
}














bool MiyaChainManager::startIBD()
{
	std::shared_ptr<unsigned char> localChainHead = _chainState.chainHead(); // これで見つからない場合は,自身のチェーンを遡って更新する


	// 仮想チェーンの一番先頭(ベース)はローカルチェーンの先頭に設定する ※ステータスは保存済みんする
	IBDVirtualChain vitrualChain;
	struct IBDBCB initialiBCB; 
	initialiBCB.block = _localStrageManager._strageManager->readBlock( localChainHead );
	initialiBCB.status = static_cast<int>(IBDState::BlockStoread);

	IBDHeaderFilter headerFilter( &vitrualChain ); // ヘッダーフィルターの宣言と検証スレッドの起動
	

	// Brokerが本コンポーネントのみにしかメッセージを転送しないように設定
	std::shared_ptr<StreamBufferContainer> incomingSBC = std::make_shared<StreamBufferContainer>();
	_brokerDaemon._broker->blockAll();
	_brokerDaemon._broker->allowedForwardingFilter[2] = true;
	_brokerDaemon._broker->forwardingDestination( incomingSBC , 2 );



	unsigned short maxTimeoutCount = 4; // 後でハードコードする
	unsigned short timeoutCount = 0;

	bool completeHeaderFlag = false;
	// ヘッダ収集終了のトリガーは  getdataに対しての応答がnotfoundだった場合
	while( !completeHeaderFlag && maxTimeoutCount > timeoutCount )	
	{
		std::unique_ptr<SBSegment> requestSB = std::make_unique<SBSegment>(); // リクエストメッセージパック用
		MiyaChainCommand command;
		MiyaChainMSG_GETBLOCKS getbloksMSG; 
		getbloksMSG.startHash( vitrualChain.chainHead() ); //　ダウンロードと検証が完了した最後尾のヘッダから検索
		
		command = getbloksMSG; // 展開用に一旦パックする
		requestSB->options.option1 = command;
		requestSB->options.option2 = MiyaChainMSG_GETBLOCKS::command;
		requestSB->sendFlag( ekp2p::EKP2P_SEND_BROADCAST ); // ブロードキャストで送信依頼
		requestSB->forwardingSBCID(DEFAULT_DAEMON_FORWARDING_SBC_ID_REQUESTER); // Sendする

		_requesterDaemon._toRequesterSBC->pushOne( std::move(requestSB) );  // Brokerに転送



		std::shared_ptr<MiyaChainMessage> responseMSG;
		std::unique_ptr<SBSegment> responseSB;
		responseSB = incomingSBC->popOne( timeoutCount ); // タイムアウト付き
		if( responseSB == nullptr ){
			maxTimeoutCount += 1;
			continue;
		} 
		
		do{ 
			responseMSG = MiyaChainBrocker::parseRawMiyaChainMessage( std::move(responseSB) );
			if( responseMSG->commandIndex() == static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_HEADERS) ) // 受信したMSGがgetbloksに対する応答(headers)だったら
			{
				MiyaChainMSG_HEADERS headers; 
				std::vector< std::shared_ptr<block::BlockHeader> > headerVector; headerVector.clear();
				headers = (std::get<MiyaChainMSG_HEADERS>(responseMSG->payload()));
				headerVector = headers.headersVector();
				for( auto itr : headerVector )
				{
					headerFilter.add( itr );
				}
			}

			if( responseMSG->commandIndex() == static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_NOTFOUND) )
			{ // 本来はこんな簡単な判断ではいけない　※要修正
				completeHeaderFlag = true; // ヘッダボディダウンロードシーケンスへ移行
			}
		} while( (responseSB = incomingSBC->popOne(0)) != nullptr ); //　一旦全てのレスポンスを受け取る
	}
	//  ここまででへっだーがすべて回収されたこととする
	


	// ブロック本体のダウンロードシーケンスを開始する
	// ワーカースレッドを起動する	
	std::vector< std::thread > blockDownloadAgentThreads;
	for( int i=0; i<DEFAULT_BLOCK_DOWNLOAD_AGENT_COUNT; i++ )	
	{
		blockDownloadAgentThreads.push_back( std::thread(IBDVirtualChain::blockDownload , &vitrualChain, _requesterDaemon._toRequesterSBC ) );
	}
	for (std::thread &activeThread : blockDownloadAgentThreads )  // スレッドの回収
	{
        activeThread.join();
    }





	// IBDが終了したら,占有しているリソースを解放する
	_brokerDaemon._broker->allowAll();
	_brokerDaemon._broker->forwardingDestination( nullptr , 2 ); // 本コンポーネントへの転送を終了する


	/* ヘッダーボディーダウンロードシーケンスへ */

	std::cout << "IBDManager 終了" << "\n";
}















};
