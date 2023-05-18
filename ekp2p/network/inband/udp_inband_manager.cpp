//#include "inband_manager.h"

// from network 
#include "./receive_handler.h"
#include "../message/header.h"
#include "../message/message.h"

#include "./receive_handler.h"

// from kademlia
#include "../../kademlia/k_tag.h"
#include "../../kademlia/k_routing_table.h"


// from socket manager
#include "../socket_manager/socket_manager.h"



namespace ekp2p{



// メインスレッドから切り離されるBaseReceiveHandler
void* UDP_HandlerCaller( void *arg ){


	// threadが受け取る引数
	ReceiveThreadArgs *receiveThreadArgs;
	receiveThreadArgs = static_cast<ReceiveThreadArgs *>( arg );

	struct sockaddr_in peerAddr;
	socklen_t peerAddrLen;

	MSGHeader *header;

	unsigned char* rawMSG;
	unsigned int rawMSGSize;

	int payloadSize;
	int recvLen = 0; 


	for(int i=0;;i++) // receive
	{
    // 受信したセグメントが正しいデータ出なかった場合 データセグメントの整合成チェックも行われる
		if( (header = UDPInbandManager::UDP_GetOutMSGHeader( receiveThreadArgs->socketManager->sock()) ) == nullptr ) continue;

		rawMSGSize = header->overallDataSize();
		rawMSG = (unsigned char *)malloc( rawMSGSize );
		delete header;

		if ( (recvLen = recvfrom( receiveThreadArgs->socketManager->sock(), rawMSG , rawMSGSize , 0 , (struct sockaddr *)&peerAddr , &peerAddrLen ))  < 0 ){ 
			// 正しくデータを取得できなかった場合
			// delete header;
			free( rawMSG );
			continue;
		}
		//delete header; // 下EKP2PMSGで再びheaderを抽出するのは非効率
	
		EKP2PMSG *msg = new EKP2PMSG( rawMSG, rawMSGSize );		


		// ReceiveHandlerを予約領域を引数として実行する
	 	receiveThreadArgs->receiveHandler->_bootHandler( msg->payload(), msg->header()->payloadSize() ,  NULL , receiveThreadArgs , (struct sockaddr *)&peerAddr );

		// MSG内のKTagよりKRoutingTableを更新する
		// if( kRoutingTable != NULL && header->KTagSize > 0 ){}
		
		//receiveThreadArgs->kRoutingTable->autoKTagHandler( msg->kTag() , receiveThreadArgs->socketManager );
		_baseMidBuffer.push( msg->kTag()->exportRaw() , msg->kTag()->exportRawSize() , false );


		delete msg;
	}


	return (NULL);	
}







pthread_t* UDPInbandManager::start(  KRoutingTable* mainKRoutingTable , short int targetPort )
{

	pthread_t* threadID;
	int ret;

	// 既に指定したポート番号でActiveでないか調べる
	if( _activeThreadIDList.count(targetPort)  ) return nullptr;


	// 1. socketの作成
	if ( (_socketManager->create()) < 0 ) return nullptr;
	if ( (_socketManager->bind( targetPort )) < 0 ) return nullptr;


	/* 引数準備 */
	struct ReceiveThreadArgs *receiveThreadArgs = new ReceiveThreadArgs;
	// 解放はどうするか

	/* 別Thread Callerに渡す引数 */
	receiveThreadArgs->socketManager = _socketManager; // InbandManager 毎に作成される
	receiveThreadArgs->receiveHandler = _receiveHandler; // InbandManager 毎に作成される
	receiveThreadArgs->kRoutingTable = mainKRoutingTable; // 全てで共通したRoutingTableを使用する


	if( (ret = pthread_create( threadID, NULL, UDP_HandlerCaller, (void *)receiveThreadArgs )) != 0 ) return nullptr;
	_activeThreadIDList[ targetPort ] = threadID;

	return threadID;
}


std::thread start( MiddleBuffer *middleBuffer , short int targetPort )
{
	
	std::thread receiveHandler(){

	}
}






}; // close ekp2p namespace
