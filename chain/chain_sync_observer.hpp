#ifndef AE9DBD80_1759_4B05_A0EA_B507FF2CFAF9
#define AE9DBD80_1759_4B05_A0EA_B507FF2CFAF9


#include <ss_p2p/observer.hpp>
#include <ss_p2p/peer.hpp>
#include <node_gateway/message/message.hpp>
#include <node_gateway/message/command/command.params.hpp>
#include <chain/block/block.params.hpp>

#include <iostream>
#include <string>
#include <set>
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
{ // chain_sync用のobserver_idを生成
  return ss::generate_uuid_from_str( std::to_string(static_cast<int>(type_id)) + id.to_string() );
}


class chain_sync_observer : public ss::base_observer
{
public:
  chain_sync_observer( io_context &io_ctx
	  , ss::peer::ref taget_peer = nullptr
	  , std::string t_name = "chain_sync", const ss::base_observer::id &id_from = ss::base_observer::id() );
  const ss::peer::id get_peer_id() const;
  // virtual void income_command() = 0; // レスポンスが到着した時のエントリーポイント

protected:
  void decrement_counter();
  void increment_counter(); 

  std::function<void(void)> _on_success_handler;
  std::function<void(void)> _on_failure_handler;
  std::function<void(void)> _on_notfound_handler;

  /*
  virtual void on_success() = 0;
  virtual void on_failure() = 0;
  */
  // virtual void on_success( const ss::peer::ref &peer_ref ) = 0;
  // virtual void on_notfound( const ss::peer::ref &peer_ref ) = 0; // 他peerにリクエストした要素に対してnotfoundが帰ってきた場合

  const ss::peer::ref _peer = nullptr;
private:
  int expire_counter = DEFUALT_OBSERVER_EXPIRE_COUNT; 
};

class getdata_observer : public chain_sync_observer
{
public:
  getdata_observer( io_context &io_ctx
  , ss::peer::ref peer_ref, const block_id &block_id 
  , const ss::base_observer::id &id_from = ss::base_observer::id(), std::string t_name = "getdata" );

  void init();
  const MiyaCoreMSG_GETDATA get_command();

  void on_receive( const ss::peer::ref &peer_ref );
  void on_success( const ss::peer::ref &peer_ref );
  // トランザクションやヘッダーが到着したパターンのon_successメソッドも作成する
  
  struct notfound_controller{
    // std::set< std::pair< ss::peer::ref, int >, ss::peer::Hash > _request_history;
  } _notfound_controller;
  void on_notfound( const ss::peer::ref &peer_ref );

  const MiyaCoreMSG_GETDATA _command;

private: 
  const block_id _base_id;

}; using getdata_obs = class getdata_observer;

class block_observer : public chain_sync_observer
{
public:
  block_observer( io_context &io_ctx
  , std::function<void( block::ref block_ref )> on_receive_handler = nullptr
  , std::function<void( block::ref block_ref )> on_success_handler = nullptr
  , std::function<void(void)> on_failure_handler = nullptr
  , const ss::base_observer::id &id_from = ss::base_observer::id()
  , std::string t_name = "block" );

  void on_receive( const ss::peer::ref &peer_ref, block::ref block_ref );

private:
  struct message_context
  {
	// on_receiveした時に受け取ったメッセージが本get_blocksメッセージに対応するものか判断するためのcontext情報を保持
  };
}; using block_obs = class block_observer;

class getheader_observer : public chain_sync_observer
{

}; using getheader_obs = class getheader_observer;

class getblocks_observer : public chain_sync_observer
{
public:
  using ref = std::shared_ptr<getblocks_observer>;
  struct obs_ctx {
	enum state{
	  complete
		, notfound
		, error
	};

	const state status;
	const block_id start_blkid;
	std::vector<const block_id> received_block_id_v; // 本observerにより取得できたブロックidのリスト
  };

  using on_done_notify_func = std::function<void(obs_ctx ctx)>; // 本observerの終了を大元のchain_sync_managerに通知する関数
  getblocks_observer( io_context &io_ctx, ss::peer::ref target_peer, const block_id start_blkid, const block_id stop_blkid /*最終目的のblock_id*/, on_done_notify_func notify_func, std::string t_name = "getblocks" );
  void request();

  bool is_correspond_inv( inv::ref target ) const;  // targetが自身が送信したgetblocksメッセージが送信新たメッセージの応答メッセージかチェックする
  void income_inv( inv::ref cmd ); // 応答が帰ってきたことを通知する
  
private: 
  const block_id _start_blkid;
  const block_id _stop_blkid;
 
  const on_done_notify_func _notify_func;
}; using getblocks_obs = class getblocks_observer;

class mempool_observer : public chain_sync_observer
{

}; using mempool_obs = class mempool_observer;


};


#endif 
