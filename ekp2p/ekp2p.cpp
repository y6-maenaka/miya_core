#include "ekp2p.h"

#include "./kademlia/k_routing_table.h"
#include "./network/nat/client_nat_manager.h"
#include "./network/inband/inband_manager.h"
#include "./network/socket_manager/socket_manager.h"

#include "./kademlia/kademlia_RPC/FIND_NODE.cpp"
#include "./network/outband/node_broadcaster.h"


namespace ekp2p{




EKP2P::EKP2P( KRoutingTable *baseKRoutingTable )
{
	_inbandManager = new InbandNetworkManager;// inbandManagerのオブジェクト化

	
	if( baseKRoutingTable != nullptr ) _kRoutingTable = baseKRoutingTable;


	// Get Global Addr	
}





void EKP2P::init()
{

	/* NAT超えする */
	struct sockaddr_in globalAddr; 

	/* GlobalAddr を取得する */

	SocketManager *socketManager = new SocketManager; // NAT超えに使うsocketManager => 以降も使う
	ClientNatManager clientNatManager( socketManager );

	if( !(clientNatManager.natTraversal( &globalAddr )) ){
		std::cout << "グローバルアドレスを取得できません" << "\n";
		exit(1);
	}

	if( !(_kRoutingTable->init( &globalAddr )) )
	{
		std::cout << "正常なノードIDを取得できません TableWrapperの起動に失敗しました" << "\n";
		exit(1);
	}

	/*
	 ここまでプロセスが進んだら,永続するinbandManagerを起動する
	*/
	_inbandManager->start( socketManager );

	if( !collectStartUpNodes(socketManager) )
	{
		std::cout << "セットアップノード正常に収集できませんでした"	<< "\n";
		exit(1);
	}
	
	
	/*
	  FIND_NODE する
	*/

	/* InbandNetworkManager を起動する */
	std::cout << "ルーティングテーブルの初期化中... 初期ノードを集めています..." << "\n";

		
	return;
}


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




void EKP2P::start( unsigned short port , int type )
{
	//_inbandManager->start( port , type );
}







};
