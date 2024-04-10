#include <ss_p2p/ice_agent/ice_agent.hpp>
#include <ss_p2p/ice_agent/ice_observer.hpp>
#include <ss_p2p/ice_agent/ice_observer_strage.hpp>
#include <ss_p2p/kademlia/k_node.hpp>
#include <ss_p2p/kademlia/node_id.hpp>
#include <ss_p2p/kademlia/k_observer.hpp>
#include <ss_p2p/kademlia/k_observer_strage.hpp>
#include <ss_p2p/kademlia/k_routing_table.hpp>
#include <ss_p2p/kademlia/rpc_manager.hpp>
#include <ss_p2p/kademlia/direct_routing_table_controller.hpp>
#include <ss_p2p/socket_manager.hpp>
#include <ss_p2p/message.hpp>
#include <ss_p2p/observer.hpp>
#include <ss_p2p/node_controller.hpp>
#include <ss_p2p/socket_manager.hpp>
#include <ss_p2p/udp_server.hpp>
#include <utils.hpp>
#include <json.hpp>

#include <iostream>
#include <vector>
#include <span>
#include <memory>
#include <string>
#include <mutex>
#include <condition_variable>

#include "boost/asio.hpp"


using namespace boost::asio;
using json = nlohmann::json;


void show_buff( std::span<char> buff, ip::udp::endpoint &src_ep )
{
  for( auto itr : buff ) std::cout << itr << "\n";
  std::cout << "\n";
  std::cout << src_ep << "\n";
}


int setup_ice_agent()
{
  #if SS_DEBUG


  ss::message::app_id id = {'a','b','c','d','e','f','g','h'};

  std::shared_ptr<io_context> io_ctx = std::make_shared<io_context>();
  ip::udp::endpoint self_endpoint( ip::udp::v4(), 8090 );

  ss::node_controller n_controller( self_endpoint, io_ctx );
  n_controller.start();


  // ルーティングテーブルのセットアップ
  auto &routing_table = n_controller.get_routing_table();
  auto &ice_agent = n_controller.get_ice_agent();


  // ダミーのピアをルーティングテーブルに追加
  /* for( int i=0; i<20; i++ )
  {
	ip::udp::endpoint dummy_ep = ss::generate_random_endpoint();
	ss::kademlia::k_node dummy_k_node(dummy_ep);
	routing_table.auto_update( dummy_k_node );
  } */

  ip::udp::endpoint test_relay_endpoint( ip::address::from_string("127.0.0.1") , 8500 ); // ルーティングテーブルに存在するアドレスを仮定
  routing_table.auto_update( test_relay_endpoint );


  // ice_agentのセットアップ
  auto s_send_func = ice_agent.get_signaling_send_func();
  
  ip::udp::endpoint local_dest_ep( ip::address::from_string("127.0.0.1"), 8100 );
  json payload; payload["greet"] = "Hello World";
  s_send_func( local_dest_ep, "body", payload );

  // ice_observer_strage.show_state( boost::system::error_code{} );

  

  #endif

  
  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait( lock );


  std::cout << "setup_ice_agent done" << "\n";
  return 0;
}
