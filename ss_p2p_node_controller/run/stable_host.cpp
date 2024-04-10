#include <utils.hpp>
#include <ss_p2p/node_controller.hpp>
#include <ss_p2p/kademlia/k_node.hpp>

#include "boost/asio.hpp"

#include <mutex>
#include <condition_variable>
#include <memory>
#include <span>
#include <vector>


using namespace boost::asio;


int main( int argc, const char* argv[] )
{
  std::span args( argv, argc );
  std::shared_ptr<io_context> io_ctx = std::make_shared<io_context>();

  ip::udp::endpoint self_ep( ip::udp::v4(), std::atoi(args[1]) );
  ss::node_controller n_controller( self_ep, io_ctx );
  auto &direct_routing_table_controller = n_controller.get_direct_routing_table_controller();

  std::vector< ip::udp::endpoint > dummy_eps;
  for( int i=0; i<5; i++ )
  {
	ip::udp::endpoint dummy_ep = ss::generate_random_endpoint();
	dummy_eps.push_back( dummy_ep );
  }
  direct_routing_table_controller.auto_update_batch( dummy_eps );

  n_controller.start();





  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait( lock );
}

