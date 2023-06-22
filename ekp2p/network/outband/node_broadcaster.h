#ifndef F24343E2_8E98_4FA3_BE11_62186450FF80
#define F24343E2_8E98_4FA3_BE11_62186450FF80


#include <vector>




namespace ekp2p
{


class Node;
class EKP2PMSG;






class NodeBroadcaster
{
private:
	std::vector< Node* > *_nodeVector = nullptr;

public:
	NodeBroadcaster(){};
	NodeBroadcaster( std::vector< Node* > *target ){ _nodeVector = target; };

	std::vector< Node* > *nodeVector(){ return _nodeVector; }; // getter
	void nodeVector( std::vector< Node*> *nodeVector ){ _nodeVector = nodeVector; }; // setter

	void broadcast( EKP2PMSG *msg ); // 送信するMSGが完全である必要がある
	void broadcast( unsigned char* payload , unsigned int pyaloadSize );

	void broadcastKademliaRPC( int RPCType );
};






}; // close ekp2p namespace


#endif // F24343E2_8E98_4FA3_BE11_62186450FF80

