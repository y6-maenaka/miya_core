#include "routing_table_updator.h"



#include "../../kademlia/k_node.h"
#include "../../kademlia/k_routing_table/k_routing_table.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../../protocol.h"


namespace ekp2p
{



EKP2PKRoutingTableUpdator::EKP2PKRoutingTableUpdator( std::shared_ptr<KRoutingTable> routingTable , std::shared_ptr<StreamBufferContainer> incomingSB , std::shared_ptr<StreamBufferContainer> toBrokerSBC )
{
	_incomingSB = incomingSB;
	_routingTable = routingTable;
	_toBrokerSBC = toBrokerSBC;

	std::cout << "EKP2P::daemon::KRoutingTableUpdator just initialized" << "\n";
}






int EKP2PKRoutingTableUpdator::start()
{
	if( _routingTable == nullptr ) return -1;
	if( _incomingSB == nullptr ) return -1;
	if( _toBrokerSBC == nullptr ) return -1;

	std::thread ekp2pKRoutingTableUpdator([&]()
	{
		std::cout <<  "\x1b[32m" << "EKP2P::daemon::KRoutingTableUpdator ekp2pKRoutingTableUpdator thread started" << "\x1b[39m" << "\n";
		_activeSenderThreadIDVector.push_back( std::this_thread::get_id() ); 
		
		int discardFlag = false;
		std::unique_ptr<SBSegment> popedSB ;
		for(;;)
		{
			popedSB = _incomingSB->popOne();
			
			// handle ekp2p msg		
			switch( popedSB->rpcType() )
			{
				case static_cast<int>(KADEMLIA_RPC_IGNORE): // IGNORE
				{
					std::cout << "case 0" << "\n";
					break;
				} 
				case static_cast<int>(KADEMLIA_RPC_PING): // PING
				{
					std::cout << "case 1" << "\n";
					break;
				}	
				case static_cast<int>(KADEMLIA_RPC_PONG): // PONG
				{
					std::cout << "case 2" << "\n";
					break;
				}
				case static_cast<int>(KADEMLIA_RPC_FIND_NODE): // FIND_NODE
				{
					std::cout << "case 3" << "\n";
					break;
				}
			}
			
			popedSB->ekp2pIsProcessed( true ); 

			if( !discardFlag ) // if not need discard
				_toBrokerSBC->pushOne( std::move(popedSB) );
		}
	});

	ekp2pKRoutingTableUpdator.detach();
	std::cout << "ekp2pKRoutingTableUpdator thread detached" << "\n";


	return 0;

}











};
