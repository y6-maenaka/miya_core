#ifndef F720D5DF_D04A_420E_950E_112986BF5BF9
#define F720D5DF_D04A_420E_950E_112986BF5BF9


#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <span>
#include <optional>

#include <unistd.h>
#include "../share/json.hpp"
#include "boost/asio.hpp"

#include <ss_p2p/message_pool.hpp>
#include <ss_p2p_node_controller/include/ss_p2p/message_pool.hpp>
// #include <crypto_utils/w_sha/sha.hpp>
#include <chain/chain_manager.hpp>
#include <chain/transaction_pool/transaction_pool.h>
#include <chain/IBD.hpp>
#include <core/core_context.hpp>
// #include <chain/miya_coin/local_strage_manager.h>
#include <chain/utxo_set/utxo_set.h>


using json = nlohmann::json;
using namespace boost::asio;


namespace core
{


// MiyaCore全体統括モジュール
class core // core class of miya_core
{
  friend class message_broker;
public:
  core();

  core_context &get_context();
  void start(); // 本体プログラムの実行

protected:
  void on_update_chain();
  void on_success_mine();
  void on_failure_mine();

private:
  io_context io_ctx;
  core_context _context;
  // class chain::chain_manager _chain_manager;
  // class BlockLocalStrage _block_local_strage;
  // class chain::TransactionPool _transaction_pool;
  // class chain::LightUTXOSet _utxoSet;

  /* class message_broker // 受信メッセージを主要管理マネージャーに渡す(ss_p2pからの受信メッセージを流す)
  {
	public:
	  void on_income_message( ss::message_pool::_message_ msg );
	private:
	  friend class core;
	  void income_command_inv();
	  void income_command_block();
	  void income_command_get_blocks();

	  message_broker( class core &p_core ) : _core(p_core){};
	  class core &_core;
  } _message_broker; */
};


};


#endif // F720D5DF_D04A_420E_950E_112986BF5BF9
