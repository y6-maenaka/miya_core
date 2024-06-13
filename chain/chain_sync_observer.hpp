#ifndef AE9DBD80_1759_4B05_A0EA_B507FF2CFAF9
#define AE9DBD80_1759_4B05_A0EA_B507FF2CFAF9


#include <ss_p2p/observer.hpp>
#include <ss_p2p/peer.hpp>
#include <node_gateway/message/message.hpp>

#include <string>
#include <functional>
#include "boost/asio.hpp"


using namespace boost::asio;


namespace chain
{


constexpr int DEFUALT_OBSERVER_EXPIRE_COUNT = 5;


struct transition_context 
{
  // observerの遷移法を制御するコンテキスト
};

class chain_sync_observer : public ss::base_observer
{
public:
  chain_sync_observer( io_context &io_ctx, std::string t_name );
  virtual void income_message() = 0; // レスポンスが到着した時のエントリーポイント

protected:
  void decrement_counter();
  void increment_counter(); 

  std::function<void(void)> on_success_handler;
  std::function<void(void)> on_failure_handler;
  std::function<void(void)> on_notfound_handler;

  /*
  virtual void on_success() = 0;
  virtual void on_failure() = 0;
  virtual void on_notfound() = 0; // 他peerにリクエストした要素に対してnotfoundが帰ってきた場合
  */

private:
  int expire_counter = DEFUALT_OBSERVER_EXPIRE_COUNT; 
};

class getdata_observer : public chain_sync_observer
{
public:
  void init();
  void income_message( ss::peer::ref peer, miya_core_command::ref cmd );
}; using getdata_obs = class getdata_observer;

class block_observer : public chain_sync_observer
{

}; using block_obs = class block_observer;

class get_header_observer : public chain_sync_observer
{

}; using get_header_obs = class get_header_observer;


};


#endif 
