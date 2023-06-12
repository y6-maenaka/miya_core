#include "client_nat_manager.h"

#include "../inband/inband_manager.h"
#include "../socket_manager/socket_manager.h"

#include "./response_handler.h"
#include "./stun_message_handler.cpp"

#include "../../kademlia/node.h"
#include "../../kademlia/k_tag.h"


namespace ekp2p{






ClientNatManager::ClientNatManager( SocketManager *setupedSocketManager )
{
	if( setupedSocketManager == nullptr )
	{
		_socketManager = new SocketManager;
		_socketManager->setupUDPSock( 8080 );
	}

	_socketManager = setupedSocketManager; // messageHandlerの設定
	
}




bool ClientNatManager::natTraversal( struct sockaddr_in *globalAddr )
{

	UDPInbandManager *inbandManager = new UDPInbandManager( _socketManager );
	_socketManager->toNonBlockingSocket(); // ノンブロッキングソケットへ

	inbandManager->_messageHandler = StunMessageHandler;
	// inbandManager->_messageHandler = stunResponseHandler;

	std::cout << "START NAT TRAVERSAL" << "\n";

	memset( globalAddr, 0x00, sizeof(struct sockaddr_in) ); // グローバルアドレスのアドレス格納用


	/* ============ STUN_Serverのセットアップ ============ */
	struct sockaddr_in stunServerAddr;
	memset( &stunServerAddr , 0x00 , sizeof(stunServerAddr) );
	stunServerAddr.sin_family = AF_INET;
	stunServerAddr.sin_addr.s_addr = inet_addr( "35.74.112.90" );
	stunServerAddr.sin_port = htons( 8080 );
	/* ===================================================== */

	
	KAddr *stunKAddr = new KAddr( &stunServerAddr );
	Node *stunNode = stunKAddr->toNode( _socketManager ); // socketManagerは使い回す必要がある

	// stunRequestMSG = // generate stunu request message


	int i;
	for( i=0; i<5; i++) // 検索回数は5回
	{
		std::cout << "inquiring ..." << "\n";

		sleep( 2*i );

		// StunRequestMSGをStunServerに送信する
		// stunNode->send();

		sleep(1);
		inbandManager->standAlone( (void*)globalAddr , true ); // バッファが空でも待機しない
																					
		if( globalAddr == nullptr )	continue;
		else if( validateSockaddrIn( globalAddr ) ) break; // 成功したら
	
		// globalAddrに正常な値が格納されていたら
	}

	delete inbandManager;
	_socketManager->toBlockingSocket();


	if( i >= 5 ){
		return false; // can't get global addr
	} 

	return true; // get global addr 

}





bool ClientNatManager::validateSockaddrIn( sockaddr_in *targetAddr )
{
	if( ntohs(targetAddr->sin_port) == 0 ) return false;
	if( targetAddr->sin_addr.s_addr == 0 ) return false;

	return true;
}




}; // close ekp2p namespace
