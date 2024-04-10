#include <ss_p2p/ice_agent/ice_agent.hpp>
#include <ss_p2p/message.hpp>
#include <utils.hpp>

#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <iostream>
#include <memory>

#include "boost/asio.hpp"


using namespace boost::asio;



int setup_signaling_response_host()
{

 
  ip::udp::endpoint self_endpoint( ip::udp::v4(), 8100 ); // nat到達先
  std::shared_ptr<io_context> io_ctx = std::make_shared<io_context>();
  ss::node_controller n_controller( self_endpoint, io_ctx );
  n_controller.start();


  auto& routing_table = n_controller.get_routing_table();
  auto& ice_agent = n_controller.get_ice_agent();


  // テスト用に一旦自身のグローバルIPを変更する
  ip::udp::endpoint new_globa_self_endpoint( ip::address::from_string("127.0.0.1"), 8100 );
  ice_agent.update_global_self_endpoint( new_globa_self_endpoint );
  std::cout << "updated global self endpoint" << "\n";
  
  auto &ice_observer_strage = ice_agent.get_observer_strage();
  // ice_observer_strage.show_state( boost::system::error_code{} );

  ip::udp::endpoint nat_peer( ip::address::from_string("127.0.0.1"), 8090 );
  auto peer = n_controller.get_peer( nat_peer );
  for( ;; )
  {
	auto received_msg = peer.receive();
	std::cout << "メッセージ受信" << "\n";
  }

  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock);



  return 0;
}
