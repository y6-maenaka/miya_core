#include "node_broadcaster.h"


#include "../../kademlia/node.h"

namespace ekp2p
{



void NodeBroadcaster::broadcast( EKP2PMSG *msg )
{
	for( auto itr : *_nodeVector )	
		itr->send( msg );

}



void NodeBroadcaster::broadcast( unsigned char* payload , unsigned int payloadSize )
{
	// msgに変換して送信する
	return;
}



void NodeBroadcaster::broadcastKademliaRPC( int RPCType )
{
	return;
}



}; // close ekp2p namespace
