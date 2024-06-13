#ifndef AF617A30_B298_41BE_8904_BC1CE6F1D871
#define AF617A30_B298_41BE_8904_BC1CE6F1D871


#include <chain/chain_sync_observer.hpp>
#include <ss_p2p/observer_strage.hpp>
#include <ss_p2p/peer.hpp>
#include <node_gateway/message/command/inv/inv.hpp>

#include "boost/align.hpp"


using namespace boost::asio;


namespace chain
{


class chain_sync_manager 
{
  friend class getdata_observer;

public:
  chain_sync_manager();
  void income_command( ss::peer::ref peer, miya_core_command::ref cmd );
  void income_inv( ss::peer::ref peer, inv::ref inv_ref );

protected:
  // void シーケンス続きのobserverを発行する();

private:
  using chain_sync_observer_strage = ss::observer_strage< getdata_observer, block_observer, get_header_observer >;
  chain_sync_observer_strage _chain_sync_obs_strage;

public:
};


};

#endif 
