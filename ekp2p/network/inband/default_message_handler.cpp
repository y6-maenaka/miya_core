#include "inband_manager.h"

#include "../message/message.h"

#include "../../../shared_components/middle_buffer/middle_buffer.h"



namespace ekp2p{



void DefaultMessageHandler( void* arg , EKP2PMSG *msg ){


	MiddleBuffer *middleBuffer = (MiddleBuffer *)arg;

	// std::cout << middleBuffer->num << "\n";


	unsigned int rawKTagSize = msg->kTag()->exportRawSize();
	unsigned char *rawKTag = new unsigned char[rawKTagSize];
	msg->kTag()->exportRaw( &rawKTag );

	if( middleBuffer != nullptr  )
		middleBuffer->push( (void*)rawKTag , rawKTagSize , false );	// update k_routing_table with k_tag 
	
}


}
