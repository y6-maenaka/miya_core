#include <ss_p2p/kademlia/k_observer.hpp>
#include <ss_p2p/kademlia/k_routing_table.hpp>
#include <ss_p2p/kademlia/k_node.hpp>
#include <utils.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include "boost/asio.hpp"


int setup_ice_observer()
{


  ip::udp::endpoint test_endpoint( ip::udp::v4(), 8100 );
  std::string ep_str = ss::endpoint_to_str( test_endpoint ) ;
  ip::udp::endpoint ep_from_str = ss::str_to_endpoint( ep_str );
  
  std::cout << ep_str << "\n";
  std::cout << ep_from_str << "\n";


  return 0;
}
