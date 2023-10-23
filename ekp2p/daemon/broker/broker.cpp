#include "broker.h"

#include "../../ekp2p.h"


#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"



namespace ekp2p
{



EKP2PBroker::EKP2PBroker( std::shared_ptr<StreamBufferContainer> incomingSB , std::shared_ptr<StreamBufferContainer> toRoutingTableUpdatorSBC )
{
	_incomingSB =	incomingSB;
	_toRoutingTableManagerSBC = toRoutingTableUpdatorSBC;
}



int EKP2PBroker::start( bool requiresRouting )
{
	if( _incomingSB == nullptr ) return -1;
	if( _toRoutingTableManagerSBC == nullptr ) return -1;
	printf(">>>>>> %p\n", _toRoutingTableManagerSBC.get() );
	printf("<<<<< isRouting :: %p - %d\n", &requiresRouting , requiresRouting );


	// 行き or 帰り はどう判断する？
	std::thread ekp2pBroker([&]()
	{
		std::cout << "\x1b[32m" << "EKP2P::daemon::Broker ekp2pBroker thread started" << "\x1b[39m" << "\n";
		_activeSenderThreadIDVector.push_back( std::this_thread::get_id() ); 


		std::unique_ptr<SBSegment> sb;
		for(;;)
		{
			sb = _incomingSB->popOne();
			unsigned short forwardingProtocol = sb->forwardingSBCID();
			if( forwardingProtocol >= MAX_PROTOCOL ) return nullptr; // 受け付けていないプロトコル

			std::cout << "forwarding protocol :: " << forwardingProtocol << "\n";

			// KRoutingTableだけは独立して転送する
			std::cout << sb->ekp2pIsProcessed() << "\n"; 
			requiresRouting = true; // ???
			if( !(sb->ekp2pIsProcessed()) && requiresRouting ) 
			{
				_toRoutingTableManagerSBC->pushOne( std::move(sb) );
				std::cout << "forwarding to RoutingTableManagerSBB ===== >> " << "\n";
				continue; // skip while done ekp2p processing
			}

			if( _sbHub.at(forwardingProtocol) == nullptr || !(_allowedProtocolSet[forwardingProtocol]) ){
				std::cout << "Not Allowed Pack Received" << "\n";
				continue;
			}

			_sbHub.at(forwardingProtocol)->pushOne( std::move(sb) ); // 受信セグメントの転送
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
