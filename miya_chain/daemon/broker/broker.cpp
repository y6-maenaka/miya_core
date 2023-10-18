#include "broker.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../../message/message.h"


namespace miya_chain
{



void MiyaChainMessageBrocker::start()
{

	if( _sourceSBC == nullptr ) return;

	std::thread miyaChainBroker([&]()
	{

		std::shared_ptr<SBSegment> popedSB;
		for(;;)
		{
			popedSB = _sourceSBC->popOne();
		}
	});

};










void MiyaChainMessageBrocker::setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target , unsigned short destination )
{
	printf("StreamBuffer seted with -> %p\n", target.get() );
	_sbHub.at(destination) = target;
}






std::shared_ptr<MiyaChainMessage> MiyaChainMessageBrocker::parseRawMiyaChainMessage( std::shared_ptr<SBSegment> fromSB )
{
	std::shared_ptr<unsigned char> rawMSG; size_t rawMSGLength;
	rawMSG = fromSB->body(); 
	rawMSGLength = fromSB->bodyLength();

	std::shared_ptr<MiyaChainMessage> ret;
	ret->header()->importRawSequentially( rawMSG );

	std::shared_ptr<unsigned char> payloadHead( rawMSG.get() + sizeof( struct MiyaChainMessageHeader) );
	ret->payload( payloadHead );


	return ret;
}


}
