#ifndef D9FF498B_70E3_4A08_98DF_231CCB8B904D
#define D9FF498B_70E3_4A08_98DF_231CCB8B904D


#include <memory>
#include <functional>

#include <ss_p2p/observer.hpp>
#include <ss_p2p/message.hpp>
#include <utils.hpp>
#include <ss_p2p/kademlia/k_message.hpp>
#include <ss_p2p/kademlia/k_node.hpp>
#include <ss_p2p/kademlia/k_routing_table.hpp>

#include "boost/asio.hpp"


using namespace boost::asio;
using namespace boost::uuids;


namespace ss
{
namespace kademlia
{


constexpr unsigned int DEFAULT_PING_RESPONSE_TIMEOUT_s = 5; // デフォルトのpong待機時間
class rpc_manager;


class k_observer : public ss::base_observer
{
public:
  k_observer( io_context &io_ctx );
};


class ping : public k_observer
{
public:
  using on_pong_handler = std::function<void(ip::udp::endpoint)>; // 引数は勝手にバイドすること
  using on_timeout_handler = std::function<void(ip::udp::endpoint)>;
  ping( io_context &io_ctx, ip::udp::endpoint ep/* 一応保持しておく*/, on_pong_handler pong_handler, on_timeout_handler timeout_handler );

  int income_message( message &msg, ip::udp::endpoint &ep ); // このメソッドをタイマーセットしてio_ctxにポスト
  void timeout( const boost::system::error_code &ec );
  void init();
  void print() const;

private:
  bool _is_pong_arrived:1;
  deadline_timer _timer; 

  ip::udp::endpoint _dest_ep;
  on_pong_handler _pong_handler;
  on_timeout_handler  _timeout_handler;
};

class find_node : public k_observer
{
public:
  using on_response_handler = std::function<void(ip::udp::endpoint)>;
  find_node( io_context &io_ctx, on_response_handler response_handler ); // find_nodeで見つかったノードに対して全てresponse_handlerを呼び出す
  void init();
  int income_message( message &msg, ip::udp::endpoint &ep );
  void print() const;

private:
  on_response_handler _response_handler;
};


using base_observer_ptr = std::shared_ptr<base_observer>;
const std::string generate_h_id();


};
};



#endif
