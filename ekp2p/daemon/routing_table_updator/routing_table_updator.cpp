#include "routing_table_updator.h"



#include "../../kademlia/k_node.h"
#include "../../kademlia/k_routing_table/k_routing_table.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"



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

		std::unique_ptr<SBSegment> popedSB ;
		for(;;)
		{
			popedSB = _incomingSB->popOne();
		}
	});

	ekp2pKRoutingTableUpdator.detach();
	std::cout << "ekp2pKRoutingTableUpdator thread detached" << "\n";


	return 0;

}











};
