#ifndef CA6372A7_80AB_4610_AB39_A166F105AE6D
#define CA6372A7_80AB_4610_AB39_A166F105AE6D


#include <unordered_set>

#include <share/observer/observer.hpp>

#include "boost/asio.hpp"


using namespace boost::asio;


constexpr std::time_t DEFAULT_OBSERVER_STRAGE_REFRESH_TICK_TIME_s = 30;


class observer_strage
{
protected:
  template < typename T >
  using entry = std::unordered_set< observer<T>, typename observer<T>::Hash >;

  template < typename ... Ts >
  using union_observer_strage = std::tuple< entry<Ts>... >;
  union_observer_strage<> _strage;

  io_context &_io_ctx;

public:
  observer_strage( io_context &io_ctx );
};


#endif

