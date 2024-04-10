#ifndef EE4EE0BA_DCE6_4CDD_9CA3_3C58E76154DC
#define EE4EE0BA_DCE6_4CDD_9CA3_3C58E76154DC


#include <span>

#include "boost/asio.hpp"
#include <ss_p2p/message.hpp>
#include <ss_p2p/socket_manager.hpp>
#include "ice_message.hpp"


using namespace boost::asio;


namespace ss
{
namespace ice
{


class ice_sender
{
private:
  udp_socket_manager &_sock_manager;
  ip::udp::endpoint &_glob_self_ep;
  message::app_id _app_id;

public:
  template < typename SuccessHandler >
  void async_send( ip::udp::endpoint &dest_ep, std::string param, json &payload, SuccessHandler handler ) // 指定したパラメータに乗せて送信
  {
	message msg = message(_app_id);
	msg.set_param( param, payload );

	auto enc_msg = message::encode( msg );

	_sock_manager.self_sock().async_send_to(
		  boost::asio::buffer( enc_msg )
		  , dest_ep
		  , handler
		);

	#if SS_CAPTURE_PACKET
	std::cout << dest_ep << " (async send)" << "\n";
	#endif
  }

  template < typename SuccessHandler >
  void async_ice_send( ip::udp::endpoint &dest_ep, ice_message &ice_msg, SuccessHandler handler ) // iceメッセージに乗せて送信
  {
	message msg = message(_app_id);
	msg.set_param("ice_agent", ice_msg.encode() );

	auto enc_msg = message::encode( msg );

	_sock_manager.self_sock().async_send_to
	  (
		boost::asio::buffer( enc_msg )
		, dest_ep
		, handler
	  );
  }

  std::size_t sync_send( ip::udp::endpoint &dest_ep, std::string param, json &payload );
  std::size_t sync_ice_send( ip::udp::endpoint &dest_ep, ice_message &ice_msg );

  void on_send_done( const boost::system::error_code &ec );

  ice_sender( udp_socket_manager &sock_manager, ip::udp::endpoint &glob_self_ep, message::app_id id );
  ip::udp::endpoint &get_self_endpoint() const;
};


};
};


#endif
