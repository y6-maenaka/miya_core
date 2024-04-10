#ifndef CD87A151_55B7_427F_969A_75CA759E9C4A
#define CD87A151_55B7_427F_969A_75CA759E9C4A


#include <iostream>
#include <variant>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include <utils.hpp>
#include <ss_p2p/observer.hpp>
#include <ss_p2p/observer_strage.hpp>
#include "./k_observer.hpp"

#include "boost/asio.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/lexical_cast.hpp"


using namespace boost::asio;
using namespace boost::uuids;


namespace ss
{
namespace kademlia
{


constexpr unsigned short DEFAULT_OBSERVER_STRAGE_TICK_TIME_s = 8/*[seconds]*/;
constexpr unsigned short DEFAULT_OBSERVER_STRAGE_SHOW_STATE_TIME_s = 2/*[seconds]*/;


class k_observer_strage : public ss::observer_strage
{
protected:
  union_observer_strage< ping, find_node > _strage;

  deadline_timer _refresh_tick_timer;

  template < typename T >
  void delete_expires_observer( observer_strage_entry<T> &entry )
  {
	for( auto itr = entry.begin(); itr != entry.end(); )
	{
	  if( (*itr).is_expired() ){
		#if SS_VERBOSE
		std::cout << "\x1b[33m" << " | [k observer strage](delete observer) " <<"\x1b[39m" << (*itr).get_id() << "\n";
		#endif
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
	if constexpr (std::is_same_v<T, ping>) std::cout << "| ping" << "\n";
	else if constexpr (std::is_same_v<T, find_node>) std::cout << "| find_node" << "\n";
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
  void call_tick( std::time_t tick_time_s = DEFAULT_OBSERVER_STRAGE_TICK_TIME_s );

public:
  template < typename T >
  std::optional< observer<T> > find_observer( observer_id id )
  {
   auto &s_entry = std::get< observer_strage_entry<T> >(_strage);
	for( auto &itr : s_entry )
	  if( itr.get_id() == id ) return itr;

	return std::nullopt;
  }

  template < typename T >
  void add_observer( observer<T> obs )
  {
	auto &s_entry = std::get< observer_strage_entry<T> >(_strage);
	s_entry.insert(obs);

	#if SS_VERBOSE
	if constexpr (std::is_same_v<T, ping>) std::cout << "| [k observer strage](pign observer) store" << "\n";
	else if constexpr (std::is_same_v<T, find_node>) std::cout << "| [k observer strage](find_node observer) store" << "\n";
	else std::cout << "| [k observer strage](undefined observer) store" << "\n";
	#endif
  }

  k_observer_strage( io_context &io_ctx );

  #if SS_DEBUG
  deadline_timer _debug_tick_timer;
  void show_state( const boost::system::error_code &ec );
  #endif
};


};
};


#endif
