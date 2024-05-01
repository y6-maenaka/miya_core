#include "ekp2p.h"

// #include "./kademlia/k_routing_table.h"
#include "./network/nat/client_nat_manager.h"
//#include "./network/inband/inband_manager.h"
#include "./network/socket_manager/socket_manager.h"

#include "./network/message/message.hpp"
#include "./network/header/header.h"

//#include "./kademlia/kademlia_RPC/FIND_NODE.cpp"
// #include "./network/outband/node_broadcaster.h"

#include "./kademlia/k_node.h"
#include "../share/stream_buffer/stream_buffer.h"
#include "../share/stream_buffer/stream_buffer_container.h"
// #include "./kademlia/message_receiver.h"
#include "./kademlia/connection_controller.h"
#include "./kademlia/k_routing_table/k_routing_table.h"
// #include "./kademlia/k_routing_table/k_routing_table_updator.h"


#include "./daemon/broker/broker.h"
#include "./daemon/sender/sender.h"
#include "./daemon/receiver/receiver.h"
#include "./daemon/routing_table_manager/routing_table_manager.h"

#include "./network/nat/client_nat_manager.h"


namespace ekp2p{




EKP2P::EKP2P( std::shared_ptr<KRoutingTable> KRoutingTable )
{
	int createdSock;
	_hostSocketManager = std::shared_ptr<SocketManager>( new SocketManager{} ); // ホストソケットの作成
	createdSock = _hostSocketManager->setupUDPSock( 8080 ); // ホストソケットのセットアップ
	std::cout << "SocketCreated at :: " << createdSock << "\n";

	if( KRoutingTable == nullptr ){
		// _hostNode = std::make_shared<KHostNode>();
		_kRoutingTable = std::make_shared<class KRoutingTable>(_hostSocketManager);
		return;	
	}
	// _mainNode = std::make_shared<KHostNode>();
	return;
}





int EKP2P::init( std::string stunServerAdddrListPath )
{
	// 一旦全てのdaemonをセットアップする
	// ブローカーのセットアップ 
	// アップデータのセットアップ
	_brokerDaemon._toBrokerSBC = std::make_shared<StreamBufferContainer>();
	_routingTableManagerDaemon._toManagerSBC = std::make_shared<StreamBufferContainer>();
	_routingTableManagerDaemon._manager = std::make_shared<EKP2PKRoutingTableUpdator>( _kRoutingTable ,_routingTableManagerDaemon._toManagerSBC , _brokerDaemon._toBrokerSBC );
	_brokerDaemon._broker = std::make_shared<EKP2PBroker>( _brokerDaemon._toBrokerSBC , _routingTableManagerDaemon._toManagerSBC );

	// センダーのセットアップ
	_senderDaemon._toSenderSBC = std::make_shared<StreamBufferContainer>();
	_senderDaemon._sender	= std::make_shared<EKP2PSender>( _hostSocketManager->sock() ,_kRoutingTable , _senderDaemon._toSenderSBC , _brokerDaemon._toBrokerSBC );
	//_senderDaemon._sender->start();

	// レシーバーのセットアップ
	_receiverDaemon._toReseiverSBC = std::make_shared<StreamBufferContainer>(); 
	_receiverDaemon._receiver = std::make_shared<EKP2PReceiver>( _hostSocketManager , _brokerDaemon._toBrokerSBC ); // receiverにはSBは不要 reseice制限フラグ
	_receiverDaemon._receiver->toRoutingTableUpdatorSBC( _routingTableManagerDaemon._toManagerSBC );
	//_receiverDaemon._receiver->start();


	// 主要フォワーディング先を設定
	_brokerDaemon._broker->forwardingDestination( _senderDaemon._toSenderSBC , DEFAULT_DAEMON_FORWARDING_SBC_ID_SENDER );

	_receiverDaemon._receiver->start(); // receiverは終了させることができないのであらかじめ起動しておく




	// Nat超えをしてグローバルIPを得る
	std::cout << "本ノードのグローバルアドレスを取得しています" << "\n";
	std::shared_ptr<StreamBufferContainer> _toNaterSBC = std::make_shared<StreamBufferContainer>(); // naterへのSBC
	_brokerDaemon._broker->forwardingDestination( _toNaterSBC , DEFAULT_DAEMON_FORWARDING_SBC_ID_NATER ); // 転送先の設定 Natterは1番へ
	
	_brokerDaemon._broker->start( false ); 
	_senderDaemon._sender->start();

	ClientNatManager natManager;
	std::shared_ptr<KNodeAddr> globalKNodeAddr;
	globalKNodeAddr = natManager.natTraversal( stunServerAdddrListPath , _toNaterSBC , _brokerDaemon._toBrokerSBC );
	// グローバルアドレスを取得できたら,_hostNodeのKNodeAddrを変更する
	_kRoutingTable->hostNode()->kNodeAddr( globalKNodeAddr );

	std::cout << "Global Address Getted with " << "\n";
	_kRoutingTable->hostNode()->printInfo();
	std::cout << "NatTraversal Successfuly Done" << "\n";


	/* 終了フラグの送信 */
	std::unique_ptr<SBSegment> exitBrokerSB = std::make_unique<SBSegment>();
	exitBrokerSB->flag(SB_FLAG_MODULE_EXIT);
	_brokerDaemon._toBrokerSBC->pushOne( std::move(exitBrokerSB) );

	std::unique_ptr<SBSegment> exitSenderSB = std::make_unique<SBSegment>();
	exitSenderSB->flag(SB_FLAG_MODULE_EXIT);
	_senderDaemon._toSenderSBC->pushOne( std::move(exitSenderSB));
	return 0;
}






// なんらかのサーバ起動用 通常は使わない
int EKP2P::initCustom()
{
	_brokerDaemon._toBrokerSBC = std::make_shared<StreamBufferContainer>();
	_routingTableManagerDaemon._toManagerSBC = std::make_shared<StreamBufferContainer>();
	_routingTableManagerDaemon._manager = std::make_shared<EKP2PKRoutingTableUpdator>( _kRoutingTable ,_routingTableManagerDaemon._toManagerSBC , _brokerDaemon._toBrokerSBC );
	_brokerDaemon._broker = std::make_shared<EKP2PBroker>( _brokerDaemon._toBrokerSBC , _routingTableManagerDaemon._toManagerSBC );

	_senderDaemon._toSenderSBC = std::make_shared<StreamBufferContainer>();
	_senderDaemon._sender	= std::make_shared<EKP2PSender>( _hostSocketManager->sock() ,_kRoutingTable , _senderDaemon._toSenderSBC , _brokerDaemon._toBrokerSBC );

	_receiverDaemon._toReseiverSBC = std::make_shared<StreamBufferContainer>(); 
	_receiverDaemon._receiver = std::make_shared<EKP2PReceiver>( _hostSocketManager , _brokerDaemon._toBrokerSBC ); // receiverにはSBは不要 reseice制限フラグ
	_receiverDaemon._receiver->toRoutingTableUpdatorSBC( _routingTableManagerDaemon._toManagerSBC );

	_brokerDaemon._broker->forwardingDestination( _senderDaemon._toSenderSBC , DEFAULT_DAEMON_FORWARDING_SBC_ID_SENDER );

	return 0;
}




int EKP2P::start( bool requiresRouting )
{
	// Daemonは全てバックグラウンドで起動される
	_brokerDaemon._broker->start( requiresRouting ); 
	_senderDaemon._sender->start(); 
	// _receiverDaemon._receiver->start(); // すでに起動している
	_routingTableManagerDaemon._manager->start();

	return 0;
}



std::shared_ptr<StreamBufferContainer> EKP2P::toBrokerSBC()
{
	return _brokerDaemon._toBrokerSBC;
}

std::shared_ptr<StreamBufferContainer> EKP2P::toReseiverSBC()
{
	return _receiverDaemon._toReseiverSBC;
}


std::shared_ptr<StreamBufferContainer> EKP2P::toSenderSBC()
{
	return _senderDaemon._toSenderSBC;
}


std::shared_ptr<StreamBufferContainer> EKP2P::toRoutingTableManagerSBC()
{
	return _routingTableManagerDaemon._toManagerSBC;
}


int EKP2P::assignBrokerDestination( std::shared_ptr<StreamBufferContainer> forwardingSBC , unsigned short destination )
{
	return _brokerDaemon._broker->forwardingDestination( forwardingSBC , destination );
}






void EKP2P::sendDimmyEKP2PMSG( const char* destIP, unsigned short destPort , std::shared_ptr<unsigned char> content , size_t contentLength , struct sockaddr_in sourceAddr ,unsigned short rpcType , unsigned short protocol )
{
	int sock = socket( AF_INET , SOCK_DGRAM , IPPROTO_UDP );

	struct sockaddr_in addr; memset( &addr , 0x00 , sizeof(addr) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr( destIP );
	addr.sin_port = htons( destPort );

	std::shared_ptr<KNodeAddr> sourceKNodeAddr = std::make_shared<KNodeAddr>( &sourceAddr ); // sourceKNodeAddrの作成

	EKP2PMessage msg;
	msg.payload( content , contentLength );
	msg.header()->protocol( protocol );
	msg.header()->rpcType( rpcType );
	msg.header()->sourceKNodeAddr( sourceKNodeAddr );

	std::shared_ptr<unsigned char> exportedMSG; size_t exportedMSGLength;
	exportedMSGLength = msg.exportRaw( &exportedMSG );

	msg.printRaw();

	size_t sendLen; 
	sendLen = sendto( sock , exportedMSG.get() , exportedMSGLength , 0 ,(struct sockaddr *)&addr, sizeof(addr) );
	std::cout << "sended dummy msg :: " << sendLen << " [bytes]" << "\n";
}




std::shared_ptr<EKP2PMessage> EKP2P::receiveSingleEKP2PMSG( unsigned short listenPort )
{
	std::shared_ptr<EKP2PMessage> retMSG;
	
	std::shared_ptr<SocketManager> sockManager = std::make_shared<SocketManager>();
	sockManager->setupUDPSock( listenPort );

	struct sockaddr_in fromAddr;
	std::shared_ptr<unsigned char> rawMSG; size_t rawMSGLength;
	rawMSGLength = sockManager->receive(  &rawMSG , fromAddr );

	std::cout << "..................." << "\n";
	std::cout << "message received from :: " << "\n";
	std::cout << "ip :: " << inet_ntoa(fromAddr.sin_addr) << "\n";
	std::cout << "port :: " << ntohs(fromAddr.sin_port) << "\n";
	std::cout << "..................." << "\n";

	auto parseRawMSG = ([]( std::shared_ptr<unsigned char> fromRawMSG , size_t fromRawMSGLength ) -> std::shared_ptr<EKP2PMessage>
	{
		std::shared_ptr<EKP2PMessage> ret = std::make_shared<EKP2PMessage>();
		ret->header()->importRawSequentially( fromRawMSG );

		if( memcmp( ret->header()->token() , "MIYA" , 4 ) != 0 ) return nullptr; // トークン不一致

		// この定義は違和感があるが..うごく
		std::shared_ptr<unsigned char> payloadHead = std::make_shared<unsigned char>(*( fromRawMSG.get() + ret->header()->headerLength() ));

		ret->payload( payloadHead );

		return ret;
	});

	
	retMSG = parseRawMSG( rawMSG , rawMSGLength );

	return retMSG;
}






};
