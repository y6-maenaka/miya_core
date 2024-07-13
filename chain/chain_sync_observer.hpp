#ifndef AE9DBD80_1759_4B05_A0EA_B507FF2CFAF9
#define AE9DBD80_1759_4B05_A0EA_B507FF2CFAF9


#include <ss_p2p/observer.hpp>
#include <ss_p2p/peer.hpp>
#include <node_gateway/message/message.hpp>
#include <node_gateway/message/command/command.params.hpp>
#include <chain/block/block.params.hpp>

#include <string>
#include <functional>
#include "boost/asio.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/algorithm/string/erase.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/lexical_cast.hpp"


using namespace boost::asio;
using namespace boost::uuids;


namespace chain
{


constexpr int DEFUALT_OBSERVER_EXPIRE_COUNT = 5;


template < std::size_t N > ss::base_observer::id generate_chain_sync_observer_id( const COMMAND_TYPE_ID type_id, const base_id<N> &id )
  // chain_sync用のobserver_idを生成
{
  return ss::generate_uuid_from_str( std::to_string(static_cast<int>(type_id)) + id.to_string() );
}


class chain_sync_observer : public ss::base_observer
{
public:
  chain_sync_observer( io_context &io_ctx, std::string t_name = "chain_sync", const ss::base_observer::id &id_from = ss::base_observer::id() );
  virtual void income_command() = 0; // レスポンスが到着した時のエントリーポイント

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
  getdata_observer( io_context &io_ctx, const ss::base_observer::id &id_from = ss::base_observer::id(), std::string t_name = "getdata" );

  void init( ss::peer::ref peer_ref, const BLOCK_ID &block_id );
  void income_command() override;
  void income_message( ss::peer::ref peer, miya_core_command::ref cmd );

private: 
  const ss::peer::ref _peer = nullptr;

}; using getdata_obs = class getdata_observer;

class block_observer : public chain_sync_observer
{

}; using block_obs = class block_observer;

class get_header_observer : public chain_sync_observer
{

}; using get_header_obs = class get_header_observer;


};


#endif 
