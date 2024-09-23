#include <chain/chain_sync_manager.hpp>
#include <chain/block/block.hpp>
#include <ss_p2p/observer.hpp>


namespace chain
{


chain_sync_manager::chain_sync_manager( io_context &io_ctx, block_iterator& forkpoint_itr, chain_sync_manager::on_sync_done_callback notify_func ) :
  _io_ctx( io_ctx )
  , _obs_strage( io_ctx )
  , _forkpoint( forkpoint_itr )
  , _notify_func( notify_func )
{
  return;
}

const bool chain_sync_manager::find_block( const block_id &block_id ) const
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

void chain_sync_manager::on_getblocks_done( getblocks_observer::obs_ctx ctx )
{
	/* -- 条件 --
	1. getblocks_observerが取得してきたblock_id_vectorのチェック(2個以上, 1個目はchain_frameの分岐点)
	2. chain_frameの未ダウンロード部先頭を探す
	3. getblocks_observerが取得してきたblock_id_vectorの先頭とchain_frameの先頭が合致しているかチェック
	*/
	if( ctx.status == getblocks_observer::obs_ctx::state::complete && !(ctx.received_block_id_v.empty()) )
	{
		if( auto itr = get_undownloaded_blkid_head(); itr != _chain_frame.end() )
		{
			if( itr->first == ctx.start_blkid )
			{
				std::vector< chain_sync_manager::chain_frame::value_type > insert_pairs;
				std::transform( ctx.received_block_id_v.begin(), ctx.received_block_id_v.end()
				, std::back_inserter(insert_pairs), [](const block_id &id){
					return std::make_pair(id, false);
				});
				_chain_frame.insert( itr + 1, insert_pairs.begin(), insert_pairs.end() );
			}
		}
	}
	else if( ctx.status == getblocks_observer::obs_ctx::state::notfound )
	{
		// 次のシーケンスにうつる?
		return;
	}
	else
	{
		return;
		// 再送 or 他のpeerへの送信
	}
}

bool chain_sync_manager::init( ss::peer::ref peer_ref, const block_id &block_id )
{
  /*
  ss::observer<getdata_observer> getdata_obs( _io_ctx, generate_chain_sync_observer_id<BLOCK_ID_BYTES_LENGTH>( COMMAND_TYPE_ID::MSG_BLOCK, block_id ) ); // (type_id): block + (id): block_id からobserver_idを生成する
  getdata_obs.init( peer_ref, block_id );

  _obs_strage.add_observer( getdata_obs );

  return true;
  */
}

chain_sync_manager::chain_frame::iterator chain_sync_manager::get_undownloaded_blkid_head()
{
	for( auto itr = _chain_frame.begin(); itr != _chain_frame.end(); itr++ ) {
		if( itr->second == true ) return (itr == _chain_frame.begin()) ? _chain_frame.end() : std::prev(itr);
	}
	return _chain_frame.end();
}

void chain_sync_manager::income_command_inv( ss::peer::ref peer, inv::ref cmd )
{	
	/*
	1. 受信元のpeerをKeyにcmdと対応するobserver(getblocks or mempool)を取り出す
	2. 1.で取り出したobserverに対してincome通知をする
	*/

	// getblocksの取り出し
	if( auto found_observers = _obs_strage.find_observer<getblocks_observer, by_peer_id>( peer->get_id() ); found_observers.size() == 0 )
	{
		for( auto obs_itr : found_observers )
		{
			// 該当するobserverを取り出す
			if( bool is_correspond = obs_itr->get()->is_correspond_inv( cmd ); !is_correspond ) continue;

			obs_itr->get()->income_inv( cmd );
			 _obs_strage.delete_observer<getblocks_observer, ss::by_observer_id/*一応*/>(obs_itr->get_id());
		}
		return;
	}
	else
	{

	}

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

void serial_chain_sync_manager::async_start( std::pair<block::ref, ss::peer::ref> target )
{
	ss::observer<getblocks_observer>::ref getblocks_obs = std::make_shared<ss::observer<getblocks_observer>>( _io_ctx, target.second
	, _forkpoint.get_id(), target.first->get_id(), std::bind( &chain_sync_manager::on_getblocks_done, this, std::placeholders::_1) );

	if( !is_forkpoint_validated ) _notify_func( sync_result::status::WRONG_FORKPOINT );

	_obs_strage.add_observer<class getblocks_observer>( getblocks_obs ); // リクエストを送信する前にobserver_strageに追加する
	getblocks_obs->get()->request();  // メッセージコマンドを送信
	return;
}

void serial_chain_sync_manager::start_block_download_sequence()
{
	return;
}


};
