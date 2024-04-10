#include <ss_p2p/kademlia/direct_routing_table_controller.hpp>


namespace ss
{
namespace kademlia
{


direct_routing_table_controller::direct_routing_table_controller( k_routing_table &routing_table ) : 
  _routing_table( routing_table )
{
  return;
}

k_routing_table &direct_routing_table_controller::get_routing_table()
{
  return _routing_table;
}

std::vector<k_node> direct_routing_table_controller::collect_node( k_node &root_node, std::size_t max_count, const std::vector<k_node> &ignore_nodes )
{
 return _routing_table.collect_node( root_node, max_count, ignore_nodes );
}

std::vector<ip::udp::endpoint> direct_routing_table_controller::collect_endpoint( ip::udp::endpoint &root_ep, std::size_t max_count, const std::vector<ip::udp::endpoint> &ignore_eps )
{
  k_node root_node(root_ep);
  std::vector<k_node> ignore_nodes; 
  for( auto itr : ignore_eps ){
	k_node kn(itr);
	ignore_nodes.push_back(kn);
  }

  const auto geted_nodes = this->collect_node( root_node, max_count, ignore_nodes );

  std::vector<ip::udp::endpoint> ret;
  for( auto itr : geted_nodes ) ret.push_back( itr.get_endpoint() );
  return ret;
}

bool direct_routing_table_controller::is_exist( ip::udp::endpoint &ep ) const
{
  k_node target_node(ep);
  return _routing_table.is_exist(target_node);
}

bool direct_routing_table_controller::is_exist( k_node &kn ) const 
{
  return _routing_table.is_exist(kn);
}

void direct_routing_table_controller::auto_update_batch( std::vector<ip::udp::endpoint> eps )
{
  for( auto itr : eps )
  {
	k_node target_k_node( itr );
	_routing_table.auto_update( target_k_node );
  }
  return;
}

void direct_routing_table_controller::auto_update( ip::udp::endpoint ep )
{
  k_node target_k_node( ep );
  _routing_table.auto_update( target_k_node );
}

std::size_t direct_routing_table_controller::get_node_count()
{
  return _routing_table.get_node_count();
}

k_bucket_iterator direct_routing_table_controller::get_begin_bucket_iterator()
{
  return _routing_table.get_begin_bucket_iterator();
}

void direct_routing_table_controller::print( int start_branch ) const
{
  return _routing_table.print( start_branch );
}


};
};
