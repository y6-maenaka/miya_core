#include "inband_manager.h"


// from network
#include "./receive_handler.h"
#include "../message/header.h"
#include "../message/message.h"

#include "./receive_handler.h"

// from kademlia
#include "../../kademlia/k_tag.h"
// #include "../../kademlia/k_routing_table.h"


// from socket manager
#include "../socket_manager/socket_manager.h"




namespace ekp2p{


BaseInbandManager::BaseInbandManager( SocketManager *setupedSocketManager )
{
	if( setupedSocketManager == nullptr )
		_socketManager = new SocketManager;

	_socketManager = setupedSocketManager;


	_messageHandler = DefaultMessageHandler; // デフォルトハンドラの設定

};


BaseInbandManager::~BaseInbandManager()
{
	// setupedSocketManagerがセットされている場合破棄されては困る
	//delete _socketManager;
};

	// 実行中のスレッドを全て停止させる };




SocketManager *BaseInbandManager::socketManager()
{
	return _socketManager;
}



void BaseInbandManager::socketManager( SocketManager *socketManager )
{
	_socketManager = socketManager;
}



bool BaseInbandManager::ValidateMSGHeader( struct MSGHeader *header , unsigned int allowedDataSize )
{
	/* 先頭文字列の比較 */ // ※ 要修正
	if( strcmp( (const char *)(header->startString() ),  (const char *)&(ekp2p::MSG_StartString) ) != 0 ) return false;

	if( header->payloadSize()  > allowedDataSize ) return false;

	return true;
}



void BaseInbandManager::handlerArg( void* arg ) // setter
{
	_handlerArg = arg;
}

void *BaseInbandManager::handlerArg()
{
	return _handlerArg;
}










// socketManagerがノンブロッキングに設定されていると結果を即座に返す
// selectを使ったtimeout機構
bool UDPInbandManager::standAlone( void *ioArg , bool allowEmpty )
{
	_handlerArg = ioArg;
	
	int messageHandlerFlag;
	struct sockaddr_in peerAddr;
	socklen_t peerAddrLen;

	unsigned char *rawMSG = nullptr; unsigned int rawMSGSize = 0;

	MSGHeader *header; // 内部で領域確保する
	int recvLen = 0;


	for(;;){
		// message_handlerが指定の値を返したら処理を終了する	
		
		header = GetOutMSGHeader( _socketManager->sock()); // 取り出しに失敗したらheader == nullptr 

		if( header == nullptr && !(allowEmpty)) continue;
		else if( header == nullptr && allowEmpty ){ //  メッセージ受信を空でも許可する場合
			messageHandlerFlag = _messageHandler( (void *)this, nullptr );
			continue;
		}


		rawMSGSize = header->rawMSGSize();
		rawMSG = new unsigned char[ rawMSGSize ];
		delete header; // ヘッダーはメッセージ到達用 // これ以降は不要

		if ( (recvLen = recvfrom( _socketManager->sock(), rawMSG , rawMSGSize , 0 , (struct sockaddr *)&peerAddr , &peerAddrLen ))  < 0 )
		{ // 正しくデータを取得できなかった場合	
				delete rawMSG;
				continue; // 取得できなかったら飛ばす
		}

		EKP2PMSG *msg = new EKP2PMSG;
		msg->toStructedMSG( rawMSG , rawMSGSize );	delete rawMSG; // ヘッダーの処理も内部で行われる
	
		messageHandlerFlag = _messageHandler( (void*)this, msg ); // バッファが空になるまで繰り返す
		if( messageHandlerFlag != 0 ) break;

		delete msg;
	}

	return true;
}




// tableの更新処理もここに書く？
/*
bool InbandNetworkManager::start( SocketManager *setupedSocketManager )
{
	if( setupedSocketManager == nullptr ) return false;

	if( setupedSocketManager->sockType() == IPPROTO_UDP )
	{
		UDPInbandManager *udpInbandManager = new UDPInbandManager( setupedSocketManager );

		

		// ここでudnInbandManagerをbind状態に移行する
		// if( udpInbandManager->monitorSetup() < 0 ) return false;

		  //======= UDP THREAD START ============
			
		//std::thread monitor( [udpInbandManager](){
		std::thread monitor( [&](){

				std::cout  << "\n" << "\x1b[032m" << "[ CONFIRMATION ] ReceiveThread was successfully started" << "\x1b[0m" << "\n\n";

				struct sockaddr_in peerAddr;
				socklen_t peerAddrLen;

				unsigned char *rawMSG ;
				unsigned int rawMSGSize;

	 			MSGHeader *header;

				int recvLen = 0;

				for(;;)
				{
						// 受信したデータセグメントが指定のフォーマットではなかった場合
						if( (header = udpInbandManager->GetOutMSGHeader( udpInbandManager->socketManager()->sock()) ) == nullptr ) continue;

						rawMSGSize = header->rawMSGSize();
						rawMSG = new unsigned char[ rawMSGSize ];
						delete header; // ヘッダーはメッセージ到達用 // これ以降は不要

						if ( (recvLen = recvfrom( udpInbandManager->socketManager()->sock(), rawMSG , rawMSGSize , 0 , (struct sockaddr *)&peerAddr , &peerAddrLen ))  < 0 ){ 
							// 正しくデータを取得できなかった場合
							delete rawMSG;
							continue; // 取得できなかったら飛ばす
						}
						
						EKP2PMSG *structedMSG = new EKP2PMSG;
						structedMSG->toStructedMSG( rawMSG , rawMSGSize );	delete rawMSG;

						udpInbandManager->_messageHandler( (void *)this , structedMSG ); // 関数抜けるとメッセージは削除される
																																																							// 関数内部で別領域に移動させる必要がある
						delete structedMSG;
				}

		});

		// threadオブジェクトはコピー不可 -> 所有権の移動
		_activeThreadList.push_back( std::make_pair( udpInbandManager->socketManager() , std::move(monitor) ) ); // threadの所有権をactiveThreadListにする
	}




	else if( setupedSocketManager->sockType() == IPPROTO_TCP )
	{
		std::cout << "tcp thread handler start" << "\n";
	}

	return true;

}
*/


/*
MSGHeader *BaseInbandManager::GetOutMSGHeader( int sock )
{
	return nullptr;
}
*/


MSGHeader *UDPInbandManager::GetOutMSGHeader( int sock )
{
	struct MSGHeader *header = new MSGHeader;
	int recvSize = 0;
	int msgLen;

	/*
	　ノンブロッキングかどうか判断する
	*/

	if(  (recvSize = recvfrom( sock , header->headerBody() , sizeof( struct MSGHeader::HeaderBody ), MSG_PEEK , NULL, NULL ))  < 0 )
	{
		delete header;
		return nullptr;
	}



	// 受信したデータセグメントの検証
	if( !(BaseInbandManager::ValidateMSGHeader( header )) ) // ヘッダーの内容が不正だったら
	{
		// データセグメントを破棄する
		recvfrom( sock , NULL, 0 , 0 , NULL , NULL );
		std::cout << "Discarded wrong data segment" << "\n";
		delete header;
		return nullptr;
	}

	return header;
}



MSGHeader *TCPInbandManager::GetOutMSGHeader( int sock )
{
	return nullptr;
}



} // close ekp2p namespace
