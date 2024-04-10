#include <ss_p2p/ice_agent/ice_agent.hpp>
#include <ss_p2p/ice_agent/stun_server.hpp>
#include <ss_p2p/ice_agent/ice_observer.hpp>
#include <ss_p2p/kademlia/direct_routing_table_controller.hpp>
#include <ss_p2p/node_controller.hpp>
#include "boost/asio.hpp"

#include <memory>
#include <mutex>
#include <vector>
#include <thread>
#include <chrono>
#include <optional>
#include <condition_variable>


class temp
{
public:
  void get_binding_response( std::optional<ip::udp::endpoint> ep )
  {
  if( ep == std::nullopt ) std::cout << "binding_request error" << "\n";
  else std::cout << "binding_request done " << *ep << "\n";
  }
};


using namespace boost::asio;


int setup_stun_client()
{

  std::shared_ptr<io_context> io_ctx = std::make_shared<io_context>();
  ip::udp::endpoint self_ep( ip::udp::v4(), 8200 );

  ip::udp::endpoint stun_server_ep( ip::address::from_string("127.0.0.1"), 8600 );

  ip::udp::endpoint stun_server_dummy_1( ip::address::from_string("223.112.1.1"), 1000 );
  ss::kademlia::k_node stun_server_dummy_node_1( stun_server_dummy_1 );
  ip::udp::endpoint stun_server_dummy_2( ip::address::from_string("10.22.0.10"), 2000 );
  ss::kademlia::k_node stun_server_dummy_node_2( stun_server_dummy_2 );

  ss::node_controller n_controller( self_ep, io_ctx );
  n_controller.start();
  std::this_thread::sleep_for( std::chrono::seconds(1) );




  auto &ice_agent = n_controller.get_ice_agent();
  auto &stun_server = ice_agent.get_stun_server();
  auto &ice_obs_strage = ice_agent.get_observer_strage();
  auto &routing_table = n_controller.get_routing_table();
  ss::kademlia::direct_routing_table_controller d_routing_table_controller( routing_table );


  routing_table.auto_update( stun_server_dummy_node_1 );
  routing_table.auto_update( stun_server_dummy_node_2 );

  std::vector<ip::udp::endpoint> target_eps; // binding request 送信先
  target_eps.push_back( stun_server_ep );
 


  
  temp t;

  auto ctx = stun_server.binding_request( target_eps );
  auto ret = ctx->sync_get();
  if( ret != std::nullopt ) std::cout << "binding_request done -> " << *ret << "\n";
  else std::cout << "binding_request error" << "\n";
  // ctx->async_get( std::bind( &temp::get_binding_response, std::ref(t), std::placeholders::_1 ) );

 


  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock);



  return 0;

  /*
  ip::udp::endpoint peer_1( ip::address::from_string("127.0.0.1"), 1000 );
  ip::udp::endpoint peer_2( ip::address::from_string("133.3.0.1"), 2000 );
  ip::udp::endpoint peer_3( ip::address::from_string("0.0.0.0"), 3000 );
  ip::udp::endpoint peer_4( ip::address::from_string("10.219.111.100"), 1000 );
  ip::udp::endpoint peer_5( ip::address::from_string("11.219.111.100"), 2000 );

  ip::udp::endpoint peer_1_response( ip::address::from_string("223.123.33.1"), 200 );
  ip::udp::endpoint peer_2_response( ip::address::from_string("127.0.0.1"), 300 );
  ip::udp::endpoint peer_3_response( ip::address::from_string("133.12.0.1"), 300 );
  ip::udp::endpoint peer_4_response( ip::address::from_string("222.12.0.1"), 300 );
  ip::udp::endpoint peer_5_response( ip::address::from_string("222.12.0.1"), 300 );
  
   
  ss::ice::binding_request binding_request_obs( *io_ctx, ice_agent.get_ice_sender(), d_routing_table_controller );


  binding_request_obs.add_requested_ep( peer_1 );
  binding_request_obs.add_requested_ep( peer_2 );
  binding_request_obs.add_requested_ep( peer_3 );
  binding_request_obs.add_requested_ep( peer_4 );
  binding_request_obs.add_requested_ep( peer_5 );

  binding_request_obs.add_response( peer_1, peer_1_response );
  binding_request_obs.add_response( peer_2, peer_2_response );
  binding_request_obs.add_response( peer_3, peer_3_response );
  binding_request_obs.add_response( peer_4, peer_4_response );
  binding_request_obs.add_response( peer_5, peer_5_response );

  unsigned int count = 0;
  for( auto itr : binding_request_obs._responses )
  {
	std::cout << "("<< count+1 << ") " ;
	std::cout << itr.first << " : ";
	if( itr.second == std::nullopt )  std::cout << "-" << "\n";
	else std::cout << *(itr.second) << "\n";
	count++;
  }

  auto ret = binding_request_obs.global_ep_consensus();
  std::cout << ret.state << "\n";
  std::cout << *(ret.ep) << "\n";
  */


  /*
  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock);
  */


  return 0;
}
