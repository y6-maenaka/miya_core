#include <chain/chain_sync_manager.hpp>
#include <ss_p2p/observer.hpp>


namespace chain
{


chain_sync_manager::chain_sync_manager( io_context &io_ctx ) : 
  _io_ctx( io_ctx )
  , _chain_sync_obs_strage( io_ctx )
{
  return;
}

const bool chain_sync_manager::init( ss::peer::ref peer_ref, const BLOCK_ID &block_id )
{
  ss::observer<getdata_observer> getdata_obs( _io_ctx );
  getdata_obs.init( peer_ref, block_id );

  _chain_sync_obs_strage.add_observer( getdata_obs );
  
  return false;
}


};
