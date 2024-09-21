#include <chain/chain_sync_manager.hpp>
#include <chain/block/block.hpp>
#include <ss_p2p/observer.hpp>


namespace chain
{


chain_sync_manager::chain_sync_manager( io_context &io_ctx, block_iterator &fork_point_itr ) : 
  _io_ctx( io_ctx )
  , _obs_strage( io_ctx )
  , _forkpoint( fork_point_itr )
{
  return;
}

const bool chain_sync_manager::find_block( const BLOCK_ID &block_id ) const
{
  /*
  std::unique_lock<boost::recursive_mutex> lock(_rmtx);
  std::find_if( _candidate_chain.begin(), _candidate_chain.end(), [block_id](const std::pair<const BLOCK_ID, block::ref> &c_block_pair){
	return std::equal( block_id.begin(), block_id.end(), c_block_pair.first.begin() );
	  });
  return false;
  */
}

const bool chain_sync_manager::find_forkpoint()
{
  return false;
}

void chain_sync_manager::register_command_getdata( MiyaCoreMSG_GETDATA &getdata_cmd )
{
  /*
	for( auto itr =  getdata_cmd.get_inv().begin(); itr != getdata_cmd.get_inv().end(); itr++ )
	{
		switch( itr.id )
		{
			case ( inv::type_id::MSG_BLOCK )
			{
				ss::observer<block_observer>::ref block_obs = std::make_shared<ss::observer<block_observer>>( itr.hash );
				block_obs.init();

				_obs_strage.add_observer<block_observer>( block_obs );
				break;
			}

			case ( inv::type_id::MSG_TX )
			{
				break;
			}
		}
	}
  */
}

void chain_sync_manager::request_prev_block( const block::id &block_id )
{
	// ss::observer<getdata_observer>::ref request_obs_ref = std::make_shared<ss::observer<getdata_observer>>( _io_ctx, generate_chain_sync_observer_id<BLOCK_ID_BYTES_LENGTH>( COMMAND_TYPE_ID::MSG_BLOCK, block_ref->get_prev_block_hash() )  );
	// request_obs_ref->init();
}

bool chain_sync_manager::init( ss::peer::ref peer_ref, const BLOCK_ID &block_id )
{
  ss::observer<getdata_observer> getdata_obs( _io_ctx, generate_chain_sync_observer_id<BLOCK_ID_BYTES_LENGTH>( COMMAND_TYPE_ID::MSG_BLOCK, block_id ) ); // (type_id): block + (id): block_id からobserver_idを生成する
  getdata_obs.init( peer_ref, block_id );

  _obs_strage.add_observer( getdata_obs );

  return true;
}

void chain_sync_manager::income_command_block( ss::peer::ref peer, MiyaCoreMSG_BLOCK::ref cmd )
	// 他peerが発掘したブロック本体が送信されてきた時
{
  /*
  block::id target_block_id = cmd->get_block()->get_id();
  ss::observer<getdata_observer>::ref target_obs = _obs_strage.find_observer<getdata_observer>( generate_chain_sync_observer_id<block::id_length>( COMMAND_TYPE_ID::MSG_BLOCK, target_block_id ) );
  
  if( target_obs != nullptr ) target_obs->get()->on_receive( peer, cmd->get_block() );
  */
}

void chain_sync_manager::income_command_notfound( ss::peer::ref peer, MiyaCoreMSG_NOTFOUND::ref cmd )
{
  /*
  auto inv_itr = (cmd->get_inv()).begin(); // 該当するpeerとobserverをobserver_strageから検索する
 
  while( inv_itr == cmd->get_inv().end() )
  {
	const auto ntf_type_id = inv_itr->first;
	const auto ntf_id = inv_itr->second->_hash;

	switch( ntf_type_id )
	{
	  case inv::type_id::MSG_TX : 
	  {
		ss::observer<getdata_observer>::ref target_obs = _obs_strage.find_observer<getdata_observer>( generate_chain_sync_observer_id<block::id_length>( COMMAND_TYPE_ID::MSG_TX, ntf_id) );
		target_obs->get_raw()->on_notfound( peer );
		break;
	  }

	  case inv::type_id::MSG_BLOCK :
	  {

		if( ntf_id == _start_block_id ) // 集取スタート点と一致した場合は開始点を下げる
		{
		}

		ss::observer<getdata_observer>::ref target_obs = _obs_strage.find_observer<getdata_observer>( generate_chain_sync_observer_id<block::id_length>( COMMAND_TYPE_ID::MSG_BLOCK, ntf_id) );
		break;
	  }
	}

  }

  return;
  */
}

void serial_chain_sync_manager::async_start( std::function<void(sync_result)> ret_callback_func )
{
	// ss::observer<getdata_observer>::ref getdata_obs = std::make_shared<ss::observer<getdata_observer>>( _target_block_id );
}


};
