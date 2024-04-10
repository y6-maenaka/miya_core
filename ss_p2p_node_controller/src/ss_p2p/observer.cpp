#include <ss_p2p/observer.hpp>


namespace ss
{


base_observer::base_observer( io_context &io_ctx ) : 
  _id( random_generator()() ) 
  , _io_ctx( io_ctx )
  , _expire_at(std::time(nullptr) + DEFAULT_EXPIRE_TIME_s)
{
  return;
}

std::time_t base_observer::get_expire_time_left() const
{
  return _expire_at - std::time(nullptr);
}

base_observer::id base_observer::get_id() const
{
  return _id;
}

std::string base_observer::get_id_str() const
{
  return boost::uuids::to_string( _id );
}

void base_observer::destruct_self()
{
  _expire_at = 0;
  // _expire_at = std::time(nullptr) + 1; // ちょっとだけバッファを儲けてtimeoutハンドラの実行中にクリアされ流ことを避ける
}

void base_observer::extend_expire_at( std::time_t t )
{
  _expire_at = std::time(nullptr) + t;
}

bool base_observer::is_expired() const
{
  return _expire_at <= std::time(nullptr);
}


base_observer::id str_to_observer_id( std::string id_str )
{
  boost::uuids::string_generator gen;
  return gen( id_str );
}

std::string observer_id_to_str( const observer_id &from )
{
  auto ret = boost::uuids::to_string( from );
  // boost::erase_all( ret, "-" );
  return ret;
}


};
