#ifndef AF617A30_B298_41BE_8904_BC1CE6F1D871
#define AF617A30_B298_41BE_8904_BC1CE6F1D871


#include <chain/chain_sync_observer.hpp>
#include <chain/block/block.params.hpp>
#include <ss_p2p/observer_strage.hpp>
#include <ss_p2p/peer.hpp>
#include <node_gateway/message/command/inv/inv.hpp>

#include "boost/asio.hpp"
#include "boost/thread/recursive_mutex.hpp"


using namespace boost::asio;


namespace chain
{


class block;


class chain_sync_manager 
{
  friend class getdata_observer;

public:
  using ref = std::shared_ptr<class chain_sync_manager>;

  chain_sync_manager( io_context &io_ctx );
  void income_command( ss::peer::ref peer, miya_core_command::ref cmd );
  void income_invs( ss::peer::ref peer, inv::ref invs_ref );

  const bool find_block( const BLOCK_ID &block_id ) const; // 現在構築している同期用のチェーンの中から該当のブロックを検索する
  const bool init( ss::peer::ref peer_ref, const BLOCK_ID &block_id );

private:
  io_context &_io_ctx;
  mutable boost::recursive_mutex _rmtx;
  std::vector< std::shared_ptr<class block> > _candidate_chain __attribute__((guarded_by(_rmtx))); // 同期中の仮想チェーン
  
  using chain_sync_observer_strage = ss::observer_strage< getdata_observer, block_observer, get_header_observer >;
  chain_sync_observer_strage _chain_sync_obs_strage __attribute__((guarded_by(_rmtx)));
};


};

#endif 
