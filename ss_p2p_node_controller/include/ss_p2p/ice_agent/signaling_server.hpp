#ifndef BD51C051_D379_4330_8C9D_0B9E15DFEFC9
#define BD51C051_D379_4330_8C9D_0B9E15DFEFC9


#include <functional>
#include <memory>
#include <string>
#include <array>
#include <span>

#include <ss_p2p/message.hpp>
#include <ss_p2p/observer.hpp>
#include <ss_p2p/sender.hpp>
#include <ss_p2p/socket_manager.hpp>
#include <ss_p2p/kademlia/k_routing_table.hpp>
#include <ss_p2p/kademlia/direct_routing_table_controller.hpp>
#include <ss_p2p/ss_logger.hpp>
#include <json.hpp>

#include "./ice_message.hpp"

#include "boost/asio.hpp"


using namespace boost::asio;
using namespace ss::kademlia;
using json = nlohmann::json;


namespace ss
{
namespace ice
{


class ice_sender;
class ice_observer_strage;


class signaling_server
{
private:
  void signaling_send( ip::udp::endpoint &dest_ep, std::string root_param, json payload );

  void async_hello( const boost::system::error_code &ec ); // debug
  void send_done( const boost::system::error_code &ec );
  void on_send_done( const boost::system::error_code &ec );
  ice_message format_relay_msg( ice_message &base_msg );

public:
  using s_send_func = std::function<void(ip::udp::endpoint &dest_ep, std::string, const json payload)>;
  s_send_func get_signaling_send_func();

  signaling_server( io_context &io_ctx, sender &sender, ice_sender& ice_sender, ip::udp::endpoint &glob_self_ep, direct_routing_table_controller &d_routing_table_controller, ice_observer_strage &obs_strage, ss_logger *logger );

  int income_message( std::shared_ptr<ss::message> msg, ip::udp::endpoint &ep );

  // members
  io_context &_io_ctx;
  direct_routing_table_controller &_d_routing_table_controller;
  sender &_sender;
  ice_sender &_ice_sender;
  ice_observer_strage &_obs_strage; // add専用
  ip::udp::endpoint &_glob_self_ep;
  ss_logger *_logger;
};


}; // namespace ice
}; // namespace ss


#endif 


