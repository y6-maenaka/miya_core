#include <ss_p2p/node_controller.hpp>

#include "boost/asio.hpp"

#include <span>
#include <mutex>
#include <condition_variable>


using namespace boost::asio;


int main( int argc, const char *argv[] )
{
  std::span args( argv, argc );
  ip::udp::endpoint self_ep( ip::udp::v4(), std::atoi(args[1]) );

  ss::node_controller n_controller( self_ep );


  std::mutex mtx;
  std::condition_variable cv;
  
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait( lock );
}
