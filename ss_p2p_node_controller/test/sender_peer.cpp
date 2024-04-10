#include <ss_p2p/node_controller.hpp>
#include <ss_p2p/kademlia/k_node.hpp>

#include <span>
#include <string>
#include <vector>
#include <thread>
#include <chrono>


int sender_peer( int argc, const char* argv[] )
{
  std::span args( argv, argc );


  std::shared_ptr<io_context> io_ctx = std::make_shared<io_context>();
  ip::udp::endpoint self_endpoint( ip::udp::v4(), std::stoi(args[1]) );
  std::cout << "bingind :: " << self_endpoint << ":" << 9000 << "\n"; 
  ip::udp::endpoint dest_endpoint( ip::address::from_string( args[2]) , std::stoi(args[3]) );
  ss::kademlia::k_node dest_k_node( dest_endpoint );
  std::cout << "setup peer :: " << args[1] << ":" << std::stoi(args[2]) << "\n"; 


  ss::node_controller n_controller( self_endpoint, io_ctx );
  auto& routing_table = n_controller.get_routing_table();
  routing_table.auto_update( dest_k_node );

  n_controller.start();
  ss::peer dest_peer = n_controller.get_peer( dest_endpoint );

  std::this_thread::sleep_for( std::chrono::seconds(1) );
  for(;;)
  {
	std::string console_input;
	std::cout << "(message) : ";
	std::getline( std::cin, console_input );

	if( console_input.empty() ) continue;
  
	dest_peer.send( console_input );
  }

  return 0;
}
