#include <utils.hpp>
#include <ss_p2p/kademlia/k_bucket.hpp>
#include <ss_p2p/kademlia/k_routing_table.hpp>

#include "boost/asio.hpp"


using namespace boost::asio;


int setup_k_bucket_iterator()
{

  ip::udp::endpoint self_endpoint( ip::udp::v4(), 8000 );
  ss::kademlia::k_node self_k_node( self_endpoint );


  ss::kademlia::k_routing_table routing_table( self_k_node.get_id() );
  ss::kademlia::direct_routing_table_controller d_routing_table_controller( routing_table );

  for( int i=0; i<5; i++ ){
	ip::udp::endpoint dummy_ep = ss::generate_random_endpoint();
	ss::kademlia::k_node dummy_k_node( dummy_ep );
	routing_table.auto_update( dummy_k_node );
  }

  
  routing_table.print();

  for( auto bucket_itr = d_routing_table_controller.get_begin_bucket_iterator(); !(bucket_itr.is_invalid()); bucket_itr++ )
  {
	bucket_itr._print_();
  }

  


  // bucket_itr._print_();

  return 0;
}
