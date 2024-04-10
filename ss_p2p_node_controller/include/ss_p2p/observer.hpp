#ifndef A3106EAE_F653_43A8_AFA9_3E54D88AD98F
#define A3106EAE_F653_43A8_AFA9_3E54D88AD98F


#include <ss_p2p/message.hpp>

#include "boost/asio.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/algorithm/string/erase.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/lexical_cast.hpp"


using namespace boost::asio;
using namespace boost::uuids;


namespace ss
{


constexpr unsigned short DEFAULT_EXPIRE_TIME_s = 20/*[seconds]*/;


class base_observer
{
public:
  using id = uuid;
  uuid get_id() const;
  std::string get_id_str() const;
  bool is_expired() const;

protected:
  base_observer( io_context &io_ctx );

  void destruct_self(); // 本オブザーバーの破棄を許可する
  void extend_expire_at( std::time_t t = DEFAULT_EXPIRE_TIME_s );
  std::time_t get_expire_time_left() const;

  std::time_t _expire_at; // このオブザーバーを破棄する時間
  io_context &_io_ctx;
  const id _id;
};


template < typename T >
class observer // 実質wrapperクラス
{
private:
  std::shared_ptr<T> _body;

public:
  using id = base_observer::id;

  observer( std::shared_ptr<T> from )
  {
	_body = from;
  }
  observer( T from )
  {
	_body = std::make_shared<T>(from);
  }

  template < typename ... Args >
  observer( io_context &io_ctx, Args&& ... args )
  {
	_body = std::make_shared<T>(io_ctx, std::forward<Args>(args)...);
  }

  template < typename ... Args >
  void init( Args&& ... args )
  {
	return _body->init( std::forward<Args>(args)... );
  }
  int income_message( message &msg, ip::udp::endpoint &ep )
  {
	return _body->income_message(msg, ep);
  }
  void on_send_done( const boost::system::error_code &ec )
  {
	return _body->on_send_done( ec );
  }
  bool is_expired() const
  {
	return _body->is_expired();
  }
  id get_id() const
  {
	return _body->get_id();
  }
  void print() const
  {
	return _body->print();
  }
  std::shared_ptr<T> get_raw()
  {
	return _body;
  }
  std::time_t get_expire_time_left() const
  {
	return std::dynamic_pointer_cast<base_observer>(_body)->get_expire_time_left();
  }
  bool operator==(const observer<T> &obs ) const
  {
	return _body->get_id() == obs.get_id();
  }
  bool operator!=(const observer<T> &obs ) const
  {
	return _body->get_id() != obs.get_id();
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
base_observer::id str_to_observer_id( std::string from );
std::string observer_id_to_str( const observer_id &from );

};


#endif 


