#ifndef E1AEB5FF_5603_4904_95D1_8CFA1AC889F4
#define E1AEB5FF_5603_4904_95D1_8CFA1AC889F4


#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <span>
  
#include <json.hpp>
#include <utils.hpp>
#include <ss_p2p/kademlia/k_node.hpp>
#include <ss_p2p/observer.hpp>


using json = nlohmann::json;


namespace ss
{
namespace kademlia
{


const std::vector<std::string> kademlia_message_params = {"rpc","k_node"};


class k_message
{
private:
  json _body;

public:
  enum rpc
  {
	ping
	, find_node
	, none
  };
  enum message_type
  {
	request
	  , response
  };

  k_message( std::string message_type );
  k_message( json &k_msg );

  void set_param( std::string key, std::string value );
  template < typename T > 
  T get_param( std::string key )
  {
	auto value = _body[key];
	if constexpr (std::is_same_v<T, observer_id>)
	{
	  return str_to_observer_id( value.get<std::string>() );
	}
	return T{};
  }

  static k_message (_request_)( k_message::rpc r );
  static k_message (_response_)( k_message::rpc r );

  struct find_node_message_controller
  {
	friend k_message;
	find_node_message_controller( k_message *from );
	std::vector<ip::udp::endpoint> get_ignore_endpoint();
	void set_ignore_endpoint( std::vector<ip::udp::endpoint> eps ); // ノードをリクエストする際に検索対象外としてもらうepリスト
	std::vector<ip::udp::endpoint> get_finded_endpoint();
	void set_finded_endpoint( std::vector<ip::udp::endpoint> eps ); // 自身のルーティングテーブルから取得したk_nodeを格納する(実際にはepを格納する)
	private: 
	  json &_body;
  };
  find_node_message_controller get_find_node_message_controller();

  bool is_request() const;

  void set_rpc( k_message::rpc r );
  rpc get_rpc() const;
  message_type get_message_type() const;
  void set_message_type( message_type t );

  [[nodiscard]] bool validate() const; // 必ずvalidateする
 
  observer_id get_observer_id(); 
  void set_observer_id( const observer_id &id );
  const json encode();
  void print();
};


};
};


#endif 


