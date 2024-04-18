#include "./observer.hpp"


base_observer::base_observer( io_context &io_ctx ) :
  _id( random_generator()() )
  , _io_ctx( io_ctx )
  , _expired_at( std::time(nullptr) + DEFAULT_EXPIRE_TIME_s )
{
  return;
}

base_observer::id base_observer::get_id() const
{
  return _id;
}
