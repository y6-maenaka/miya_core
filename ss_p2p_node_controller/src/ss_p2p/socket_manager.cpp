#include <ss_p2p/socket_manager.hpp>


namespace ss
{


udp_socket_manager::udp_socket_manager( ip::udp::endpoint &ep, io_context &io_ctx ) :
  _io_ctx(io_ctx) ,
  _sock(io_ctx) ,
  _ep(ep)
{
  _sock.open( ep.protocol() );
  _sock.bind( ep );
}

io_context &udp_socket_manager::io_ctx()
{
  return _io_ctx;
} 

ip::udp::socket &udp_socket_manager::self_sock()
{
  return _sock;
}


}; // namespace ss

