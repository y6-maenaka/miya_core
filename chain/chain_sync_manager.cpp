#include <chain/chain_sync_manager.hpp>
#include <chain/block/block.hpp>
#include <ss_p2p/observer.hpp>


namespace chain
{


chain_sync_manager::chain_sync_manager( io_context &io_ctx ) : 
  _io_ctx( io_ctx )
  , _chain_sync_obs_strage( io_ctx )
{
  return;
}

const bool chain_sync_manager::find_block( const BLOCK_ID &block_id ) const
{
  std::unique_lock<boost::recursive_mutex> lock(_rmtx);
  std::find_if( _candidate_chain.begin(), _candidate_chain.end(), [block_id](const std::pair<const BLOCK_ID, block::ref> &c_block_pair){
	return std::equal( block_id.begin(), block_id.end(), c_block_pair.first.begin() );
	  });
  return false;
}

void chain_sync_manager::income_notfound( ss::peer::ref peer, MiyaCoreMSG_NOTFOUND::ref ntf )
{
  /*
    逆引きするにはどうしたらいい
  */

  auto inv_itr = (ntf->get_inv()).begin(); // 該当するpeerとobserverをobserver_strageから検索する
 
  while( inv_itr == ntf->get_inv().end() )
  {
	if( inv_itr->first == inv::type_id::MSG_BLOCK )
	{
	  return;
	} 
	else if( inv_itr->first == inv::type_id:: MSG_TX )
	{
	  return;
	}
	else 
	{
	  return;
	}
  }

  return;
}

bool chain_sync_manager::init( ss::peer::ref peer_ref, const BLOCK_ID &block_id )
{
  ss::observer<getdata_observer> getdata_obs( _io_ctx, generate_sync_command_observer_id<BLOCK_ID_BYTES_LENGTH>( COMMAND_TYPE_ID::MSG_BLOCK, block_id ) );
  getdata_obs.init( peer_ref, block_id );

  _chain_sync_obs_strage.add_observer( getdata_obs );

  return true;
}


};
