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

	for( int i=0; i< MAX_PROTOCOL; i++ )  // 一旦全ての転送は許可することとする
		_allowedProtocolSet[i] = true;
}







int EKP2PBroker::start( bool requiresRouting )
{
	if( _incomingSB == nullptr ) return -1;
	if( _toRoutingTableManagerSBC == nullptr ) return -1;


	// 行き or 帰り はどう判断する？
	std::thread ekp2pBroker([&]()
	{
		std::cout << "\x1b[32m" << "EKP2P::daemon::Broker ekp2pBroker thread started" << "\x1b[39m" << "\n";
		_activeSenderThreadIDVector.push_back( std::this_thread::get_id() ); 


		std::unique_ptr<SBSegment> sb;
		for(;;)
		{
			sb = _incomingSB->popOne();
			if( sb->flag() == SB_FLAG_MODULE_EXIT ){
				std::cout << "exit flag received" << "\n";
				return;
			}

			unsigned short forwardingProtocol = sb->forwardingSBCID();
			if( forwardingProtocol >= MAX_PROTOCOL ) continue; // 受け付けていないプロトコルメッセージ

			// KRoutingTableだけは独立して転送する
			std::cout << "ekp2pIsProcessed :: " << sb->ekp2pIsProcessed() << "\n"; 
			std::cout << "requiresRouting :: " << requiresRouting << "\n";
			if( !(sb->ekp2pIsProcessed()) && requiresRouting ) 
			{
				_toRoutingTableManagerSBC->pushOne( std::move(sb) );
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

	std::cout << "\x1b[31m" <<"Set forwarding destination with :: " << destination << "\x1b[39m" << "\n";
	_sbHub.at(destination) = sbc;

	return destination;
}


};
