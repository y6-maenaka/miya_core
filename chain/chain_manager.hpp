#ifndef C78031F5_9D5E_477D_955D_24CC8E7510C4
#define C78031F5_9D5E_477D_955D_24CC8E7510C4


#include "boost/asio.hpp"

#include <core/core_context.hpp>
#include <node_gateway/message/command/block.hpp>
#include <node_gateway/message/command/headers.hpp>
// #include <chain/chain.hpp>
#include <chain/miya_coin/local_strage_manager.h>


using namespace boost::asio;


namespace chain
{


class chain_manager 
{
public:
  void income_block( struct MiyaCoreMSG_BLOCK command ); 
  void income_headers( struct MiyaCoreMSG_HEADERS command );

  chain_manager( io_context &io_ctx, class core_context &core_ctx, class BlockLocalStrage &block_strage, std::string path_to_chainstate_sr, std::function<void(void)> on_chain_update );

  // void on_chain_extended(); // chain_manager内部処理で最新ブロックがチェーンに繋がれ,ローカルのチェーンが伸びたときに呼び出されるハンドラ
  // class local_chain &get_local_chain();

private:
  class BlockLocalStrage &_block_strage;
  // class local_chain _local_chain;
 
  io_context &_io_ctx;
  deadline_timer _refresh_timer;
};


};


#endif 