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

};
