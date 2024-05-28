#include <ss_p2p/ice_agent/ice_observer_strage.hpp>


namespace ss
{
namespace ice
{


ice_observer_strage::ice_observer_strage( io_context &io_ctx ) :
  observer_strage( io_ctx )
  , _refresh_tick_timer( io_ctx )
  #if SS_DEBUG
  , _debug_tick_timer( io_ctx )
  #endif
{
  this->call_tick();
}

void ice_observer_strage::refresh_tick( const boost::system::error_code &ec )
{
  std::apply([this](auto &... args)
	  {
		  ((delete_expires_observer(args)), ...);
	  }, _strage );

  /* #if SS_VERBOSE
  std::cout << "(ice_observer_strage) refresh_tick called" << "\n";
  #endif */

  call_tick(); // 循環的に呼び出し
}

void ice_observer_strage::call_tick()
{
  _refresh_tick_timer.expires_from_now(boost::posix_time::seconds( DEFAULT_OBSERVER_STRAGE_TICK_TIME_s ));
  _refresh_tick_timer.async_wait( std::bind( &ice_observer_strage::refresh_tick, this , std::placeholders::_1 ) );
}

#if SS_DEBUG
void ice_observer_strage::show_state( const boost::system::error_code &ec )
{
  std::apply([this](auto &... args)
	{
		((print_entry_state(args)), ...);
	}, _strage );

  _debug_tick_timer.expires_from_now(boost::posix_time::seconds( DEFAULT_OBSERVER_STRAGE_SHOW_STATE_TIME_s ));
  _debug_tick_timer.async_wait( std::bind( &ice_observer_strage::show_state, this , std::placeholders::_1 ) );
}
#endif


};
};
