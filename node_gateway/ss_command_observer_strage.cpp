#include <node_gateway/ss_command_observer_strage.hpp>


ss_command_observer_strage::ss_command_observer_strage( io_context &io_ctx ) :
  observer_strage(io_ctx)
  , _requires_refresh(true)
  , _refresh_tick_timer(io_ctx)
{
  if( _requires_refresh ) this->call_tick();
}


void ss_command_observer_strage::refresh_tick( const boost::system::error_code &ec )
{
  std::apply([this](auto &... args)
	  {
		((delete_expires_observer(args)), ...);
	  }, _strage );

  #if L1_VERBOSE
  std::cout << "(ss_command_observer_strage) refresh_tick called" << "\n";
  #endif

  call_tick(); // 次のtickをタイマーする
}

void ss_command_observer_strage::call_tick()
{
  if( !_requires_refresh ) return;

  _refresh_tick_timer.expires_from_now( boost::posix_time::seconds(DEFAULT_OBSERVER_STRAGE_REFRESH_TIME_s) );
  _refresh_tick_timer.async_wait( std::bind(&ss_command_observer_strage::refresh_tick, this, std::placeholders::_1) );
}


