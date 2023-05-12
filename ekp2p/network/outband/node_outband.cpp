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
	MSGHeader header;

	msg.header( &header );
	msg.payload( payload , payloadSize );
	msg.kTag( kTag  );

	unsigned char* exportedRawMSG;
	unsigned int exportedRawMSGSize = 0;

	exportedRawMSGSize = msg.exportRawMSG( &exportedRawMSG );

	sendSize = socketManager->send( exportedRawMSG, exportedRawMSGSize  );

	free( exportedRawMSG );

	return sendSize;
}






}; // close ekp2p namespae
