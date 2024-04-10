#include <ss_p2p/kademlia/k_routing_table.hpp>
#include <ss_p2p/node_controller.hpp>
#include <string>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "openssl/evp.h"
#include "boost/asio.hpp"

#include "../test/setup_node_controller.cpp"
#include "../test/dummy_message.cpp"
#include "../test/setup_peer.cpp"
#include "../test/setup_k_routing_table.cpp"
#include "../test/setup_ice_observer.cpp"
#include "../test/setup_ice_agent.cpp"
#include "../test/setup_signaling_relay_host.cpp"
#include "../test/setup_signaling_response_host.cpp"


using boost::asio::ip::udp;


int main( int argc, const char* argv[] )
{
  std::cout << "Hello node_2" << "\n";

  setup_node_controller( argc, argv );


  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait( lock );
  return 0;
}
