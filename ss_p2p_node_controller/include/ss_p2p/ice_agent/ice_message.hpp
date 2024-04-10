#ifndef FE95B122_6933_464A_8DF3_16040813D3D9
#define FE95B122_6933_464A_8DF3_16040813D3D9


#include <string>
#include <optional>

#include <json.hpp>
#include <utils.hpp>
#include <ss_p2p/observer.hpp>

#include "boost/asio.hpp"
#include "boost/uuid/uuid.hpp"


using json = nlohmann::json;
using namespace boost::asio;
using namespace boost::uuids;


namespace ss
{
namespace ice
{


class ice_message
{
private:
  json _body;

public:
  void print() const;
    
  enum protocol_t
  {
	signaling
	, stun
	, none
  };
  protocol_t _protocol;

  static ice_message (_stun_)();
  static ice_message (_signaling_)();
  ice_message( protocol_t p ); // ホスト作成
  ice_message( json &from );


  protocol_t get_protocol() const;
  void set_protocol( protocol_t p );

  struct stun
  {
	enum sub_protocol_t
	{
	  binding_request
		, binding_response
	};
  };
  struct signaling
  {
	enum sub_protocol_t
	{
	  request
		, response
		, relay
	};
  };

  struct stun_message_controller
  {
	friend ice_message;
	ice_message::stun::sub_protocol_t sub_protocol;
	void set_sub_protocol( ice_message::stun::sub_protocol_t p );
	ice_message::stun::sub_protocol_t get_sub_protocol();
	std::optional<ip::udp::endpoint> get_global_ep();
	stun_message_controller( json &body );
	stun_message_controller( ice_message *from );
	void set_global_ep( ip::udp::endpoint &ep ); // リクエスト元の(グローバル)アドレスを格納する
	void print() const;

	private:
	  json &_body;
  };

  struct signaling_message_controller
  {
	friend ice_message;
	ice_message::signaling::sub_protocol_t sub_protocol;

	signaling_message_controller( json &body );
	signaling_message_controller( ice_message *from );

	void set_dest_endpoint( ip::udp::endpoint &ep ); // nat到達先アドレスのセット
	void set_src_endpoint( ip::udp::endpoint &ep ); // 自身(リクエスト元)アドレスのセット
	void add_relay_endpoint( ip::udp::endpoint ep ); // 中継したノード(endpoint)を追加
	std::vector< ip::udp::endpoint > get_relay_endpoints();
	ip::udp::endpoint get_dest_endpoint() const;
	ip::udp::endpoint get_src_endpoint() const;
	void set_sub_protocol( ice_message::signaling::sub_protocol_t p ); // request, response, relay
	ice_message::signaling::sub_protocol_t get_sub_protocol();
	int get_ttl() const;
	void decrement_ttl();
	void print() const;

	private:		
	  json &_body;
  };
  signaling_message_controller get_signaling_message_controller();
  stun_message_controller get_stun_message_controller();

  template <typename T>
  T get_param( std::string key )
  {
	
	auto value = _body[key];
	if constexpr (std::is_same_v<T, observer_id>)
	{
	  return str_to_observer_id( value.get<std::string>() );
	}
	
	return T{};
  }
  
  void set_param( std::string key, std::string value );
  void set_observer_id( const observer_id &id );
  observer_id get_observer_id();
  const json encode();
};


};
};


#endif 


