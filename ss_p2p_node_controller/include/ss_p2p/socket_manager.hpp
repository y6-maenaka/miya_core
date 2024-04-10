#ifndef E87D59E2_C05E_45E5_B9EA_2CE775846C43
#define E87D59E2_C05E_45E5_B9EA_2CE775846C43


#include <iostream>
#include <variant>
#include <string>
#include <vector>

#include "boost/asio.hpp"


using namespace boost::asio;


namespace ss
{


class udp_socket_manager
{
private:
  ip::udp::socket _sock;
  io_context &_io_ctx;
  ip::udp::endpoint &_ep;

public:
  udp_socket_manager( ip::udp::endpoint &ep, io_context &io_ctx );

  io_context &io_ctx();
  ip::udp::socket& self_sock();
};

class tcp_socket_manager
{
private:
  const ip::tcp::socket _sock;
  std::shared_ptr<io_context> const _io_ctx;
};


using union_socket_manager = std::variant< udp_socket_manager, tcp_socket_manager >;
template< typename T >
concept AllowedSocketManagerTypes = std::is_same_v<T, udp_socket_manager> || std::is_same_v<T, tcp_socket_manager>;

using union_socket = std::variant< ip::udp::socket , ip::tcp::socket >;
template< typename T >
concept AllowedSocketTypes = std::is_same_v<T, ip::udp::socket> || std::is_same_v<T, ip::tcp::socket>;

class socket_manager
{
public:
  template< AllowedSocketManagerTypes T >
  socket_manager( T& from ) : _u_sock_manager(from){};

  enum sock_type
  {
	udp,
	tcp
  };
  sock_type sock_type();

private:
  union_socket_manager _u_sock_manager;
};


}; // namesapce ss


#endif
