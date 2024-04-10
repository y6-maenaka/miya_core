#include <ss_p2p/ice_agent/stun_server.hpp>
#include <ss_p2p/node_controller.hpp>

#include "boost/asio.hpp"

#include <memory>
#include <mutex>
#include <condition_variable>

  
using namespace boost::asio;


int setup_stun_server()
{
  std::shared_ptr<io_context> io_ctx = std::make_shared<boost::asio::io_context>();
  ip::udp::endpoint self_ep( ip::udp::v4(), 8600 );

  ss::node_controller n_controller( self_ep, io_ctx );
  n_controller.start();


  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock);

 

  return 0;
}
