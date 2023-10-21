#include "broker.h"



#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"



namespace ekp2p
{



EKP2PBroker::EKP2PBroker( std::shared_ptr<StreamBufferContainer> incomingSB )
{
	_incomingSB =	incomingSB;
}



int EKP2PBroker::start()
{
	if( _incomingSB == nullptr ) return -1;

	// 行き or 帰り はどう判断する？
	std::thread ekp2pBroker([&]()
	{
		std::cout << "\x1b[32m" << "EKP2P::daemon::Broker ekp2pBroker thread started" << "\x1b[39m" << "\n";
		_activeSenderThreadIDVector.push_back( std::this_thread::get_id() ); 


		std::unique_ptr<SBSegment> sb;
		for(;;)
		{
			sb = _incomingSB->popOne();
			unsigned short protocol = sb->protocol();
			if( protocol >= MAX_PROTOCOL ) return nullptr; // 受け付けていないプロトコル

			if( _sbHub.at(protocol) == nullptr ) std::cout << "_sbHub.at(ptorocol) is nullptr" << "\n";
			if( _allowedProtocolSet[protocol] == false ) std::cout << "protocol :: " <<  protocol << " :: is not allowed" << "\n";


			if( _sbHub.at(protocol) == nullptr || !(_allowedProtocolSet[protocol]) ){
				std::cout << "Not Allowed Pack Received" << "\n";
				continue;
			}

			// KRoutingTableだけは独立して転送する



			_sbHub.at(protocol)->pushOne( std::move(sb) ); // 受信セグメントの転送
		}
	});

	ekp2pBroker.detach();
	std::cout << "ekp2pBroker thread detached" << "\n";

	return 0;
}




int EKP2PBroker::forwardingDestination( std::shared_ptr<StreamBufferContainer> sbc , unsigned short destination )
{
	if( destination >= MAX_PROTOCOL ) return -1;
	if( sbc == nullptr ) return -1;

	std::cout << "Set forwarding destination with :: " << destination << "\n";
	_sbHub.at(destination) = sbc;

	return destination;
}


};
