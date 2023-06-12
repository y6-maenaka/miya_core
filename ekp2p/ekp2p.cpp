#include "ekp2p.h"

#include "./kademlia/k_routing_table.h"
#include "./network/nat/client_nat_manager.h"
#include "./network/inband/inband_manager.h"
#include "./network/socket_manager/socket_manager.h"


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

	if( !(_kRoutingTable->init( &globalAddr )) ){
		std::cout << "正常なノードIDを取得できません TableWrapperの起動に失敗しました" << "\n";
		exit(1);
	}


	/*
	  FIND_NODE する
	*/
	std::cout << "ルーティングテーブルの初期化中... 初期ノードを集めています..." << "\n";

		
	return;
}




void EKP2P::start( unsigned short port , int type )
{
	_inbandManager->start( port , type );
}







};
