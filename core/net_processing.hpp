#ifndef F0EC4DF8_A94D_4877_A8B011389774B51D
#define F0EC4DF8_A94D_4877_A8B011389774B51D


#include <ss_p2p_node_controller/include/ss_p2p/node_controller.hpp>
#include <string>


namespace chain
{
  class chain_manager;
  class mempool;
};


namespace core
{

/*
  他Peerから受信したメッセージを適切な処理系に流す
*/

class net_processing
{

public:
  struct options
  {
	const bool ignore_incoming_txs{false}; // 軽量ノード用
  };


public:
  net_processing( chain::chain_manager &chain_manager, chain::mempool &mempool, ss::message_pool &message_pool );
  // message_hubがイベント稼働であるため、net_processingもイベント稼働型になる
  void process_message( ss::peer::ref peer, ss::ss_message::ref income_msg ); // 恐らく変更になる
  void start();

protected:
  void process_command_getheaders();
  void process_command_getdata();

private:
  chain::chain_manager &_chain_manager;
  chain::mempool &_mempool;
  ss::message_pool &_message_pool;
  ss::message_pool::message_hub *_message_hub; // 複数のPeerを管理する統一されたインターフェース(message_poolから取り出す)

};


}; // namespace core


#endif 


// reference: https://github.com/bitcoin/bitcoin/blob/master/src/net_processing.h
