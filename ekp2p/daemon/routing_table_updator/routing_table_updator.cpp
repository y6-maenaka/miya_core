#include "routing_table_updator.h"



#include "../../kademlia/k_node.h"
#include "../../kademlia/k_routing_table/k_routing_table.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"



namespace ekp2p
{



KRoutingTableUpdator::KRoutingTableUpdator( std::shared_ptr<KRoutingTable> routingTable , std::shared_ptr<StreamBufferContainer> incomingSB )
{
	_incomingSB = incomingSB;
	_routingTable = routingTable;

	std::cout << "EKP2P::daemon::KRoutingTableUpdator just initialized" << "\n";
}






void KRoutingTableUpdator::start()
{
	std::cout << "EKP2P::daemon::KRoutingTableUpdator ekp2pKRoutingTableUpdator thread started" << "\n";
}











};
