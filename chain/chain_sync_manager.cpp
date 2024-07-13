#include <chain/chain_sync_manager.hpp>
#include <chain/block/block.hpp>
#include <ss_p2p/observer.hpp>


namespace chain
{


chain_sync_manager::chain_sync_manager( io_context &io_ctx ) : 
  _io_ctx( io_ctx )
  , _obs_strage( io_ctx )
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

bool chain_sync_manager::init( ss::peer::ref peer_ref, const BLOCK_ID &block_id )
{
  ss::observer<getdata_observer> getdata_obs( _io_ctx, generate_chain_sync_observer_id<BLOCK_ID_BYTES_LENGTH>( COMMAND_TYPE_ID::MSG_BLOCK, block_id ) ); // (type_id): block + (id): block_id からobserver_idを生成する
  getdata_obs.init( peer_ref, block_id );

  _obs_strage.add_observer( getdata_obs );

  return true;
}

void chain_sync_manager::income_command_block( ss::peer::ref peer, MiyaCoreMSG_BLOCK::ref cmd )
{
  return;
}

void chain_sync_manager::income_command_notfound( ss::peer::ref peer, MiyaCoreMSG_NOTFOUND::ref cmd )
{
  /*
    逆引きするにはどうしたらいい
  */

  auto inv_itr = (cmd->get_inv()).begin(); // 該当するpeerとobserverをobserver_strageから検索する
 
  while( inv_itr == cmd->get_inv().end() )
  {
	const auto ntf_type_id = inv_itr->first;
	const auto ntf_id = inv_itr->second->_hash;

	switch( ntf_type_id )
	{
	  case inv::type_id::MSG_TX : 
	  {
		_obs_strage.find_observer<getdata_observer>( generate_chain_sync_observer_id<block::id_length>( COMMAND_TYPE_ID::MSG_TX, ntf_id) );
		break;
	  }

	  case inv::type_id::MSG_BLOCK :
	  {
		_obs_strage.find_observer<getdata_observer>( generate_chain_sync_observer_id<block::id_length>( COMMAND_TYPE_ID::MSG_BLOCK, ntf_id) );
		break;
	  }
	}

  }

  return;
}


};
