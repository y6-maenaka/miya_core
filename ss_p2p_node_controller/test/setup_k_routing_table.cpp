#include <ss_p2p/kademlia/k_routing_table.hpp>
#include <ss_p2p/kademlia/dht_manager.hpp>
#include <ss_p2p/kademlia/node_id.hpp>
#include <ss_p2p/kademlia/k_bucket.hpp>
#include <ss_p2p/kademlia/k_node.hpp>
#include <ss_p2p/node_controller.hpp>
#include <utils.hpp>
#include <json.hpp>

#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "boost/asio.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/lexical_cast.hpp"


using namespace boost::asio;
using namespace boost::uuids;
using json = nlohmann::json;


class ping_host
{
public:
  ss::kademlia::k_routing_table &_routing_table;
  ping_host( ss::kademlia::k_routing_table &routing_table ) : 
	_routing_table(routing_table)
  {
	return;
  }

  void on_timeout( k_node swap_from, k_node swap_to ){
	std::cout << "on timeout" << "\n";
	std::cout << swap_from.get_endpoint() << " :: " << swap_to.get_endpoint() << "\n";
	_routing_table.auto_update( swap_from );
	_routing_table.auto_update( swap_to );
	
	_routing_table.print();
  }

  void on_pong()
  {
	std::cout << "on pong" << "\n";
  }
};


int setup_k_routing_table()
{
  std::shared_ptr<io_context> io_ctx = std::make_shared<io_context>();
  ip::udp::endpoint self_endpoint( ip::udp::v4(), 9000 );
  ss::kademlia::k_node self_k_node( self_endpoint );
 

  
  ss::node_controller n_controller( self_endpoint, io_ctx );
  n_controller.start();

  auto& dht_manager = n_controller.get_dht_manager();
  auto& routing_table = n_controller.get_routing_table();
  auto& rpc_manager = dht_manager.get_rpc_manager();
  auto& ice_agent = n_controller.get_ice_agent();
  auto signaling_send_func = ice_agent.get_signaling_send_func();

  


  for( int i=0; i<10; i++ ) // ダミーのk_nodeを追加
  {
	ip::udp::endpoint _dummy_ep = ss::generate_random_endpoint();
	ss::kademlia::k_node _dummy_k_node(_dummy_ep);
  }

  ip::udp::endpoint ping_clinet_ep( ip::address::from_string("127.0.0.1"), 8100 );
  k_node ping_clinet_k_node(ping_clinet_ep);
  routing_table.auto_update( ping_clinet_k_node );

  
  ip::udp::endpoint swap_from_ep( ip::address::from_string("10.0.0.1"), 100 ); 
  k_node swap_from_k_node( swap_from_ep );

  ip::udp::endpoint swap_to_ep( ip::address::from_string("192.168.0.1"), 200 );
  k_node swap_to_k_node( swap_to_ep );

 

  ping_host ph(routing_table);
  rpc_manager.ping_request( ping_clinet_ep
	  , std::bind( &ping_host::on_pong, ph  )
	  , std::bind( &ping_host::on_timeout, ph, swap_from_k_node, swap_to_k_node ) );  


  auto &k_observer_strage = dht_manager.get_observer_strage();
  k_observer_strage.show_state( boost::system::error_code{} );
  std::cout << "done" << "\n";



  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait( lock );


  return 0;
}
