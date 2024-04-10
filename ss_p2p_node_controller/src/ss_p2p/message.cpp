#include <ss_p2p/message.hpp>


namespace ss
{


message::message( app_id id )
{
  this->set_app_id(id);
}

message::message( json from )
{
  this->_body = from;
}

void message::set_app_id( app_id id )
{
  _body["app_id"] = id;
}

std::shared_ptr<json> message::get_param( std::string param )
{
  if( _body.contains(param) ) return std::make_shared<json>(_body[param]);
  return nullptr;
}

void message::set_param( std::string key, const json value )
{
  _body[key] = value;
}

bool message::is_contain_param( std::string param )
{
  return _body.contains(param);
}

std::vector<std::uint8_t> message::encode( const message &from )
{
  std::vector<std::uint8_t> ret;
  ret = nlohmann::json::to_bson(from._body);
  return ret;
}

message message::decode( std::vector<std::uint8_t> from )
{
  std::string _( from.begin(), from.end() ); // 直接std::vector<std::uint8_t>を渡すと警告が発生するため仕方なく
  json ret_json = nlohmann::json::from_bson( _ );
  return message(ret_json); 
}

void message::print() 
{
  std::cout << _body << "\n";
}


}; // namespace ss
