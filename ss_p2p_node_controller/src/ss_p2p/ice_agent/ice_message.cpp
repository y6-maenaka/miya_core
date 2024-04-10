#include <ss_p2p/ice_agent/ice_message.hpp>
#include <ss_p2p/observer.hpp>


namespace ss
{
namespace ice
{


void ice_message::print() const
{
  std::cout << _body << "\n";
}

ice_message::ice_message( ice_message::protocol_t p )
{
  set_protocol( p );

  // 必須項目
  json relay_eps = json::array();
  _body["relay_eps"] = relay_eps;
  _body["ttl"] = 5;
}

ice_message ice_message::_signaling_()
{
  ice_message ret( protocol_t::signaling );
  return ret;
}

ice_message ice_message::_stun_()
{
  ice_message ret( protocol_t::stun );
  return ret;
}

ice_message::ice_message( json &from )
{
  _body = from;

  if( _body["protocol"] == "signaling" ) _protocol = protocol_t::signaling;
  else if( _body["protocol"] == "stun" ) _protocol = protocol_t::stun;
  else _protocol = protocol_t::none;
}

ice_message::protocol_t ice_message::get_protocol() const
{
  return _protocol;
}

void ice_message::set_protocol( protocol_t p )
{
  switch( p )
  {
	case protocol_t::signaling :
	  {
		_body["protocol"] = "signaling";
		_protocol = p;
		break;
	  }
	case protocol_t::stun :
	  {
		_body["protocol"] = "stun";
		_protocol = p;
		break;
	  }
	default :
	  {
		_body["protocol"] = "none";
		_protocol = p;
	  }
  }
}

ice_message::signaling_message_controller ice_message::get_signaling_message_controller()
{
  return ice_message::signaling_message_controller(this);
}

ice_message::stun_message_controller ice_message::get_stun_message_controller()
{
  return ice_message::stun_message_controller(this);
}

const json ice_message::encode()
{
  return _body;
}

void ice_message::set_param( std::string key, std::string value )
{
  _body[key] = value;
}

void ice_message::set_observer_id( const observer_id &id )
{
  this->set_param("observer_id", observer_id_to_str(id) );
}

observer_id ice_message::get_observer_id()
{
  return this->get_param<observer_id>("observer_id");
}


ice_message::stun_message_controller::stun_message_controller( json &body ) : 
  _body( body)
{
  sub_protocol = _body["sub_protocol"];
}

ice_message::stun_message_controller::stun_message_controller( ice_message *from )
  : _body(from->_body)
{
  return;
}

void ice_message::stun_message_controller::set_sub_protocol( ice_message::stun::sub_protocol_t p )
{
  _body["sub_protocol"] = p;
}

ice_message::stun::sub_protocol_t ice_message::stun_message_controller::get_sub_protocol()
{
  return _body["sub_protocol"];
}

std::optional<ip::udp::endpoint> ice_message::stun_message_controller::get_global_ep()
{
  std::string global_ip_str = _body["global_ip"];
  unsigned short global_port = _body["global_port"];
  
  return std::optional<ip::udp::endpoint>(ss::addr_pair_to_endpoint( global_ip_str, global_port ));
}

void ice_message::stun_message_controller::set_global_ep( ip::udp::endpoint &ep )
{
  _body["global_ip"] = ep.address().to_string();
  _body["global_port"] = ep.port();
}


ice_message::signaling_message_controller::signaling_message_controller( json &body ) :
  _body(body)
{
  sub_protocol = _body["sub_protocol"];
}

ice_message::signaling_message_controller::signaling_message_controller( ice_message *from ) : 
  _body( from->_body )
{
  return;
}

void ice_message::signaling_message_controller::set_dest_endpoint( ip::udp::endpoint &ep )
{
  _body["dest_ip"] = ep.address().to_string();
  _body["dest_port"] = ep.port();
}

void ice_message::signaling_message_controller::set_src_endpoint( ip::udp::endpoint &ep )
{
  _body["src_ip"] = ep.address().to_string();
  _body["src_port"] = ep.port();
}

void ice_message::signaling_message_controller::add_relay_endpoint( ip::udp::endpoint ep )
{
  _body["relay_eps"].push_back( endpoint_to_str(ep) );
}

void ice_message::signaling_message_controller::set_sub_protocol( ice_message::signaling::sub_protocol_t p )
{
  _body["sub_protocol"] = p;
}

ice_message::signaling::sub_protocol_t ice_message::signaling_message_controller::get_sub_protocol()
{
  return _body["sub_protocol"];
}

std::vector<ip::udp::endpoint> ice_message::signaling_message_controller::get_relay_endpoints()
{
  std::vector<ip::udp::endpoint> ret;
  for( int i=0; i<_body["relay_eps"].size(); i++ ){
	std::string ep_str = _body["relay_eps"][i].get<std::string>();
	ret.push_back( str_to_endpoint( ep_str ) );
  }
  return ret;
}

ip::udp::endpoint ice_message::signaling_message_controller::get_dest_endpoint() const
{
  std::string dest_ip = _body["dest_ip"];
  unsigned short dest_port = _body["dest_port"];

  return ss::addr_pair_to_endpoint( dest_ip, dest_port );
}

ip::udp::endpoint ice_message::signaling_message_controller::get_src_endpoint() const
{
  std::string src_ip = _body["src_ip"];
  unsigned short src_port = _body["src_port"];
  
  return ss::addr_pair_to_endpoint( src_ip, src_port );
}

int ice_message::signaling_message_controller::get_ttl() const
{
  return _body["ttl"].get<int>();
}

void ice_message::signaling_message_controller::decrement_ttl()
{
  _body["ttl"] = _body["ttl"].get<int>() - 1;
}

void ice_message::signaling_message_controller::print() const
{
  std::cout << _body << "\n";
}


}; // namespace ice
}; // namespace ss
