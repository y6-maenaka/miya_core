#include "ekp2p.h"


namespace ekp2p{






EKP2P::EKP2P()
{
	_kRoutingTable = new KRoutingTable;
	_inbandManager = new InbandManager;
}




void EKP2P::init()
{

	_kRoutingTable.init(); // wrapperも起動される
		
	return;
}






bool EKP2P::startMonitor( unsigned short port )
{
	
	_inbandManager->handlerArg((void*)_kRoutingTable->wrapper()->monitorBuffer() ); // ハンドラ引数の指定
	_inbandManager->start( 0 , 8080 );
}

};
