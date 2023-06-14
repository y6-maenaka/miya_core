#include "node_outband.h"

#include "../../kademlia/k_tag.h"
#include "../socket_manager/socket_manager.h"
#include "../message/header.h"
#include "../message/message.h"


namespace ekp2p{



int NodeOutband::send( unsigned char* payload, unsigned int payloadSize , KTag* kTag , SocketManager *socketManager  )
{

	if( socketManager == NULL ){
		std::cout << " socket Manager not seted" << "\n";
		return -1;
	}

	int sendSize =  -1;	

	EKP2PMSG msg;
	msg.payload( payload , payloadSize ); // payloadがnullptrでもうまくやってくれる
	msg.kTag( kTag ); // kTagがnullptrでもうまくやってくれる

	unsigned char *rawMSG; unsigned int rawMSGSize;
	rawMSGSize = msg.exportRaw( &rawMSG );

	sendSize = socketManager->send( rawMSG , rawMSGSize );

	return sendSize;
}




int NodeOutband::send( unsigned char* msg , unsigned int msgSize, SocketManager *socketManager )
{
	if( socketManager == nullptr ){
		std::cout << " socket Manager not seted" << "\n";
		return -1;
	}

	
	int sendSize = -1;
	sendSize = socketManager->send( msg , msgSize );

	return sendSize;
}






}; // close ekp2p namespae
