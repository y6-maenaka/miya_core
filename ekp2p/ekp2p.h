#ifndef A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C
#define A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C



namespace ekp2p{

class InbandManager;
class KRoutingTable;



class EKP2P
{

private:
	InbandNetworkManager *_inbandManager;
	KRoutingTable *_kRoutingTable;

public:

	EKP2P();
	
	void init();

	bool startMonitor( unsigned short port );
};



};

#endif // A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C

