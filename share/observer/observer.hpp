#ifndef C776E3FF_665F_4DE0_A70F_182B328C349F
#define C776E3FF_665F_4DE0_A70F_182B328C349F

#include "boost/asio.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/algorithm/string/erase.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/lexical_cast.hpp"

#include <memory>
#include <string>


using namespace boost::asio;
using namespace boost::uuids;
constexpr std::time_t DEFAULT_EXPIRE_TIME_s = 10;


class base_observer
{
public:
  using id = uuid;
  uuid get_id() const;
  bool is_expired() const;
  std::time_t get_expire_time_left() const;

protected:
  base_observer( io_context &io_ctx ); // 同期処理の為
  
  void destruct_self();
  void extend_expired_at(); // 有効期限の延長

private:
  std::time_t _expired_at; // 有効期限
  io_context &_io_ctx;
  const id _id;
};


template < typename T >
class observer
{
private:
  std::shared_ptr<T> _body;

public:
  using id = base_observer::id; // エイリアス
  
  observer( std::shared_ptr<T> from ) {
	_body = from;
  }
  observer( T from ){
	_body = std::make_shared<T>(from);
  }
  template < typename ... Args >
  observer( io_context &io_ctx/*strageのio_ctxを使ってもいいか？*/, Args... args ){
	_body = std::make_shared<T>( io_ctx, std::forward<Args>(args)... );
  }
  template < typename ... Args >
  void init( Args&& ... args ){
	return _body->init( std::forward<Args>(args)... );
  }
  bool is_expired() const
  {
	return _body->is_expired();
  }
  struct Hash;
};
template < typename T >
struct observer<T>::Hash
{
  std::size_t operator()( const observer<T> &obs ) const
  {
	return std::hash<std::string>()( boost::uuids::to_string(obs.get_id()) );
  }
};


using observer_id = base_observer::id;


#endif 
