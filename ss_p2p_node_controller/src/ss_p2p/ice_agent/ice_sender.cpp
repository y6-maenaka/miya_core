#include <ss_p2p/ice_agent/ice_sender.hpp>


namespace ss
{
namespace ice
{


ice_sender::ice_sender( udp_socket_manager &sock_manager, ip::udp::endpoint &glob_self_ep, message::app_id id ) :
  _sock_manager( sock_manager )
  , _glob_self_ep( glob_self_ep )
  , _app_id( id )
{
  return;
}

std::size_t ice_sender::sync_send( ip::udp::endpoint &dest_ep, std::string param, json &payload )
{
  message msg = message(_app_id);
  msg.set_param( param, payload );

  auto enc_msg = message::encode( msg );
  boost::system::error_code ec;

  std::size_t bytes = _sock_manager.self_sock().send_to( boost::asio::buffer(enc_msg)
	  , dest_ep
	  , socket_base::message_out_of_band
	  , ec );

  if( !ec ) return bytes;
  return 0;
}

std::size_t ice_sender::sync_ice_send( ip::udp::endpoint &dest_ep, ice_message &ice_msg )
{
  message msg = message(_app_id);
  msg.set_param("ice_agent", ice_msg.encode() );

  auto enc_msg = message::encode( msg );
  boost::system::error_code ec;

  std::size_t bytes = _sock_manager.self_sock().send_to( boost::asio::buffer(enc_msg)
	  , dest_ep
	  , 0
	  , ec );

  if( !ec ) return bytes;
  return 0;
}

void ice_sender::on_send_done( const boost::system::error_code &ec )
{
  #if SS_VERBOSE
  if( !ec ) std::cout << "ice_sender::send success" << "\n";
  else std::cout << "ice_sender::send error" << "\n";
  #endif
}

ip::udp::endpoint &ice_sender::get_self_endpoint() const
{
  return _glob_self_ep;
}


};
};
