#include "broker.h"

#include "../../../share/stream_buffer/stream_buffer.h"
#include "../../../share/stream_buffer/stream_buffer_container.h"

#include "../../message/message.h"

#include "../../../ekp2p/ekp2p.h"
#include "../../../ekp2p/daemon/sender/sender.h"


namespace miya_chain
{


void MiyaChainBrocker::allowAll()
{
	for( int i=0; i<allowedForwardingFilter.size(); i++ )
		allowedForwardingFilter[i] = true;
}
void MiyaChainBrocker::blockAll()
{
	for( int i=0; i<allowedForwardingFilter.size(); i++ )
		allowedForwardingFilter[i] = false;
}





MiyaChainBrocker::MiyaChainBrocker( std::shared_ptr<StreamBufferContainer> incomingSBC , std::shared_ptr<StreamBufferContainer> toEKP2PBrokerSBC )
{
	_incomingSBC = incomingSBC;
	_toEKP2PBrokerSBC = toEKP2PBrokerSBC;

	for( int i=0; i<allowedForwardingFilter.size(); i++ ) //一旦全て許可する
		allowedForwardingFilter[i] = true;
}


int MiyaChainBrocker::start()
{
	if( _incomingSBC == nullptr ) return -1;
	if( _toEKP2PBrokerSBC == nullptr ) return -1;

	std::thread miyaChainBroker([&]()
	{
		std::cout << "MiyaChain::Daemon::Broker Thread Started" << "\n";
		_activeSenderThreadIDVector.push_back( std::this_thread::get_id() );

		std::unique_ptr<SBSegment> popedSB;
		for(;;)
		{
			popedSB = _incomingSBC->popOne();

			std::cout << "[ MiyaChain ] を経由しました" << "\n";
			std::cout << popedSB->bodyLength() << "\n";
			popedSB->forwardingSBCID( ekp2p::DEFAULT_DAEMON_FORWARDING_SBC_ID_SENDER );

			_toEKP2PBrokerSBC->pushOne( std::move(popedSB) );
		}
	});
	
	miyaChainBroker.detach();
	std::cout << "MiyaChainBrocker thread detached" << "\n";

	return 0;

};










void MiyaChainBrocker::setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target , unsigned short destination )
{
	printf("StreamBuffer seted with -> %p\n", target.get() );
	_sbHub.at(destination) = target;
}






std::shared_ptr<MiyaChainMessage> MiyaChainBrocker::parseRawMiyaChainMessage( std::shared_ptr<SBSegment> fromSB )
{
	/*
	std::shared_ptr<unsigned char> rawMSG; size_t rawMSGLength;
	rawMSG = fromSB->body(); 
	rawMSGLength = fromSB->bodyLength();

	std::shared_ptr<MiyaChainMessage> ret;
	ret->header()->importRawSequentially( rawMSG );

	std::shared_ptr<unsigned char> payloadHead( rawMSG.get() + sizeof( struct MiyaChainMessageHeader) );
	ret->payload( payloadHead );

	return ret;
	*/
}



std::shared_ptr<StreamBufferContainer> MiyaChainBrocker::incomingSBC()
{
	return _incomingSBC;
}



int MiyaChainBrocker::forwardingDestination( std::shared_ptr<StreamBufferContainer> sbc , unsigned short destination )
{
	if( destination >= MAX_PROTOCOL ) return -1;
	if( sbc == nullptr ) return -1;

	std::cout << "\x1b[31m" <<"(MiyaChainBroker) Set forwarding destination with :: " << destination << "\x1b[39m" << "\n";
	_sbHub.at(destination) = sbc;

	return destination;
}




}
