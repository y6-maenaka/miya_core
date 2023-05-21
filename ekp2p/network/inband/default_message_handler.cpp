#include "inband_manager.h"

#include "../message/message.h"

#include "../../../shared_components/middle_buffer/middle_buffer.h"



namespace ekp2p{





struct KTagPack
{
	SocketManager* _relatedSocketManager;
	unsigned char* _rawKTag;


	KTagPack( unsigned int kTagSize ){
		_rawKTag = new unsigned char[kTagSize];
	};

	~KTagPack(){
		delete rawKTag;
	};
};





void DefaultMessageHandler( void* arg , EKP2PMSG *msg ){


	InbandManager *inbandManager = (InbandManager *)arg;
	MiddleBuffer *middleBuffer = (MiddleBuffer *)(inbandManager->_handlerArg); // handlerArgにデータが格納されている


	unsigned int rawKTagSize = msg->kTag()->exportRawSize();
	unsigned char *rawKTag = new unsigned char[rawKTagSize];
	msg->kTag()->exportRaw( &rawKTag );


	KTagPack *kTagPack = new KTagPack( rawKTagSize );
	kTagPack->_relatedSocketManager = inbandManager->relatedSocketManager;
	memcpy( kTagPack->_rawKTag , rawKTag , rawKTagSize ); delete rawKTag;

	if( middleBuffer != nullptr  )
		middleBuffer->pushOne( (void*)kTagPack , rawKTagSize + sizeof(kTagPack->relatedSocketManager), false );	// update k_routing_table with k_tag 

	return;
}


};
