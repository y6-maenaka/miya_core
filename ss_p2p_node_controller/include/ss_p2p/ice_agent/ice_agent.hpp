#ifndef A3401654_2A6C_4D83_ACCD_0FAAE564EA70
#define A3401654_2A6C_4D83_ACCD_0FAAE564EA70


#include <functional>
#include <string>
#include <memory>

#include <ss_p2p/kademlia/direct_routing_table_controller.hpp>
#include <ss_p2p/message.hpp>
#include <ss_p2p/socket_manager.hpp>
#include <ss_p2p/sender.hpp>
#include <ss_p2p/ss_logger.hpp>
#include <utils.hpp>
#include "./ice_message.hpp"
#include "./ice_sender.hpp"
#include "./ice_observer_strage.hpp"
#include "./signaling_server.hpp"
#include "./stun_server.hpp"

#include "boost/asio.hpp"


using namespace boost::asio;


namespace ss
{
namespace ice
{


class ice_agent
{
public:
  ice_agent( io_context &io_ctx, udp_socket_manager &sock_manager, ip::udp::endpoint &glob_self_ep, message::app_id id, sender &sender, ss::kademlia::direct_routing_table_controller &d_routing_table_controller, ss_logger *logger );
  void hello();

  int income_message( std::shared_ptr<message> msg, ip::udp::endpoint &ep /*original_src_ep*/ ); // メッセージ受信
  
  signaling_server::s_send_func get_signaling_send_func();
  void update_global_self_endpoint( ip::udp::endpoint &ep );

  stun_server &get_stun_server();

private:
  #if SS_DEBUG
public:
  ice_observer_strage &get_observer_strage();
  #endif

  udp_socket_manager &_sock_manager;
  ip::udp::endpoint &_glob_self_ep; // グローバルendpoing
  message::app_id _app_id;
  sender &_sender;

  signaling_server _sgnl_server;
  stun_server _stun_server;
  ice_observer_strage _obs_strage;

  ice_sender _ice_sender;
  ice_sender& get_ice_sender();
  ss_logger *_logger;
};


}; // namespace ice
}; // namespace ss


#endif 


