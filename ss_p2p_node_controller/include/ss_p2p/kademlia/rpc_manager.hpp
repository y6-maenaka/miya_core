#ifndef C93919D3_F914_40AA_9B56_FA0F3445F739
#define C93919D3_F914_40AA_9B56_FA0F3445F739


#include <functional>
#include <memory>
#include <string>

#include <hash.hpp>
#include <ss_p2p/observer.hpp>
#include <ss_p2p/sender.hpp>
#include "./k_observer.hpp"
#include "./k_observer_strage.hpp"
#include "./node_id.hpp"
#include "./k_routing_table.hpp"
#include "./direct_routing_table_controller.hpp"

#include "boost/asio.hpp"


using namespace boost::asio;


namespace ss
{
namespace kademlia
{


constexpr unsigned short DEFAULT_FIND_NODE_SIZE = 5; // find_nodeで応答するノード数
class k_observer_strage;
  

class rpc_manager
{
public:
  using s_send_func = std::function<void(ip::udp::endpoint&, std::string, json)>;
  rpc_manager( node_id &self_id, io_context &io_ctx, k_observer_strage &obs_strage, sender &sender, s_send_func &send_func );

  int income_message( std::shared_ptr<message> msg, ip::udp::endpoint &ep );

  k_routing_table &get_routing_table();
  direct_routing_table_controller &get_direct_routing_table_controller();
  void update_self_id( node_id &id );
 
  /* リクエスト系メソッド */
  using on_pong_handler = ping::on_pong_handler; // ping responseが到着した時に呼び出される
  using on_ping_timeout_handler = ping::on_timeout_handler ;
  using on_find_node_response_handler = find_node::on_response_handler;
  void ping_request( ip::udp::endpoint ep, on_pong_handler pong_handler, on_ping_timeout_handler timeout_handler ); // pingを送信する
  void find_node_request( ip::udp::endpoint ep, std::vector<ip::udp::endpoint> ignore_eps, on_find_node_response_handler handler );

  /* レスポンス系メソッド */
  void ping_response( k_message &k_msg, ip::udp::endpoint &ep );
  void find_node_response( k_message &k_msg, ip::udp::endpoint &ep );

  void null_handler( ip::udp::endpoint ep ); // pign-responseに対する特に何もしないハンドラ
protected:
  int income_request( k_message &k_msg, ip::udp::endpoint &ep );
  int income_response( k_message &k_msg, ip::udp::endpoint &ep ); // observerで処理仕切れなかったメッセージが入ってくる
  
  void on_send_done( const boost::system::error_code &ec );

private:
  k_routing_table _routing_table;
  direct_routing_table_controller _d_routing_table_controller;

  node_id &_self_id;
  io_context &_io_ctx;
  deadline_timer _tick_timer;
  sender &_sender;
  s_send_func &_s_send_func;
  k_observer_strage &_obs_strage;
};



};
};


#endif
