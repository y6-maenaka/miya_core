#ifndef DF2CC8B7_BABA_4168_BC62_6B54504BC58F
#define DF2CC8B7_BABA_4168_BC62_6B54504BC58F


#include <iostream>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <thread>
#include <chrono>

#include <ss_p2p/message.hpp>
#include <ss_p2p/sender.hpp>
#include <ss_p2p/ss_logger.hpp>

#include "boost/asio.hpp"


using namespace boost::asio;


namespace ss
{


namespace kademlia
{
  class direct_routing_table_controller;
}


namespace ice
{


class ice_sender;
class ice_observer_strage;


class stun_server
{
public:
  stun_server( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ss::kademlia::direct_routing_table_controller &d_routing_table_controller, ice_observer_strage &obs_strage, ss_logger* logger );
  int income_message( std::shared_ptr<message> msg, ip::udp::endpoint &ep );
  void on_send_done( const boost::system::error_code &ec, std::size_t bytes_transferred );

  struct sr_object // stun reserved object  基本的にbinding_request observerに1つ割り当てられる
  {
    using on_nat_traversal_success_handler = std::function<void( std::optional<ip::udp::endpoint> )>; // asyncのときに成功ハンドラとして使う
	sr_object();
	enum state_t
	{
	  done = 0 // 取得完了
	  , pending // 取得処理中
	  , notfound // 取得失敗
	};
	std::optional<ip::udp::endpoint> sync_get();  // 応答が帰ってくるまで実行スレッドをブロッキングする(今の実装だと,node_contollerスレッドから呼び出すと永遠ブロッキングする)
	void async_get( on_nat_traversal_success_handler handler ); // レスポンスを受信したら結果をハンドラに渡す
	
	static sr_object (_error_)();
	static sr_object (_pending_)();

	/* 非同期取得でのみ使用するパラメータ */
	bool is_async() const;
	on_nat_traversal_success_handler handler;
  
	state_t get_state() const;
	void update_state( state_t s, std::optional<ip::udp::endpoint> ep = std::nullopt );
	private:
	  std::shared_ptr<std::mutex> _mtx; // ナンセンスかも
	  std::shared_ptr<std::condition_variable> _cv; 
	  ip::udp::endpoint _global_ep;
	  state_t _state;

	  bool _is_async:1; // 非同期取得のみ使用する
  };

  using sr_ptr = std::shared_ptr<sr_object>;
  [[nodiscard]] sr_ptr binding_request( std::vector<ip::udp::endpoint> target_nodes = std::vector<ip::udp::endpoint>(), unsigned short inquire_count = 3 ); 

private:
  io_context &_io_ctx;
  sender &_sender;
  ice_sender &_ice_sender;
  ice_observer_strage &_obs_strage; // ice_observer_strage
  ss::kademlia::direct_routing_table_controller &_d_routing_table_controller;
  ss_logger *_logger;
};


};
};


#endif 


