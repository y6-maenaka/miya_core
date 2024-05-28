#include <ss_p2p/kademlia/dht_manager.hpp>


namespace ss
{
namespace kademlia
{


dht_manager::dht_manager( boost::asio::io_context &io_ctx, ip::udp::endpoint &ep, sender &sender, ss_logger *logger ) :
  _io_ctx( io_ctx )
  , _self_ep( ep )
  , _tick_timer( io_ctx )
  , _self_id( calc_node_id(ep) )
  , _obs_strage( io_ctx )
  , _sender( sender )
  , _rpc_manager( _self_id, _io_ctx, _obs_strage, sender, _s_send_func, logger )
  , _maintainer( this, _rpc_manager, io_ctx, logger )
  , _logger(logger)
{
  return;
}

int dht_manager::income_message( std::shared_ptr<message> msg, ip::udp::endpoint &ep )
{
  if( auto k_param = msg->get_param("kademlia"); k_param == nullptr ) return 0;
  k_message k_msg( *(msg->get_param("kademlia")) );

  auto call_observer_income_message = [&]( auto &obs )
  {
	return obs.income_message( *msg, ep );
  };

  const auto rpc = k_msg.get_rpc();
  observer_id obs_id = k_msg.get_observer_id();

  switch( rpc )
  {
	case k_message::rpc::ping :
	  {
		if( std::optional<observer<ping>> obs = _obs_strage.find_observer<ping>(obs_id); obs != std::nullopt ){
		  // std::cout << "\x1b[33m" << "<k observer strage> ping found" << "\n" << "\x1b[39m";
		  return call_observer_income_message(*obs); // relay_observerの検索
		}
		break;
	  }
	case k_message::rpc::find_node :
	  {
		if( std::optional<observer<find_node>> obs = _obs_strage.find_observer<find_node>(obs_id); obs != std::nullopt ){
		  // std::cout << "\x1b[33m" << "[dht_manager](k observer strage) find_node found" << "\n" << "\x1b[39m";
		  return call_observer_income_message(*obs); // relay_observerの検索
		}
		break;
	  }
	default :
	  {
		// std::cout << "\x1b[31m" << "[dht_manager](k observer strage) not found" << "\x1b[39m" << "\n";
	  }
  }

  _rpc_manager.income_message( msg, ep );

  return 0;
}

k_routing_table &dht_manager::get_routing_table()
{
  return _rpc_manager.get_routing_table();
}

direct_routing_table_controller &dht_manager::get_direct_routing_table_controller()
{
  return _rpc_manager.get_direct_routing_table_controller();
}

rpc_manager &dht_manager::get_rpc_manager()
{
  return _rpc_manager;
}

void dht_manager::update_global_self_endpoint( ip::udp::endpoint &ep )
{
  ip::udp::endpoint __prev_self_ep = _self_ep;
  node_id __prev_self_id = _self_id;

  _self_ep = ep;
  _self_id = calc_node_id(ep); // 自身のidの更新

  #ifndef SS_LOGGING_DISABLE
  _logger->log( logger::log_level::INFO, "(@dht_manager)", "update global self endpoint"
	  , endpoint_to_str(__prev_self_ep).c_str(), " -> ", endpoint_to_str(_self_ep).c_str() );
  #endif

  _rpc_manager.update_self_id( _self_id ); // 配下rpc_managerの更新
}

void dht_manager::start()
{
  _maintainer._requires_tick = true;
  _maintainer.tick();
}

void dht_manager::stop()
{
  _maintainer._requires_tick = false;
}

void dht_manager::init( s_send_func s_send_func )
{
  _s_send_func = s_send_func;
}

#if SS_DEBUG
k_observer_strage &dht_manager::get_observer_strage()
{
  return _obs_strage;
}
#endif


}; // namespace kademlia
}; // namespace ss
