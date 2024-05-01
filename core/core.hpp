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
#include <ss_p2p/message_pool.hpp>
#include <chain/chain_manager.h>
#include <chain/chain.hpp>
#include <crypto_utils/w_sha/sha.hpp>
#include "../share/json.hpp"
#include "boost/asio.hpp"

#include <ss_p2p_node_controller/include/ss_p2p/message_pool.hpp>


using json = nlohmann::json;
using namespace boost::asio;


namespace core
{


struct core_context
{
private:
    std::string _ecdsa_pem_pass;

    struct mining_info
    {
        std::uint32_t _version;
        struct coinbase
        {
            std::size_t _mining_reward;
            std::string _mining_text; // conibaseに埋め込むテキスト
        } _coinbase;
    };

public:
    core_context( std::string path_to_ecdsa_pem );
    std::string get_ecdsa_pem_pass();
    bool load_ecdsa_key( std::string path_to_ecdsa_pem );
};

// MiyaCore全体統括モジュール
class core // core class of miya_core
{
  std::shared_ptr<io_context> _io_ctx;
  core_context _context;

public:
  class broker // 受信メッセージを主要管理マネージャーに渡す
  {
	public:
	  void on_income_message( ss::message_pool::_message_ msg );
	private:
	  friend class core;
	  void income_inv();
	  void income_block();
	  void income_get_blocks();

	  broker( class core &p_core ) : _core(p_core){};
	  class core &_core;
  } _broker;
  // class chain::chain_manager _chain_manager;

public:
  core( std::shared_ptr<io_context> io_ctx = std::make_shared<io_context>() );
  core_context &get_context();

  friend class broker;
};


};


#endif // F720D5DF_D04A_420E_950E_112986BF5BF9
