#include <ss_p2p/ice_agent/ice_agent.hpp>
#include <ss_p2p/socket_manager.hpp>
#include <ss_p2p/node_controller.hpp>
#include <ss_p2p/udp_server.hpp>
#include <ss_p2p/message.hpp>
#include <utils.hpp>

#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

#include "boost/asio.hpp"


using namespace boost::asio;


int setup_signaling_relay_host()
{

  ip::udp::endpoint self_endpoint( ip::udp::v4(), 8500 );
  std::shared_ptr<io_context> io_ctx = std::make_shared<io_context>();
  ss::node_controller n_controller( self_endpoint, io_ctx );
  n_controller.start();

  auto& routing_table = n_controller.get_routing_table();
  auto& ice_agent = n_controller.get_ice_agent();

  // 転送用にダミーのノードをルーティングテーブルに追加
  for( int i=0; i<2; i++ ){
	ip::udp::endpoint dummy_ep = ss::generate_random_endpoint();
	k_node dummy_k_node( dummy_ep );
	routing_table.auto_update( dummy_k_node );
  }

  // nat到達先ノードを追加( 検証用 )
  ip::udp::endpoint nat_dest_ep( ip::address::from_string("127.0.0.1"), 8100 ); // nat目標ep
  k_node nat_dest_k_node(nat_dest_ep);
  routing_table.auto_update( nat_dest_k_node );


  std::this_thread::sleep_for( std::chrono::seconds(1) );
  auto &ice_obs_strage = ice_agent.get_observer_strage();
  ice_obs_strage.show_state( boost::system::error_code{} );


  std::cout << "back to main thread" << "\n";
  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock);

  return 0;
}
