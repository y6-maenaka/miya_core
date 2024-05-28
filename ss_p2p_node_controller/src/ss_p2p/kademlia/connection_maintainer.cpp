#include <ss_p2p/kademlia/dht_manager.hpp>


namespace ss
{
namespace kademlia
{


dht_manager::connection_maintainer::connection_maintainer( class dht_manager *manager, class rpc_manager &rpc_manager, io_context &io_ctx, ss_logger *logger ) :
  _d_manager( manager )
  // , _io_ctx( io_ctx )
  , _rpc_manager( rpc_manager )
  , _io_ctx( io_ctx )
  , _timer( io_ctx )
  , _requires_tick( false )
  , _logger( logger )
{
  return;
}

void dht_manager::connection_maintainer::tick()
{
  if( !(_requires_tick) ) return;

  std::time_t refresh_ping_done_time = this->send_refresh_ping(); // 生存しているノード数(実際の)をカウントする為

  _timer.expires_from_now( boost::posix_time::seconds(refresh_ping_done_time + 1) );
  _timer.async_wait( std::bind( &dht_manager::connection_maintainer::get_remote_nodes, this ) );
}

void dht_manager::connection_maintainer::tick_done()
{
  auto &d_table_controller = _d_manager->get_direct_routing_table_controller();
  std::size_t node_count = d_table_controller.get_node_count();
  std::time_t next_tick_time_s = ( node_count < MINIMUM_NODES ) ? 10 : node_count * 20 ; /* 実装段階では雑に決定 */

  if( !(_requires_tick) ) return; // tickがstopされていたらそれ以上は繰り返さない
  // d_table_controller.print( 155 );

  _timer.expires_from_now( boost::posix_time::seconds( next_tick_time_s/*適当*/) );
  _timer.async_wait( std::bind( &dht_manager::connection_maintainer::tick, this ) );
}

void dht_manager::connection_maintainer::get_remote_nodes()
{
  auto &d_table_controller = _d_manager->get_direct_routing_table_controller();
  std::size_t node_count = d_table_controller.get_node_count();

  if( node_count < MINIMUM_NODES )
  {
	ip::udp::endpoint root_ep( ip::address::from_string("0.0.0.0"), 0 );
	auto request_eps = d_table_controller.collect_endpoint( root_ep, 5/*適当*/ );
	auto &d_routing_table_controller = _d_manager->get_direct_routing_table_controller();

	for( auto itr : request_eps )
	{
	  _rpc_manager.find_node_request( itr, request_eps
		  , [&](ip::udp::endpoint ep) // find_node_responseに対するハンドラ
		  {
			_rpc_manager.ping_request( ep
				, std::bind( &direct_routing_table_controller::auto_update, d_routing_table_controller, std::placeholders::_1 ) // pingが帰ってきたらルーティングテーブルに追加
				, std::bind( &rpc_manager::null_handler, std::ref(_d_manager->_rpc_manager), std::placeholders::_1 )
			  );
		  });
	}
  }
  this->tick_done();
}

void dht_manager::connection_maintainer::no_handle( ip::udp::endpoint ep )
{
  // dummy
  return;
}

std::time_t dht_manager::connection_maintainer::send_refresh_ping()
{
  // 全てのノードを収集する
  auto &d_table_controller = _d_manager->get_direct_routing_table_controller();
  for( auto bucket_itr = d_table_controller.get_begin_bucket_iterator(); !(bucket_itr.is_invalid()); bucket_itr++ )
  {
	auto bucket_nodes = bucket_itr.get_nodes();
	for( auto itr : bucket_nodes ) // 全てのノードにpingを送信する(timeoutでdelete)
	{
	  _rpc_manager.ping_request
		( itr.get_endpoint()  // timeout->deleteのpingを送信する
		  , std::bind( &k_bucket::move_back, std::ref(bucket_itr.get_raw()), itr )
		  , std::bind( &k_bucket::delete_node, std::ref(bucket_itr.get_raw()), itr )
		);

	  #ifndef SS_LOGGING_DISABLE
	  _logger->log_packet( logger::log_level::INFO, ss_logger::packet_direction::OUTGOING, itr.get_endpoint(), "(@dht_manager::connection_maintainer)", "refrech ping" );
	  #endif
	}
  }

  return DEFAULT_PING_RESPONSE_TIMEOUT_s;
}


};
};
