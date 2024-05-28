#ifndef F2EBFFE1_1F20_4856_915B_0B15079C07E8
#define F2EBFFE1_1F20_4856_915B_0B15079C07E8


#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <span>
#include <optional>
#include <chrono>

#include <json.hpp>
#include "./message.hpp"
#include "./message_pool.hpp"


#include "boost/asio.hpp"
using json = nlohmann::json;


namespace ss
{


class peer
{
public:
  using s_send_func = std::function<void(ip::udp::endpoint&, std::string, json)>;
  ip::udp::endpoint _ep;

  std::time_t keep_alive = 1;

  bool operator ==(const peer& pr) const;

  std::pair< std::shared_ptr<message>, std::time_t > receive( std::time_t timeout = -1 );
  void send( std::span<char> msg );

  void send( std::string msg );
  peer( ip::udp::endpoint &ep, message_pool::symbolic msg_pool_symbolic ,s_send_func send_func );
  ~peer();

  ip::udp::endpoint get_endpoint();
  void print() const;
private:
  message_pool::symbolic _msg_pool_symbolic;
  // -1: データが到着するまでブロッキングする
  // 0 : すぐに戻す
  // n : n秒間データの到着を待つ
  s_send_func _s_send_func;
};

using peer_ptr = std::shared_ptr<peer>;


}; // namespace ss


#endif 
