#ifndef A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C
#define A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C


#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>


namespace ekp2p{

class InbandNetworkManager;
class KRoutingTable;



class EKP2P
{

private:

	InbandNetworkManager *_inbandManager; // tcp,udp 兼用
	
	KRoutingTable *_kRoutingTable = nullptr;

public:

	EKP2P( KRoutingTable *baseKRoutingTable = nullptr );
	
	void init();
	void start( unsigned short port , int type );

	//bool startMonitor( unsigned short port );

};



};

#endif // A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C

