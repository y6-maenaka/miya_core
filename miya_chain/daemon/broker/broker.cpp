#include "broker.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../../message/message.h"


namespace miya_chain
{




MiyaChainMessageBrocker::MiyaChainMessageBrocker( std::shared_ptr<StreamBufferContainer> incomingSBC )
{
	_incomingSBC = incomingSBC;
}


int MiyaChainMessageBrocker::start()
{
	if( _incomingSBC == nullptr ) return -1;

	std::thread miyaChainBroker([&]()
	{
		std::cout << "MiyaChain::Daemon::Broker Thread Started" << "\n";
		_activeSenderThreadIDVector.push_back( std::this_thread::get_id() );

		std::shared_ptr<SBSegment> popedSB;
		for(;;)
		{
			popedSB = _incomingSBC->popOne();

			std::cout << "SB Poped"	<< "\n";
		}
	});
	
	miyaChainBroker.detach();
	std::cout << "MiyaChainMessageBrocker thread detached" << "\n";

	return 0;

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



std::shared_ptr<StreamBufferContainer> MiyaChainMessageBrocker::incomingSBC()
{
	return _incomingSBC;
}


}
