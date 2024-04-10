#include <ss_p2p/sender.hpp>


namespace ss
{


sender::sender( udp_socket_manager &sock_manager, message::app_id id ) :
  _sock_manager( sock_manager )
  , _app_id( id )
{
  return;
}

bool sender::sync_send( ip::udp::endpoint dest_ep, message &msg )
{
  auto enc_msg = message::encode( msg );
  boost::system::error_code ec;

  _sock_manager.self_sock().send_to( boost::asio::buffer(enc_msg)
	  , dest_ep
	  , 0
	  , ec );

  if( !ec ) return true;
  return false;
}

bool sender::sync_send( ip::udp::endpoint dest_ep, std::string param, json &payload )
{
  message msg = message(_app_id);
  msg.set_param( param, payload );

  auto enc_msg = message::encode( msg );
  boost::system::error_code ec;

  _sock_manager.self_sock().send_to( boost::asio::buffer(enc_msg)
	  , dest_ep
	  , 0
	  , ec );

  if( !ec ) return true;
  return false;
}

void sender::on_send_done( const boost::system::error_code &ec )
{
  #if SS_VERBOSE
  if( !ec ) ;//std::cout << "sender:: send success" << "\n";
  else std::cout << "(sender)" << "\x1b[31m" << " send failure" << "\x1b[39m" << "\n";
  #endif
}


}; // namespace ss
