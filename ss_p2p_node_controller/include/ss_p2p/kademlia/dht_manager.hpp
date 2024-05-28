#ifndef A6BB07B7_B27C_4C21_97B9_7B2C325B4F43
#define A6BB07B7_B27C_4C21_97B9_7B2C325B4F43


#include <memory>
#include <unordered_map>
#include <functional>

#include <ss_p2p/message.hpp>
#include <ss_p2p/sender.hpp>
#include <ss_p2p/ss_logger.hpp>
#include <json.hpp>
#include <utils.hpp>

#include "./k_bucket.hpp"
#include "./k_routing_table.hpp"
#include "./k_node.hpp"
#include "./node_id.hpp"
#include "./k_observer.hpp"
#include "./k_observer_strage.hpp"
#include "./rpc_manager.hpp"
#include "./k_message.hpp"

#include "boost/asio.hpp"

using namespace boost::asio;
using namespace boost::uuids;
using json = nlohmann::json;


namespace ss
{
namespace kademlia
{

constexpr unsigned short MINIMUM_NODES = 5; // 最低限のノード数


class dht_manager
{
public:
  using s_send_func = std::function<void(ip::udp::endpoint&, std::string, json)>;
  dht_manager( boost::asio::io_context &io_ctx , ip::udp::endpoint &ep, sender &sender, ss_logger *logger );

  int income_message( std::shared_ptr<message> msg, ip::udp::endpoint &ep );
  k_routing_table &get_routing_table();
  direct_routing_table_controller &get_direct_routing_table_controller();
  rpc_manager &get_rpc_manager();

  void update_global_self_endpoint( ip::udp::endpoint &ep );
  void init( s_send_func s_send_func ); 

  #if SS_DEBUG
  k_observer_strage &get_observer_strage();
  #endif

  void start(); // 実質はtickをcallするだけ
  void stop(); // tickのストップ

private:
  io_context &_io_ctx;
  deadline_timer _tick_timer;
  ip::udp::endpoint &_self_ep;
  node_id _self_id;
  rpc_manager _rpc_manager;
  k_observer_strage _obs_strage;
  sender &_sender;
  s_send_func _s_send_func; // initにより初期化され
  ss_logger *_logger;

  void tick();

  class connection_maintainer
  {
	friend class dht_manager;

	std::time_t calc_next_tick_time(); // 次にtickを呼び出すまでの時間を計算する
	void tick();
	void tick_done(); // 次のtickを呼び出すため,必ずtick終了時に呼び出す
	bool _requires_tick;
	void get_remote_nodes();

	void no_handle( ip::udp::endpoint ep );

	std::time_t send_refresh_ping(); // 全てのping_responseが買ってくる時間を取得
 
	connection_maintainer( class dht_manager *manager, rpc_manager &rpc_manager, io_context &io_ctx, ss_logger *logger ); 
	private:
	  dht_manager *_d_manager;
	  io_context &_io_ctx;
	  class rpc_manager &_rpc_manager;
	  deadline_timer _timer;
	  ss_logger *_logger;

  } _maintainer;
};


};
};


#endif 


