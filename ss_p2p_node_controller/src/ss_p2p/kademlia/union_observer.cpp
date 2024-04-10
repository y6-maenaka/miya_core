#include <ss_p2p/kademlia/union_observer.hpp>


namespace ss
{
namespace kademlia
{


base_observer::observer_id _union_observer_get_id::operator()( const std::shared_ptr<ping_observer> &obs )
{
  return obs->get_id();
}
bool _union_observer_is_expired::operator()( const std::shared_ptr<ping_observer> &obs )
{
  return obs->is_expired();
}

void union_observer::init()
{
  if( std::holds_alternative<std::shared_ptr<ping_observer>>(_obs) ){
	auto ping_obs = std::get<std::shared_ptr<ping_observer>>(_obs);
	ping_obs->init();
	return;
  }
  if( std::holds_alternative<std::shared_ptr<find_node_observer>>(_obs) ){
	auto find_node_obs = std::get<std::shared_ptr<find_node_observer>>(_obs);
	find_node_obs->init();
	return;
  }
  return;
}

base_observer::observer_id union_observer::get_id() const
{
  std::shared_ptr<base_observer> base_obs;
  if( std::holds_alternative<std::shared_ptr<ping_observer>>(_obs) )
	base_obs = std::dynamic_pointer_cast<base_observer>( std::get<std::shared_ptr<ping_observer>>(_obs) );
  else if( std::holds_alternative<std::shared_ptr<find_node_observer>>(_obs) )
	base_obs = std::dynamic_pointer_cast<base_observer>( std::get<std::shared_ptr<find_node_observer>>(_obs) );

  return base_obs->get_id();
}

void union_observer::print() const
{
  if( std::holds_alternative<std::shared_ptr<ping_observer>>(_obs) ) std::cout << "(type): ping" << "\n";
  std::cout << "(id): " << this->get_id() << "\n";
}

bool union_observer::operator==( const union_observer &obs ) const
{
  return this->get_id() == obs.get_id();
}

bool union_observer::operator!=( const union_observer &obs ) const
{
  return this->get_id() != obs.get_id();
}

bool union_observer::is_expired() const
{
  std::shared_ptr<base_observer> base_obs;
  if( std::holds_alternative<std::shared_ptr<ping_observer>>(_obs) )
	base_obs = std::dynamic_pointer_cast<base_observer>( std::get<std::shared_ptr<ping_observer>>(_obs) );
  else if( std::holds_alternative<std::shared_ptr<find_node_observer>>(_obs) )
	base_obs = std::dynamic_pointer_cast<base_observer>( std::get<std::shared_ptr<find_node_observer>>(_obs) );

  if( base_obs == nullptr) return false;
  return base_obs->is_expired();
}


std::size_t union_observer::Hash::operator()(const union_observer &obs ) const
{
  return std::hash<std::string>()( boost::uuids::to_string(obs.get_id()) );
}


}; // namespace kademlia
}; // namespace ss
