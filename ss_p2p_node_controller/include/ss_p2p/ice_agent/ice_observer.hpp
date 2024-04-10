#ifndef F0CCAA63_1708_4F32_AE3B_810C8652A7B4
#define F0CCAA63_1708_4F32_AE3B_810C8652A7B4


#include <memory>
#include <functional>
#include <optional>
#include <vector>
#include <map>
#include <algorithm>

#include <ss_p2p/observer.hpp>
#include <ss_p2p/kademlia/direct_routing_table_controller.hpp>
#include <ss_p2p/message.hpp>
#include <ss_p2p/sender.hpp>
#include <utils.hpp>
#include "./ice_message.hpp"
#include "./stun_server.hpp"

#include "boost/asio.hpp"

using namespace boost::asio;
using namespace ss::kademlia;


namespace ss
{
namespace ice
{


class ice_sender;
constexpr unsigned short DEFAULT_SIGNALING_OPEN_TTL = 5;
constexpr unsigned short MAXIMUM_BINDING_REQUEST_INQUIRE_COUNT = 6; // デフォルトでbinding_requestを送信するノード数
constexpr unsigned short DEFAULT_BINDING_REQUEST_TIMEOUT_s = 4;


class signaling_observer : public ss::base_observer
{
public:
  signaling_observer( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ip::udp::endpoint &glob_self_ep, ss::kademlia::direct_routing_table_controller &d_routing_table_controller );
protected:
  ss::kademlia::direct_routing_table_controller &_d_routing_table_controller;
  sender &_sender;
  ice_sender &_ice_sender;
  ip::udp::endpoint &_glob_self_ep;
};

class signaling_request : public signaling_observer
{
public:
  void init( ip::udp::endpoint &dest_ep, std::string parma, json payload );
  void timeout();
  int income_message( ss::message &msg, ip::udp::endpoint &ec/*src_ep*/ );
  void print() const;

  struct msg_cache
  {
	ip::udp::endpoint ep;
	std::string param;
	json payload;

  } _msg_cache;

  signaling_request( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ip::udp::endpoint &glob_self_ep, ss::kademlia::direct_routing_table_controller &d_routing_table_controller );
  void on_send_done( const boost::system::error_code &ec );
private:
  json format_request_msg( ip::udp::endpoint &src_ep, ip::udp::endpoint &dest_ep ); // リクエストメッセージのフォーマット

  void on_traversal_done( const boost::system::error_code &ec );
  void send_dummy_message( ip::udp::endpoint &ep );
  
  bool _done = false;
};

class signaling_response : public signaling_observer
{
public:
  signaling_response( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ip::udp::endpoint &glob_self_ep, direct_routing_table_controller &d_routing_table_controller );
  void init( const boost::system::error_code &ec ); // 有効期限を設定する
  int income_message( message &msg, ip::udp::endpoint &ep );
  void on_send_done( const boost::system::error_code &ec );
  void print() const;
};

class signaling_relay : public signaling_observer
{
public:
  signaling_relay( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ip::udp::endpoint &glob_self_ep, direct_routing_table_controller &d_routing_table_controller );
  void init();
  int income_message( message &msg, ip::udp::endpoint &ep );
  void on_send_done( const boost::system::error_code &ec );
  void print() const;
};


class stun_observer : public ss::base_observer
{
public:
  void init( ip::udp::endpoint &glob_self_ep );
  int income_message( message &msg, ip::udp::endpoint &ep );
  void print() const;
  
  stun_observer( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ss::kademlia::direct_routing_table_controller &_d_routing_table_controller );
private:
  sender &_sender;
  ice_sender &_ice_sender;
  ss::kademlia::direct_routing_table_controller &_d_routing_table_controller;
};

class binding_request : public stun_observer
{
public:
  void init( std::shared_ptr<stun_server::sr_object> sr );

  struct consensus_ctx
  {
	enum state_t
	{
	  done 
		, on_handling // まだ処理中
		, error_done // エラー終了
	};
	state_t state;
	std::optional<ip::udp::endpoint> ep;
  };
  consensus_ctx global_ep_consensus( bool is_force = false /*現時点で受信済みの応答だけでグローバルIPの結果を得る*/ ); // 受信した結果から最もらしいglobal_epを選出する

  void on_timeout( const boost::system::error_code &ec );
  void print() const;
  int income_message( message &msg, ip::udp::endpoint &ep );
  binding_request( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ss::kademlia::direct_routing_table_controller &_d_routing_table_controller );

  void update_sr( const consensus_ctx &cctx );
  void async_call_sr_handler( std::optional<ip::udp::endpoint> ep ); // スレッドセーフでhandlerを呼び出す
  void add_requested_ep( ip::udp::endpoint ep );
  void add_response( ip::udp::endpoint &src_ep, ip::udp::endpoint response_ep /*レスポンス*/ );

private:
#if SS_DEBUG
public:
#endif
  deadline_timer _timer; // for notice timeout
  std::vector <std::pair<ip::udp::endpoint/*問い合わせ先*/, std::optional<ip::udp::endpoint>/*問い合わせ結果*/> > _responses; // stun_requestを送信したノード一覧
  std::shared_ptr<stun_server::sr_object> _sr;
  bool _is_handler_called:1;
  bool _is_timeout:1;
};

/*class binding_response // 使わないかも(単にstun_serverがレスポンスする)
{
}; */


}; // namespace ice
}; // namespace ss


#endif


