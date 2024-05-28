#ifndef F348B366_1EA4_449B_943F_E1BB62918819
#define F348B366_1EA4_449B_943F_E1BB62918819


#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <functional>

#include <utils.hpp>
#include <ss_p2p/ss_logger.hpp>
#include <ss_p2p/observer.hpp>
#include <ss_p2p/observer_strage.hpp>
#include "./ice_observer.hpp"
#include "./ice_observer_strage.hpp"

#include "boost/asio.hpp"


using namespace boost::asio;


namespace ss
{
namespace ice
{


constexpr unsigned short DEFAULT_OBSERVER_STRAGE_TICK_TIME_s = 10/*[seconds]*/;
constexpr unsigned short DEFAULT_OBSERVER_STRAGE_SHOW_STATE_TIME_s = 2/*[seconds]*/;


class ice_observer_strage : public ss::observer_strage
{
protected:
  union_observer_strage< signaling_request, signaling_response, signaling_relay, binding_request > _strage;

  template < typename T >
  void delete_expires_observer( observer_strage_entry<T> &entry )
  {
	for( auto itr = entry.begin(); itr != entry.end(); )
	{
	  if( (*itr).is_expired() )
	  {
		/* #if SS_VERBOSE
		std::cout << "\x1b[33m" << " | [ice observer strage](delete observer) :: " <<"\x1b[39m" << (*itr).get_id() << "\n";
		#endif */
		itr = entry.erase(itr);
	  }
	  else itr++;
	}
	return;
  }

  template < typename T >
  void print_entry_state( observer_strage_entry<T> &entry )
  {
	for( int i=0; i<get_console_width()/2; i++ ){ printf("="); } std::cout << "\n";
	if constexpr (std::is_same_v<T, signaling_request>)
	  std::cout << "| signaling_request" << "\n";
	else if constexpr (std::is_same_v<T, signaling_relay>) std::cout << "| signaling_relay" << "\n";
	else if constexpr (std::is_same_v<T, signaling_response>) std::cout << "| signaling_response" << "\n";
	else if constexpr (std::is_same_v<T, binding_request>) std::cout << "| binding_request" << "\n";
	else std::cout << "| undefine" << "\n";

	unsigned int count = 0;
	for( int i=0; i<get_console_width()/2; i++ ){ printf("-"); } std::cout << "\n";
	std::cout << "\x1b[32m";
	for( auto &itr : entry )
	{
	  std::cout << "| ("<< count << ") ";
	  itr.print(); std::cout << "\n";
	  count++;
	}
	std::cout << "\x1b[39m" << "\n";
  }

  void refresh_tick( const boost::system::error_code &ec );
  void call_tick();

  deadline_timer _refresh_tick_timer;

public:
  template < typename T >
  std::optional< observer<T> > find_observer( observer_id id ) // 検索・取得メソッド
  {
   auto &s_entry = std::get< observer_strage_entry<T> >(_strage);
	for( auto &itr : s_entry )
	  if( itr.get_id() == id && !(itr.is_expired()) ) return itr;

	return std::nullopt;
  }

  template < typename T >
  void add_observer( observer<T> obs ) // 追加メソッド
  {
	/* #if SS_VERBOSE
	if constexpr (std::is_same_v<T, signaling_request>) std::cout << "| [ice observer strage](signaling_request observer) store" << "\n";
	else if constexpr (std::is_same_v<T, signaling_relay>) std::cout << "| [ice observer strage](signaling_relay observer) store" << "\n";
	else if constexpr (std::is_same_v<T, signaling_response>) std::cout << "| [ice observer strage](signaling_response observer) store" << "\n";
	else if constexpr (std::is_same_v<T, binding_request>) std::cout << "| [ice observer strage](binding_request observer) store" << "\n";
	else std::cout << "| [ice observer strage](undefined observer) store" << "\n";
	#endif */

	auto &s_entry = std::get< observer_strage_entry<T> >(_strage);
	s_entry.insert(obs);
  }

  ice_observer_strage( io_context &io_ctx );

  #if SS_DEBUG
  deadline_timer _debug_tick_timer;
  void show_state( const boost::system::error_code &ec );
  #endif

};


};
};


#endif
