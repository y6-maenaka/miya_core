#include <ss_p2p/kademlia/rpc_manager.hpp>
#include <ss_p2p/kademlia/k_message.hpp>
#include <utils.hpp>


namespace ss
{
namespace kademlia
{


rpc_manager::rpc_manager( node_id &self_id, io_context &io_ctx, k_observer_strage &obs_strage, sender &sender, s_send_func &send_func ) :
  _self_id( self_id )
  , _io_ctx( io_ctx )
  , _tick_timer( io_ctx )
  , _obs_strage( obs_strage )
  , _sender( sender )
  , _s_send_func( send_func )
  , _routing_table( self_id )
  , _d_routing_table_controller( _routing_table )
{
  return;
}

void rpc_manager::ping_request( ip::udp::endpoint ep, on_pong_handler pong_handler, on_ping_timeout_handler timeout_handler )
{
  observer<ping> ping_obs( _io_ctx, ep, pong_handler, timeout_handler );
  ping_obs.init();
  _obs_strage.add_observer( ping_obs ); // 送信より先に保存しておく

  k_message k_msg = k_message::_request_( k_message::rpc::ping );
  k_msg.set_observer_id( ping_obs.get_id() );

  _s_send_func( ep, "kademlia", k_msg.encode() );
  #if SS_VERBOSE
  std::cout << "\x1b[33m" << "[rpc_manager](ping_request) send -> " << "\x1b[39m" << ep << "\n";
  #endif
}

void rpc_manager::find_node_request( ip::udp::endpoint ep, std::vector<ip::udp::endpoint> ignore_eps, on_find_node_response_handler response_handler )
{
  observer<find_node> find_node_obs( _io_ctx, response_handler );
  find_node_obs.init();
  _obs_strage.add_observer( find_node_obs ); // 送信するより先に保存しておく

  k_message k_msg = k_message::_request_( k_message::rpc::find_node );
  k_msg.set_observer_id( find_node_obs.get_id() );

  auto msg_controller = k_msg.get_find_node_message_controller();
  msg_controller.set_ignore_endpoint( ignore_eps );

  _s_send_func( ep, "kademlia", k_msg.encode() );

  #if SS_VERBOSE
  std::cout << "\x1b[33m" << "[rpc_manager](find_node_request) send -> " << "\x1b[39m" << ep << "\n";
  #endif

}

void rpc_manager::ping_response( k_message &k_msg, ip::udp::endpoint &ep )
{
  k_msg.set_message_type(k_message::message_type::response); // レスポンスに変更

  _sender.async_send( ep, "kademlia", k_msg.encode() // response系は普通に送信する
	  , std::bind( &rpc_manager::on_send_done, this, std::placeholders::_1 )
	);

  // 一旦送信の成功可否にかかわらずping元ノードをルーティングテーブルに追加する
  std::vector<ip::udp::endpoint> eps;  eps.push_back( ep );
  _d_routing_table_controller.auto_update_batch( eps );

  #if SS_VERBOSE
  std::cout << "[rpc_manager](ping_response) send -> " << ep << "\n";
  #endif
}

void rpc_manager::find_node_response( k_message &k_msg, ip::udp::endpoint &ep )
{
  auto msg_controller = k_msg.get_find_node_message_controller();
  auto ignore_eps = msg_controller.get_ignore_endpoint();
  ignore_eps.push_back( ep ); // リクエスト元も検索対象外とする

  auto eps = _d_routing_table_controller.collect_endpoint( ep, DEFAULT_FIND_NODE_SIZE, ignore_eps );
  msg_controller.set_finded_endpoint( eps ); // 自身のルーティングテーブルからみつかった ノードを格納する

  k_msg.set_message_type(k_message::message_type::response); // request -> responseI

  _sender.async_send( ep, "kademlia", k_msg.encode() // response系は普通に送信する
	  , std::bind( &rpc_manager::on_send_done, this, std::placeholders::_1 )
	  );

  #if SS_VERBOSE
  std::cout << "[rpc_manager](find_node_response) send -> " << ep << "\n";
  #endif
}

void rpc_manager::null_handler( ip::udp::endpoint ep )
{
  return ;
}

int rpc_manager::income_request( k_message &k_msg, ip::udp::endpoint &ep )
{
  const auto rpc = k_msg.get_rpc();

  switch( rpc )
  {
	case k_message::rpc::ping :
	  {
		this->ping_response( k_msg, ep );
		break;
	  }
	case k_message::rpc::find_node :
	  {
		this->find_node_response( k_msg, ep );
		break;
	  }
	default :
	  {
		break; // 不正なk_msg
	  }
  }

  return 0;
}

int rpc_manager::income_response( k_message &k_msg, ip::udp::endpoint &ep )
{
  // 基本的にobserverで捌かれる
  return 0;
}

void rpc_manager::on_send_done( const boost::system::error_code &ec )
{
  #if SS_VERBOSE
  if( !ec ); //std::cout << "[rpc_manager](send_done)" << "\n";
  else std::cout << "(rpc_manager)" << "\x1b[31m" << " send failure" << "\x1b[39m" << "\n";
  #endif
}

int rpc_manager::income_message( std::shared_ptr<message> msg, ip::udp::endpoint &ep )
{
  if( auto k_param = msg->get_param("kademlia"); k_param == nullptr ) return 0;
  k_message k_msg( *(msg->get_param("kademlia")) );

  auto message_type = k_msg.get_message_type();

  switch( message_type )
  {
	case k_message::message_type::request :
	  {
		return this->income_request( k_msg, ep );
	  }
	case k_message::message_type::response :
	  {
		return this->income_response( k_msg, ep );
	  }
	default :
	  {
		return 0;
	  }
  }
  return 0;
}

k_routing_table &rpc_manager::get_routing_table()
{
  return _routing_table;
}

direct_routing_table_controller &rpc_manager::get_direct_routing_table_controller()
{
  return _d_routing_table_controller;
}

void rpc_manager::update_self_id( node_id &id )
{
  _self_id = id;
}


};
};
