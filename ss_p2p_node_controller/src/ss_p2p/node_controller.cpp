#include <ss_p2p/node_controller.hpp>


namespace ss
{


node_controller::node_controller( ip::udp::endpoint &self_ep, std::shared_ptr<io_context> io_ctx ) :
   _self_ep(self_ep)
  , _core_io_ctx( io_ctx )
  , _u_sock_manager( self_ep, std::ref(*io_ctx) )
  , _tick_timer( *io_ctx )
  , _msg_pool( *io_ctx, true )
  , _sender( _u_sock_manager, _id )
{
  ip::udp::endpoint init_ep( ip::address::from_string("0.0.0.0"), 0 ); // 一旦適当な初期値で開始する
  _glob_self_ep = init_ep;

  _udp_server = std::make_shared<udp_server>( _u_sock_manager, *_core_io_ctx
	  , std::bind( &node_controller::on_receive_packet
		, this
		, std::placeholders::_1
		, std::placeholders::_2)
	  );
  _dht_manager = std::make_shared<dht_manager>( *io_ctx, _self_ep, _sender );
  _ice_agent = std::make_shared<ice::ice_agent>( *_core_io_ctx, _u_sock_manager, self_ep/*一旦*/, _id, _sender
	  , _dht_manager->get_direct_routing_table_controller() );


  /* 必ず初期化する */
  _dht_manager->init( _ice_agent->get_signaling_send_func() );
}

udp_socket_manager &node_controller::get_socket_manager()
{
  return _u_sock_manager;
}

#if SS_DEBUG
kademlia::k_routing_table &node_controller::get_routing_table()
{
  return _dht_manager->get_routing_table();
}

ice::ice_agent &node_controller::get_ice_agent()
{
  return *_ice_agent;
}

kademlia::dht_manager &node_controller::get_dht_manager()
{
  return *_dht_manager;
}

message_pool &node_controller::get_message_pool()
{
  return _msg_pool;
}
#endif

std::optional< ip::udp::endpoint > node_controller::sync_get_global_address( std::vector<ip::udp::endpoint> boot_eps )
{
  auto &stun_server = _ice_agent->get_stun_server();
  auto sr_obj = stun_server.binding_request( boot_eps );

  return sr_obj->sync_get();
}

kademlia::direct_routing_table_controller &node_controller::get_direct_routing_table_controller()
{
  return _dht_manager->get_direct_routing_table_controller();
}

message_pool::message_hub &node_controller::get_message_hub()
{
  return _msg_pool.get_message_hub();
}

void node_controller::update_global_self_endpoint( ip::udp::endpoint ep )
{
  ip::udp::endpoint __prev_global_self_ep = _glob_self_ep;

  _glob_self_ep = ep;

  #if SS_VERBOSE
  std::cout << "\x1b[33m" << "[node_controller](update global self endpoint)" << "\n" << "\x1b[39m";
  std::cout << __prev_global_self_ep << " -> " << _glob_self_ep << "\n";
  std::cout << "\n";
  #endif

  _ice_agent->update_global_self_endpoint( ep );
  _dht_manager->update_global_self_endpoint( ep );
}

void node_controller::start( std::vector<ip::udp::endpoint> boot_eps )
{
  std::thread daemon([&, boot_eps ]()
	{
	  assert( _udp_server != nullptr );
	  _udp_server->start();
	  _dht_manager->start();

	  auto sr_obj = _ice_agent->get_stun_server().binding_request( boot_eps );
	  sr_obj->async_get( std::bind( [this]( std::optional<ip::udp::endpoint> ep ){
			  if( ep == std::nullopt ) return;
			  this->update_global_self_endpoint( *ep );
			}, std::placeholders::_1 )
		  );

	  _msg_pool.requires_refresh(true); // msg_poolの定期リフレッシュを停止する
	  this->call_tick();

	  _core_io_ctx->run();
	});
  daemon.detach();

  #if SS_VERBOSE
  std::cout << "[\x1b[32m start \x1b[39m] node controller" << "\n";
  std::cout << "(self endpoint) : " << _self_ep << "\n";
  #endif
}

void node_controller::stop()
{
  _udp_server->stop();
  _core_io_ctx->stop();
  _msg_pool.requires_refresh(false); // msg_poolの定期リフレッシュを再開する
  _dht_manager->stop();

  #if SS_VERBOSE
  std::cout << "[\x1b[31m stop \x1b[39m] node controller" << "\n";
  #endif
}

void node_controller::tick()
{
  #if SS_VERBOSE
  std::cout << "\x1b[31m" << "(node controller) グローバルアドレスの再取得" << "\x1b[39m" << "\n";
  #endif
  // グローバルアドレスの再取得
  auto sr_obj = _ice_agent->get_stun_server().binding_request();
	  sr_obj->async_get( std::bind( [this]( std::optional<ip::udp::endpoint> ep ){
			  if( ep == std::nullopt ) return;
			  this->update_global_self_endpoint( *ep );
			}, std::placeholders::_1 )
		  );

  this->call_tick();
}

void node_controller::call_tick( std::time_t tick_time_s )
{
  _tick_timer.expires_from_now( boost::posix_time::seconds( tick_time_s ) );
  _tick_timer.async_wait( std::bind( &node_controller::tick, this) );
}

peer node_controller::get_peer( ip::udp::endpoint &ep )
{
  peer ret( ep, _msg_pool.get_symbolic(ep), _ice_agent->get_signaling_send_func() );
  return ret;
}

void node_controller::on_receive_packet( std::vector<std::uint8_t> raw_msg, ip::udp::endpoint &ep )
{
  int flag = 1;
  std::shared_ptr<message> msg = std::make_shared<message>( message::decode(raw_msg) );
  if( msg == nullptr ) return;

  #if SS_CAPTURE_PACKET
  /*
  for( int i=0; i<get_console_width(); i++ ) printf("*");
  std::cout << "\n";
  std::cout << "(recv) from : " << ep << "\n";
  msg->print();
  for( int i=0; i<get_console_width(); i++ ) printf("*");
  */
  #endif

  if( msg->is_contain_param("kademlia") )
  {
	int state = _dht_manager->income_message( msg , ep );
  }

  if( msg->is_contain_param("ice_agent") )
  {
	flag = _ice_agent->income_message( msg, ep );
  }

  if( flag == 1 ){ // 現時点では全ての流入してくるメッセージを保存するようにする
	_msg_pool.store( msg, ep ); // メッセージの保存
  }
}


};
