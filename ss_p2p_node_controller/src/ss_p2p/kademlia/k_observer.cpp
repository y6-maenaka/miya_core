#include <ss_p2p/kademlia/k_observer.hpp>


namespace ss
{
namespace kademlia
{


k_observer::k_observer( io_context &io_ctx )
  : base_observer( io_ctx )
{
  return;
}


ping::ping( io_context &io_ctx, ip::udp::endpoint ep, on_pong_handler pong_handler, on_timeout_handler timeout_handler ) :
  k_observer( io_ctx )
  , _timer( io_ctx )
  , _dest_ep(ep)
  , _pong_handler( pong_handler )
  , _timeout_handler( timeout_handler )
  , _is_pong_arrived( false )
{
  return;
}

void ping::init()
{
  _timer.expires_from_now( boost::posix_time::seconds(DEFAULT_PING_RESPONSE_TIMEOUT_s-1) ); // observerの方が先に削除されないように
  _timer.async_wait( std::bind( &ping::timeout, this, std::placeholders::_1 ) );

  k_observer::extend_expire_at( DEFAULT_PING_RESPONSE_TIMEOUT_s + 1 );
  // 指定時間経過後にpongが到着していなければswapする
}

void ping::timeout( const boost::system::error_code &ec )
{
  if( _is_pong_arrived ) return; // pongが到着していれば特に何もしない

  /* #if SS_VERBOSE
  std::cout << "\x1b[33m" << "[ping observer]:<" << get_id_str() << "> ::" << "\x1b[39m" <<" timeout" << "\n";
  #endif */

  if( !_is_pong_arrived ){
  _io_ctx.post( [this]() // タイムアウトした時ようのハンドラを呼び出す
	  {
		this->_timeout_handler(_dest_ep);
	  }) ;
  }
  this->destruct_self(); // 実質破棄を許可する
}

int ping::income_message( message &msg, ip::udp::endpoint &ep )
{
  /* #if SS_VERBOSE
  std::cout << "\x1b[33m" << "[ping observer]:<"<< get_id_str() <<"> :: " << "\x1b[39m" <<" pong arrive" << "\n";
  #endif */

  _is_pong_arrived = true;

  _io_ctx.post([this](){ // on_pong_handlerを呼び出す
		this->_pong_handler(_dest_ep);
	  });

  _timer.cancel();
  this->destruct_self() ; // 実質破棄を許可する(refresh_tickで削除される)
  return 0;
}

void ping::print() const
{
  std::cout << "[observer] (ping) " << "<" << _id << ">";
  std::cout << " [ at: "<< k_observer::get_expire_time_left() <<" ]";
}


find_node::find_node( io_context &io_ctx, on_response_handler response_handler ) :
  k_observer( io_ctx )
  // , _rpc_manager( rpc_manager )
  , _response_handler( response_handler )
{
  return;
}

void find_node::init()
{
  return;

  #if SS_VERBOSE
  std::cout << "[find_node observer]:<" << get_id_str() << ">(init)" << "\n";
  #endif
}

int find_node::income_message( message &msg, ip::udp::endpoint &ep )
{
  /* #if SS_VERBOSE
  std::cout << "\x1b[31m" << "[find_node observer]:<" << get_id_str() << ">(response arrive)" << "\x1b[39m" << "\n";
  #endif */

  if( auto k_param = msg.get_param("kademlia"); k_param == nullptr ) return 0;
  k_message k_msg( *(msg.get_param("kademlia")) );

  auto msg_controller = k_msg.get_find_node_message_controller();
  auto finded_eps = msg_controller.get_finded_endpoint();

  for( auto itr : finded_eps )
  {
	_io_ctx.post([this, itr](){
		  this->_response_handler(itr);
		});
  }

  this->destruct_self() ; // 実質破棄を許可する(refresh_tickで削除される)
  return 0;
}

void find_node::print() const
{
  std::cout << "[observer] (find_node) " << "<" << _id << ">";
  std::cout << " [ at: "<< k_observer::get_expire_time_left() <<" ]";
}


}; // namespace kademlia
}; // namespace ss
