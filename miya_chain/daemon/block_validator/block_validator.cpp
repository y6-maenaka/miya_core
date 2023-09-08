#include "block_validator.h"



#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"


#include "../../message/message.h"

#include "../../message/protocol_message/block_header_msg.h"
#include "../../message/protocol_message/block_data_msg.h"


namespace miya_chain
{







BlockValidationControlBlock::BlockValidationControlBlock( std::shared_ptr<StreamBufferContainer> senderSBC , BlockValidator *validator )
{
	time(&_startTime);
	_networks._toSenderSBC = senderSBC;
	_validator = validator;
}




void BlockValidationControlBlock::importStrucretHeader( std::shared_ptr<BlockHeaderMessage> target )
{
	_blockHash = std::shared_ptr<unsigned char>(target->_blockHash);
	_rawBlock._txCount = target->_txCount;
	_structedBlock._block->_header = target->_blockHeader;
}

void BlockValidationControlBlock::blockHash( unsigned char* target )
{
	_blockHash = std::make_shared<unsigned char>( *target );
}



void BlockValidationControlBlock::networks( std::shared_ptr<ekp2p::KNodeAddr> sourceNodeAddr , std::vector< std::shared_ptr<ekp2p::KNodeAddr> > relayNodeAddr )
{
	_networks._sourceNodeAddr = sourceNodeAddr;
	_networks._relayKNodeAddrVector = relayNodeAddr;
}




void BlockValidationControlBlock::autoStart()
{

	// ノードに問い合わせる
	if( _networks._sourceNodeAddr == nullptr ) return;  // 初回問い合わせノードが存在しなければスレッドには入らない

	std::thread validationTH([this]()  // 検証用スレッドとして起動する
	{

		//std::unique_ptr<SBSegment> pack = std::make_unique<SBSegment>(); // senderに送る用のSBSegmentを作成する
		SBSegment pack;
		pack._ekp2pBlock._sourceNodeAddr = _networks._sourceNodeAddr; // 後に送信元に書き換えられる
		pack._ekp2pBlock._relayKNodeAddrVector = _networks._relayKNodeAddrVector;


		/*　リクエスト初回 ブロックの送信元に問い合わせる */
		BlockDataRequestMessage requestMSG; // データ(トランザクション)リクエスト用のメッセージを作成する
		requestMSG.blockHash( _blockHash ); // ブロックハッシュのセット
		requestMSG._txSequenceFrom = 0; // 内包トランザクションリクエストのシーケンス番号は先頭(0)から
	
		std::shared_ptr<unsigned char> exportedRawMSG; // メッセージの書き出し
		unsigned int exportedRawMSGLength = requestMSG.exportRaw( exportedRawMSG ); 

		pack.body( exportedRawMSG , exportedRawMSGLength ); 
		_networks._toSenderSBC->pushOne( std::move(std::make_unique<SBSegment>(pack)) ); // 送信元にリクエストメッセージを送信する

		// 3秒くらい待機する
		std::unique_lock<std::mutex>	lock(_mtx);
		//_cv.wait_for( lock , std::chrono::seconds(5),  [this]{ return !(_structedBlock._updateFlag);} );
		_cv.wait_for( lock , std::chrono::seconds(5) ); // アップデートされていなければ再送する
		if( _structedBlock._updateFlag == false ) _networks._toSenderSBC->pushOne( std::move(std::make_unique<SBSegment>(pack)) );


		// 初回のトランザクションの収集が完了したら	


	});
	validationTH.detach();



 return;
}



void BlockValidationControlBlock::txArrive( std::shared_ptr<BlockDataResponseMessage> msg )
{
	// 受信したメッセージを解析して_structedBlockにセットする

	_structedBlock._updateFlag = true;
	_cv.notify_all(); // 要求したtxが到着したら検証用スレッドを起こす
}





void PendingResponseQueue::timeOutRefresh()
{
	//std::array< std::shared_ptr<BlockValidationControlBlock> , MAX_PENDING_QUEUE_SIZE >::iterator itr;
	//itr = _bvcbQueue.begin();
	time_t currentTime; time(&currentTime); // 比較用に現在時刻を取得


	for( auto itr = _bvcbQueue.begin(); itr != _bvcbQueue.end(); )
	{
		if( (currentTime - (*itr)->_startTime ) >= MAX_PENDING_TIME ) _bvcbQueue.erase(itr);
		else itr++;
	}

	return;
}





std::shared_ptr<BlockValidationControlBlock> PendingResponseQueue::find( unsigned char* targetHash )
{
	//std::array< std::shared_ptr<BlockValidationControlBlock> , MAX_PENDING_QUEUE_SIZE >::iterator itr;
	//itr = _bvcbQueue.begin();

	for( auto itr : _bvcbQueue ) // 要素そのものの型で
	{
		if( memcmp( itr->_blockHash.get() , targetHash  , 32 ) == 0 ) return itr;
	}
	
	return nullptr;
}







void PendingResponseQueue::enqueue( std::shared_ptr<BlockValidationControlBlock> target )
{
	timeOutRefresh();

	if( _bvcbQueue.size() >= MAX_PENDING_QUEUE_SIZE ) return;

	//_bvcbQueue[_bvcbCount] = target;
	_bvcbQueue.push_back( target ); // 単純追加
}



std::shared_ptr<BlockValidationControlBlock> PendingResponseQueue::dequeue( std::shared_ptr<BlockValidationControlBlock> target )
{
	
	//std::array< std::shared_ptr<BlockValidationControlBlock> , MAX_PENDING_QUEUE_SIZE >::iterator itr;
	//itr = _bvcbQueue.begin();
	auto itr = std::find(_bvcbQueue.begin(), _bvcbQueue.end(), target ); // イテレータ型で
	std::shared_ptr<BlockValidationControlBlock> ret = nullptr; 

	// 対象の要素が見つかったら
	if( itr != _bvcbQueue.end() ){	// 対象の要素が見つかったら
		ret = (*itr);
		_bvcbQueue.erase( itr );
	}
	else{
		return nullptr;
	}

	return ret;
}












void BlockValidator::start()
{

	if( _sourceSBC == nullptr ) return;
	std::unique_ptr<SBSegment> popedSB;

	MiyaChainMessage miyaChainMSG; //サイズは固定長 MiyaChainProtocolMessage
	std::shared_ptr<BlockValidationControlBlock> bvcb; // block validation control block

	std::shared_ptr<BlockHeaderMessage> blockHeaderMSG;
	std::shared_ptr<BlockDataResponseMessage> blockDataResponseMSG;

	for(;;)
	{
		popedSB = _sourceSBC->popOne();
	
		// リクエストがあったブロックデータを収集する
		// StreamBufferに格納されているアドレスにセンドバックする	
		memcpy( &miyaChainMSG , (popedSB->body()).get() , popedSB->bodyLength() );
		

		switch( miyaChainMSG.protocol() )
		{
			case 0:
				blockHeaderMSG = miyaChainMSG.blockHeaderMSG(); // ブロックヘッダメッセージの書き出し // 新たなアドレスで書き出す
				
				if( _pendingQueue.find( blockHeaderMSG->blockHash() ) == nullptr )
				{
					bvcb = std::make_shared<BlockValidationControlBlock>( _destinationSBC , this ); // 新たにコントロールブロックを作成する
					bvcb->importStrucretHeader( blockHeaderMSG ); // ブロックハッシュとブロックヘッダをセット
					bvcb->networks( popedSB->sourceKNodeAddr() , popedSB->relayKNodeAddrVector() );
					_pendingQueue.enqueue( bvcb ); // キューに追加する
					bvcb->autoStart(); // 検証ルーチンをスタートする
				}
				break;

			case 1:
				break;

			default:
				break;
		}
	
	
	}

}


void BlockValidator::vlidationDone( std::shared_ptr<BlockValidationControlBlock> target )
{
	std::shared_ptr<BlockValidationControlBlock> targetBVCB;
	targetBVCB = _pendingQueue.dequeue( target ); // キューから対象のブロックを取り出す


}



};
