#include "client_nat_manager.h"

#include "../inband/inband_manager.h"


namespace ekp2p{



bool ClientNatManager::natTraversal( struct sockaddr_in *globalAddr )
{

	UDPInbandManager *inbandManager = new UDPInbandManager;
	
	std::cout << "START NAT TRAVERSAL" << "\n";
	

	globalAddr = new sockaddr_in;
	memset( globalAddr, 0x00, sizeof(struct sockaddr_in) );

	for( int i=0; i<5; i++) // 検索回数は5回
	{
		std::cout << "inquiring ..." << "\n";
		inbandManager->standAlone( (void*)globalAddr , true ); // バッファが空でも待機しない
		if( validateSockaddrIn( globalAddr ) ) return true;
		sleep(2 * i);
		// globalAddrに正常な値が格納されていたら
	}

	return false;

}





bool ClientNatManager::validateSockaddrIn( sockaddr_in *targetAddr )
{
	if( ntohs(targetAddr->sin_port) == 0 ) return false;
	if( targetAddr->sin_addr.s_addr == 0 ) return false;

	return true;
}




}; // close ekp2p namespace
