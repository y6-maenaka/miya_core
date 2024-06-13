#ifndef F948F351_5877_4388_A5DD_A06ED2791A4D
#define F948F351_5877_4388_A5DD_A06ED2791A4D


#include <optional>

#include "boost/asio.hpp"
#include <observer/observer.hpp>
#include <observer/observer_strage.hpp>
#include <node_gateway/ss_command_observer.hpp>


using namespace boost::asio;
constexpr unsigned short DEFAULT_OBSERVER_STRAGE_REFRESH_TIME_s = 10;

/*
class ss_command_observer_strage : observer_strage
{
protected:
  union_observer_strage< class getdata > _strage;

  template < typename T >
  void delete_expires_observer( entry<T> &e )
  {
	for( auto itr = e.begin(); itr != e.end(); )
	{
	  if( (*itr).is_expired() )
	  {
		#if L1_VERBOSE
		std::cout << "\x1b[33m" << " | [ice observer strage](delete observer) :: " <<"\x1b[39m" << (*itr).get_id() << "\n";
		#endif
		itr = e.erase(itr);
	  }
	  else itr++;
	}
	return;
  };

  void refresh_tick( const boost::system::error_code &ec );
  void call_tick();
  deadline_timer _refresh_tick_timer;
  bool _requires_refresh;

public:
  template < typename T >
  std::optional< observer<T> > find( const observer_id id )
  {
	auto &e = std::get< entry<T> >(_strage);
	for( auto &itr : e )
	  if( itr.get_id() == id && !(itr.is_expired()) ) return itr;

	return std::nullopt;
  };

  template < typename T >
  void add( observer<T> target )
  {
	auto &e = std::get< entry<T> >(_strage);
	e.insert(target);
  };

  ss_command_observer_strage( io_context &io_ctx );
};
*/


#endif 
