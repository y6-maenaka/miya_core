#include "ekp2p.h"

#include "./network/inband/inband_manager.h"
#include "./kademlia/k_routing_table.h"
#include "./network/nat/client_nat_manager.h"


namespace ekp2p{






EKP2P::EKP2P( KRoutingTable *baseKRoutingTable )
{
	_inbandManager = new InbandNetworkManager;

	_kRoutingTable = baseKRoutingTable;
	if( _kRoutingTable == nullptr  ) return;

	// Get Global Addr
	

}




void EKP2P::init()
{

	struct sockaddr_in globalAddr;

	/* GlobalAddr を取得する */
	ClientNatManager clientNatManager;
	clientNatManager.natTraversal( &globalAddr );

	_kRoutingTable->init( &globalAddr ); // wrapperも起動される
		
	return;
}



void EKP2P::start( unsigned short port , int type )
{
	_inbandManager->start( port , type );
}








};
