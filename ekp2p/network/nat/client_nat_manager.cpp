#include "client_nat_manager.h"

#include "../socket_manager/socket_manager.h"

#include "./stun_message.h"

#include "../../kademlia/k_node.h"
//#include "../../kademlia/node.h"
// #include "../../kademlia/k_tag.h"

#include "../../network/header/header.h"
#include "../../network/message/message.h"

#include "../../../shared_components/json.hpp"
#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../../ekp2p.h"
#include "../../daemon/sender/sender.h"

namespace ekp2p{






std::shared_ptr<KNodeAddr> ClientNatManager::natTraversal( std::string stunServerAddrListPath ,std::shared_ptr<StreamBufferContainer> incomingSBC , std::shared_ptr<StreamBufferContainer> toBrokerSBC )
{
	std::cout << "StunServerAddrList open with :: " << stunServerAddrListPath << "\n";
	std::ifstream stunServerAddrFile(stunServerAddrListPath);
	if( !stunServerAddrFile.is_open() ){
		std::cout << "Stun Server Address List File を開くことができません" << "\n";
		return nullptr;
	}

	nlohmann::json stunServerAddrListJson;
	stunServerAddrFile >> stunServerAddrListJson; // 例外処理する
	stunServerAddrListJson = stunServerAddrListJson["servers"];

	struct StunResponse response;
	struct StunRequest stunRequestMSG; 
	std::shared_ptr<unsigned char> rawStunRequestMSG;
	size_t rawStunRequestMSGLength = stunRequestMSG.exportRaw( &rawStunRequestMSG );

	struct sockaddr_in stunServerAddr;
	std::string ipv4; unsigned short port;
	for( int i=0 ;i < DEFAULT_GLOBAL_ADDR_INQUIRE_COUNT ; i++ )
	{
		for( int j=0; j<stunServerAddrListJson.size(); j++ )
		{
			std::unique_ptr<SBSegment> sb = std::make_unique<SBSegment>();
			sb->body( rawStunRequestMSG , rawStunRequestMSGLength );
			sb->protocol( DEFAULT_DAEMON_FORWARDING_SBC_ID_NATER );

			// サーバアドレスのセットアップ
			memset( &stunServerAddr , 0x00 , sizeof(stunServerAddr) );
			stunServerAddr.sin_family = AF_INET;
			ipv4 = stunServerAddrListJson[j]["ipv4"]; port = stunServerAddrListJson[j]["port"];
			stunServerAddr.sin_addr.s_addr = inet_addr( ipv4.c_str() );
			stunServerAddr.sin_port = htons( port );

			sb->sendFlag( EKP2P_SEND_UNICAST );
			sb->forwardingSBCID( DEFAULT_DAEMON_FORWARDING_SBC_ID_SENDER );
			sb->destinationAddr( stunServerAddr );

			std::cout << "Inquired to ... " << "\n";
			toBrokerSBC->pushOne( std::move(sb) );

			//inquireSocketManager->send(msg); // リクエストの送信
			sb = incomingSBC->popOne( 5 ); // タイムアウト付きレスポンスの受信
		
			if( sb == nullptr ){
				std::cout << "Inquireing GlobalAddr timeouted" << "\n";
				continue;
			} 
			response.importRaw( sb->body() , sb->bodyLength() );

			struct sockaddr_in globalAddr = response.toSockaddr_in();
			return std::make_shared<KNodeAddr>( &globalAddr );
		}
	}
	return nullptr;
}



/*
bool ClientNatManager::natTraversal( struct sockaddr_in *globalAddr )
{

	UDPInbandManager *inbandManager = new UDPInbandManager( _socketManager );
	_socketManager->toNonBlockingSocket(); // ノンブロッキングソケットへ

	inbandManager->_messageHandler = StunMessageHandler;
	// inbandManager->_messageHandler = stunResponseHandler;

	std::cout << "START NAT TRAVERSAL" << "\n";

	memset( globalAddr, 0x00, sizeof(struct sockaddr_in) ); // グローバルアドレスのアドレス格納用


	// ============ STUN_Serverのセットアップ ============
	struct sockaddr_in stunServerAddr;
	memset( &stunServerAddr , 0x00 , sizeof(stunServerAddr) );
	stunServerAddr.sin_family = AF_INET;
	stunServerAddr.sin_addr.s_addr = inet_addr( "35.74.112.90" );
	stunServerAddr.sin_port = htons( 8080 );
	// =====================================================


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
*/




bool ClientNatManager::validateSockaddrIn( sockaddr_in *targetAddr )
{
	if( ntohs(targetAddr->sin_port) == 0 ) return false;
	if( targetAddr->sin_addr.s_addr == 0 ) return false;

	return true;
}






}; // close ekp2p namespace
