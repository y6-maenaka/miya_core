#include <ss_p2p/ice_agent/stun_server.hpp>
#include <ss_p2p/ice_agent/ice_observer.hpp>
#include <ss_p2p/ice_agent/ice_observer_strage.hpp>
#include <ss_p2p/observer.hpp>
#include <ss_p2p/ice_agent/ice_sender.hpp>

#include <algorithm>


namespace ss
{
namespace ice
{


stun_server::stun_server( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ss::kademlia::direct_routing_table_controller &d_routing_table_controller, ice_observer_strage &obs_strage, ss_logger *logger ) :
  _io_ctx( io_ctx )
  , _sender( sender )
  , _ice_sender( ice_sender )
  , _d_routing_table_controller( d_routing_table_controller )
  , _obs_strage( obs_strage )
  , _logger( logger )
{
  return;
}

void stun_server::on_send_done( const boost::system::error_code &ec, std::size_t bytes_transferred )
{
  #if SS_VERBOSE
  if( !ec );// std::cout << "(stun server) send done -> " << bytes_transferred << " [bytes]" << "\n";
  else ; // std::cout << "(stun_server)" << "\x1b[31m" << " send failure" << "\x1b[39m" << "\n";
  #endif
}

[[nodiscard]] stun_server::sr_ptr stun_server::binding_request( std::vector<ip::udp::endpoint> target_nodes , unsigned short reliability )
{
  std::vector<ip::udp::endpoint> request_nodes = std::vector<ip::udp::endpoint>();
  request_nodes.reserve( target_nodes.size() );
  std::copy( target_nodes.begin(), target_nodes.end(), std::back_inserter(request_nodes) );

  std::size_t request_node_count = std::max( (std::size_t)1, request_nodes.size() );
  request_node_count = std::max( request_node_count, (std::size_t)(reliability/10) );

  if( request_node_count > request_nodes.size() ) // request先ノード数が不住ぶな場合
  {
	ip::udp::endpoint root_ep( ip::address::from_string("0.0.0.0") , 0 );
	auto collected_nodes = _d_routing_table_controller.collect_endpoint( root_ep, request_node_count - request_nodes.size(), request_nodes );
	for( auto itr : collected_nodes ) request_nodes.push_back( itr );
  }

  if( request_nodes.empty() ) return std::make_shared<stun_server::sr_object>(stun_server::sr_object::_error_());

  std::shared_ptr<sr_object> sr = std::make_shared<sr_object>(); // 予約オブジェクトの作成
  observer<class binding_request> binding_req_obs( _io_ctx, _sender, _ice_sender, _d_routing_table_controller ); // binding_request observerの作成
  binding_req_obs.init( sr ); // observerにsrを紐づける  (重要)

  ice_message ice_msg = ice_message::_stun_(); // stunメッセーを作成
  ice_msg.set_observer_id( binding_req_obs.get_id() ); // observer_idのセット
  auto msg_controller = ice_msg.get_stun_message_controller();
  msg_controller.set_sub_protocol( ice_message::stun::sub_protocol_t::binding_request ); // サブプロトコル(request)のセット

  _obs_strage.add_observer<class binding_request>(binding_req_obs); // 送信に先駆けてobserverを保存しておく

  for( auto itr : request_nodes )
  { // binding_requestを送信する
	_ice_sender.async_ice_send( itr, ice_msg, std::bind( &stun_server::on_send_done, this, std::placeholders::_1, std::placeholders::_2 ) );
	binding_req_obs.get_raw()->add_requested_ep( itr );
  }
  return sr;
}


int stun_server::income_message( std::shared_ptr<message> msg, ip::udp::endpoint &ep )
{
  ice_message ice_msg( *(msg->get_param("ice_agent")));
  auto stun_message_controller = ice_msg.get_stun_message_controller();

  if( stun_message_controller.get_sub_protocol() == ice_message::stun::sub_protocol_t::binding_request )
  { // observerは特に作成する必要はない
	auto msg_controller = ice_msg.get_stun_message_controller(); // リクエストメッセージをそのまま使う
	msg_controller.set_sub_protocol( ice_message::stun::sub_protocol_t::binding_response ); // メッセージをレスポンスへ
	msg_controller.set_global_ep(ep); // 送信元ノードのグローバル(ローカルの場合もある)アドレスをセット

	_ice_sender.async_ice_send( ep, ice_msg
		, std::bind( &stun_server::on_send_done, this, std::placeholders::_1, std::placeholders::_2 ) );

	#ifndef SS_LOGGING_DISABLE
	_logger->log_packet( logger::log_level::INFO, ss_logger::packet_direction::OUTGOING, ep, "(@stun_server)", "binding response" );
	#endif

	return 0;
  }
  return 0;
}


stun_server::sr_object::sr_object() :
  _is_async( false )
  , _state( stun_server::sr_object::state_t::notfound )
  , _mtx( std::make_shared<std::mutex>() )
  , _cv( std::make_shared<std::condition_variable>() )
{
  _global_ep = ip::udp::endpoint( ip::address::from_string("0.0.0.0"), 0 ); // 一応無効なアドレスとして
}

std::optional< ip::udp::endpoint> stun_server::sr_object::sync_get()
{
  std::unique_lock<std::mutex> lock(*_mtx);
  if( _state != stun_server::sr_object::state_t::pending ) // 待機状態以外(処理済み)だったら速攻返す
	_cv->wait( lock );

  if( _state == stun_server::sr_object::state_t::done ) return std::optional(_global_ep);
  else return std::nullopt;
}

void stun_server::sr_object::async_get( on_nat_traversal_success_handler handler )
{
  this->handler = handler;
  _is_async = true;
}

void stun_server::sr_object::update_state( state_t s, std::optional<ip::udp::endpoint> ep )
{
  if( s == stun_server::sr_object::state_t::done && ep != std::nullopt ){
	_state = stun_server::sr_object::state_t::done;
	_global_ep = *ep;
  }
  else{
	_state = stun_server::sr_object::state_t::notfound;
  }

  if( !(this->is_async()) ) _cv->notify_all(); // 成功でも失敗でも起こす
}

stun_server::sr_object::state_t stun_server::sr_object::get_state() const
{
  return _state;
}

stun_server::sr_object stun_server::sr_object::_error_()
{
  sr_object ret;
  ret._state = stun_server::sr_object::state_t::notfound;
  return ret;
}

stun_server::sr_object stun_server::sr_object::_pending_()
{
  sr_object ret;
  ret._state = stun_server::sr_object::state_t::pending;
  return ret;
}

bool stun_server::sr_object::is_async() const
{
  return _is_async;
}


};
};
