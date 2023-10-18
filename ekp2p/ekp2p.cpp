#include "ekp2p.h"

// #include "./kademlia/k_routing_table.h"
#include "./network/nat/client_nat_manager.h"
#include "./network/inband/inband_manager.h"
#include "./network/socket_manager/socket_manager.h"

#include "./network/header/header.h"

#include "./kademlia/kademlia_RPC/FIND_NODE.cpp"
#include "./network/outband/node_broadcaster.h"

#include "./kademlia/k_node.h"
#include "../shared_components/stream_buffer/stream_buffer.h"
#include "../shared_components/stream_buffer/stream_buffer_container.h"
// #include "./kademlia/message_receiver.h"
#include "./kademlia/connection_controller.h"
#include "./kademlia/k_routing_table/k_routing_table.h"
// #include "./kademlia/k_routing_table/k_routing_table_updator.h"


#include "./daemon/sender/sender.h"
#include "./daemon/receiver/receiver.h"
#include "./daemon/routing_table_updator/routing_table_updator.h"


namespace ekp2p{




EKP2P::EKP2P( std::shared_ptr<KRoutingTable> KRoutingTable )
{

	_hostSocketManager = std::shared_ptr<SocketManager>( new SocketManager{} );


	if( KRoutingTable == nullptr ){
		// _hostNode = std::make_shared<KHostNode>();
		_kRoutingTable = std::make_shared<class KRoutingTable>(_hostSocketManager);
		return;	
	}
	// _mainNode = std::make_shared<KHostNode>();
	return;
}



/*
EKP2P::EKP2P( KRoutingTable *baseKRoutingTable )
{
	_inbandManager = new InbandNetworkManager;// inbandManagerのオブジェクト化

	if( baseKRoutingTable != nullptr ) _kRoutingTable = baseKRoutingTable;

	// Get Global Addr	
}
*/





	/*
void EKP2P::init()
{
	std::cout << "[ # ] MiyaCoreネットワークに接続します" << "\n";

	std::cout << "[ ## ] マスターソケットマネージャーを作成します" << "\n";
	_mainSocketManager = new SocketManager();
	_mainSocketManager->create();
	_mainSocketManager->bind( 8080 );



	std::cout << "[ ### ] NAT状況を取得します" << "\n";
	std::cout << " -  ブートストラップノードに問い合わせています"  << "\n";

	struct sockaddr_in bootstrapNodeAddr; memset( &bootstrapNodeAddr , 0x00 , sizeof(bootstrapNodeAddr) );
	bootstrapNodeAddr.sin_addr.s_addr = inet_addr( (char *)"35.74.112.90" );
	bootstrapNodeAddr.sin_port = htons( 8080 );

	std::shared_ptr<KNodeAddr> nodeAddr = std::make_shared<KNodeAddr>( &bootstrapNodeAddr );
	KClientNode bootstrapNode( nodeAddr , _mainSocketManager );


	
	struct sockaddr_in responderNodeAddr; memset( &responderNodeAddr , 0x00 , sizeof(responderNodeAddr) );
	unsigned responderNodeAddrLength = sizeof(responderNodeAddr);
	EKP2PMessageHeader header; 

	auto dropMSG = [&]()
	{
		unsigned char dummy[1];
		recvfrom( _mainSocketManager->sock() , dummy , 1 , 0, nullptr , 0 );
	};


	int receivedLength = 0;
	std::shared_ptr<KNodeAddr> outsideGlobalAddr;
	for( int i=0; i<3; i++ )	 // selectで監視する
	{
		this->send( &bootstrapNode , nullptr , 0 , 4 );
		std::cout << " -  問い合わせ回数 :: " << i+1  << "\n";

		sleep( i * 2 );

		memset( &header , 0x00 , sizeof(header) );

		receivedLength = recvfrom( _mainSocketManager->sock() , &(header._meta) , sizeof(header._meta) , MSG_PEEK , (struct sockaddr*)&responderNodeAddr , &responderNodeAddrLength );

	// 受信元のパケットが異なっていたら破棄する
		if( responderNodeAddr.sin_addr.s_addr != bootstrapNodeAddr.sin_addr.s_addr || responderNodeAddr.sin_port != bootstrapNodeAddr.sin_port ){
			std::cout << "msg droped with sorce node" << "\n";
			dropMSG();
			continue;
		} 
		// 不正なメッセージだったら破棄する
		if( !(header.validate()) ){ 
			std::cout << "msg droped with validation error" << "\n";
			dropMSG();
			continue;
		}

		std::shared_ptr<unsigned char> receivedMSG = std::shared_ptr<unsigned char>( new unsigned char(header.headerLength() + header.headerLength()) );
		receivedLength = recvfrom( _mainSocketManager->sock() ,  receivedMSG.get() , header.payloadLength() + header.headerLength() ,0  , nullptr , 0 );
	

		unsigned char* payload; unsigned int payloadLength;
		payloadLength = MessageReceiver::payload( receivedMSG.get() , &payload );

	
		outsideGlobalAddr = ConnectionController::outsideGlobal( payload );
		if( outsideGlobalAddr != nullptr ) break;
	}


	std::cout << "[ #### ] ブートストラップノードへの問い合わせが完了しました" << "\n";

	_mainNode->kNodeAddr( outsideGlobalAddr );  // ノードIDもセットされる

	// NAT解決アドレス以外を破棄して検証するシーケンス
	std::cout << "[ ##### ] グローバルIPアドレスを取得しました"  << "\n";
	std::cout << "[ IPv4 ] :: " << inet_ntoa( outsideGlobalAddr->sockaddr_in()->sin_addr ) << "\n";
	std::cout << "[ Port ] :: " << ntohs(outsideGlobalAddr->sockaddr_in()->sin_port) << "\n";

	//ここでブートノード(NonNatNode)の情報を取得してくる

	// RoutingTableをセットアップする
	_kRoutingTable = new KRoutingTable( _mainNode );
	return;
}
*/



/*
int EKP2P::send( KClientNode *targetNode , void *payload , unsigned short payloadLength , unsigned short protocol )
{
	if( payload == nullptr ) payloadLength = 0;
	
	EKP2PMessageHeader header;


	header.protocol( protocol );
	header.payloadLength( payloadLength );
	header.sourceNodeAddr( nullptr );


	unsigned char* rawHeader; unsigned int rawHeaderLength = 0;
	rawHeaderLength = header.exportRaw( &rawHeader );

	
	unsigned char* rawMsg = new unsigned char[ (rawHeaderLength + payloadLength) ]; 
	unsigned int rawMsgLenght = (rawHeaderLength + payloadLength);

	// メッセージをフォーマットする 
	memcpy( rawMsg ,rawHeader , rawHeaderLength );
	if( payload != nullptr ) memcpy( rawMsg + rawHeaderLength , payload , payloadLength );


	sendto( _mainSocketManager->sock() , rawMsg ,  rawMsgLenght , 0 , (struct sockaddr *)(targetNode->kNodeAddr()->sockaddr_in().get()) , sizeof( struct sockaddr_in) );

	delete[] rawHeader; delete[] rawMsg;
	return 0;
}
*/










/*
bool EKP2P::collectStartUpNodes( SocketManager *baseSocketManager )
{
	// このルーチンに入る前に,tableWrapperは起動している事が前提

	Node* bootstrapNode; // 複数のブートストラップノードを登録しておく
	RequestRPC_FIND_NODE( bootstrapNode , _kRoutingTable->selfKAddr() ); // ブートストラップノードにFIND_NODEを送信

	NodeBroadcaster broadcaster;
	EKP2PMSG *findNodeMSG = GenerateRPCMSG_FIND_NODE( _kRoutingTable->selfKAddr() );
	
	std::vector< Node* > queriedNodeVector;
	std::vector< Node* > *arraivedNodeVector;
	// 再起的にノードの集計とルックアップを行う
	
	std::chrono::system_clock::time_point startTimeStamp = std::chrono::system_clock::now(); 
	
	while( queriedNodeVector.size() >= 10 || (std::chrono::system_clock::now() - startTimeStamp).count() >= DEFAULT_FIND_NODE_ROUTINE_TIMEOUT  )
	{
		sleep(1);
		arraivedNodeVector = _kRoutingTable->selectNodeBatch( -1 , &queriedNodeVector ); // 新規の全てのノードを回収
			
		broadcaster.nodeVector( arraivedNodeVector );
		broadcaster.broadcast( findNodeMSG );

		queriedNodeVector.insert( queriedNodeVector.cend() , arraivedNodeVector->cbegin() , arraivedNodeVector->cend() );
	}

	return true;
}
*/


/*
void EKP2P::start() // 一旦daemon系を起動してflagで制御する
{
	//1. Updaterの起動
	auto routingTableUpdator = std::make_shared<KRoutingTableUpdator>( _kRoutingTable );
	auto routingTableUpdatorConsumerSB = std::make_shared<StreamBufferContainer>( nullptr , nullptr );
	
	routingTableUpdator->setSourceStreamBuffer( routingTableUpdatorConsumerSB );
	std::thread routingTableUpdatorTH([routingTableUpdator](){
		routingTableUpdator->start();
	});
	routingTableUpdatorTH.detach();


	
	//2. ConnectionControllerの起動
	// 後回しでOK


	//3. BlockChainControllerの起動
	



	//4. MessageReceiverの起動
	auto messageReceiver = std::make_shared<MessageReceiver>(_mainSocketManager);
	auto messageReceiverProducerSB = std::make_shared<StreamBufferContainer>( nullptr, nullptr );

	messageReceiver->setDestinationStreamBuffer( messageReceiverProducerSB , 4 );
	std::thread messageReceiverTH([messageReceiver](){
		messageReceiver->start();
	});
	messageReceiverTH.detach();



	// ベーシックハンドラの起動



	// サードパーティーハンドラの起動
}
*/




int EKP2P::init()
{
	// 一旦全てのdaemonをセットアップする
	// センダーのセットアップ
	_senderDaemon._toSenderSB = std::make_shared<StreamBufferContainer>();
	_senderDaemon._sender	= std::make_shared<Sender>( _kRoutingTable , _senderDaemon._toSenderSB );
	//_senderDaemon._sender->start();

	// レシーバーのセットアップ
	_receiverDaemon._toReseiverSB = std::make_shared<StreamBufferContainer>();
	_receiverDaemon._receiver = std::make_shared<Receiver>( /*_receiverDaemon._toReseiverSB */ nullptr ); // receiverにはSBは不要 reseice制限フラグ
	//_receiverDaemon._receiver->start();

	// アップデータのセットアップ
	_updatorDaemon._toUpdatorSB = std::make_shared<StreamBufferContainer>();
	_updatorDaemon._updator = std::make_shared<KRoutingTableUpdator>( _kRoutingTable ,_updatorDaemon._toUpdatorSB );
	//_updatorDaemon._updator->start();

	return 0;
}




int EKP2P::start()
{

	_senderDaemon._sender->start();

	_receiverDaemon._receiver->start();

	// _updatorDaemon._updator->start();

	sleep(1);

	return 0;
}



std::shared_ptr<StreamBufferContainer> EKP2P::toReseiverSB()
{
	return _receiverDaemon._toReseiverSB;
}


std::shared_ptr<StreamBufferContainer> EKP2P::toSenderSB()
{
	return _senderDaemon._toSenderSB;
}


std::shared_ptr<StreamBufferContainer> EKP2P::toUpdatorSB()
{
	return _updatorDaemon._toUpdatorSB;
}








};
