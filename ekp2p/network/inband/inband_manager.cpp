#include "inband_manager.h"


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


InbandManager::InbandManager(){
	_socketManager = new SocketManager;
	_receiveHandler = new ReceiveHandler;
};



InbandManager::~InbandManager(){
	delete _socketManager;
	delete _receiveHandler;

	// 実行中のスレッドを全て停止させる
};




void InbandManager::receiveHandler( ReceiveHandler *receiveHandler ){
	_receiveHandler = receiveHandler;
}






int InbandManager::stop( short int targetPort ){

	pthread_t *targetThreadID = _activeThreadIDList[ targetPort ];

	int ret;
	ret = stop( *targetThreadID );
	std::cout << strerror(errno)  << "\n";

	return ret;

	// errno出力が必要ない時はこっち
	//return stop( targetThreadID );
}



int InbandManager::stop( pthread_t targetThreadID ){

	int ret;

	// 対象スレッドから戻り値は受け取らない
	ret = pthread_join( targetThreadID , NULL );
	std::cout << strerror(errno)  << "\n";
	return ret;

	// errno出力が必要ない時はこっち
	//return pthread_join( targetThreadID , NULL );
}


	

bool InbandManager::ValidateMSGHeader( struct MSGHeader *header , unsigned int allowedDataSize )
{	
	/* 先頭文字列の比較 */ // ※ 要修正
	if( strcmp( (const char *)(header->startString() ),  (const char *)&(ekp2p::MSG_StartString) ) != 0 ) return false;

	if( header->payloadSize()  > allowedDataSize ) return false;

	return true;
}






} // close ekp2p namespace
