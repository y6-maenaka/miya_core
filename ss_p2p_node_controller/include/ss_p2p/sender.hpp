#ifndef C43E8114_AAEE_43C0_B816_07ECAD6C5E83
#define C43E8114_AAEE_43C0_B816_07ECAD6C5E83


#include <ss_p2p/socket_manager.hpp>
#include <ss_p2p/message.hpp>
#include <json.hpp>

#include "boost/asio.hpp"


using namespace boost::asio;
using json = nlohmann::json;


namespace ss
{


class sender
{
public:
  template < typename SuccessHandler >
  void async_send( ip::udp::endpoint dest_ep, std::string param, const json &payload, SuccessHandler handler )
  {
	message msg(_app_id);
	msg.set_param(param, payload);

	auto enc_msg = message::encode( msg );

	_sock_manager.self_sock().async_send_to(
		  boost::asio::buffer( enc_msg )
		  , dest_ep
		  , handler
		);

	/* #if SS_CAPTURE_PACKET
	std::cout << "(send) -> " << dest_ep << "\n";
	#endif */
  }

  template < typename SuccessHandler >
  void async_send( ip::udp::endpoint dest_ep, message &msg, SuccessHandler handler )
  {
	auto enc_msg = message::encode( msg );

	_sock_manager.self_sock().async_send_to(
		  boost::asio::buffer( enc_msg )
		  , dest_ep
		  , handler
		);

	/* #if SS_CAPTURE_PACKET
	std::cout << " (send) -> " << dest_ep << "\n";
	#endif */
  }

  bool sync_send( ip::udp::endpoint dest_ep, message &msg );
  bool sync_send( ip::udp::endpoint dest_ep, std::string param, json &payload );

  sender( udp_socket_manager &sock_manager, message::app_id id );

  using send_func = std::function<void(ip::udp::endpoint &dest_ep, std::string, json payload)>;
private:
  void on_send_done( const boost::system::error_code &ec );

  udp_socket_manager &_sock_manager;
  message::app_id _app_id;
};


};


#endif
