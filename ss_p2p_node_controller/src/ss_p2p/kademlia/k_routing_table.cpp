#include <ss_p2p/kademlia/k_routing_table.hpp>


namespace ss
{
namespace kademlia
{


k_routing_table::k_routing_table( node_id self_id, ss_logger *logger ) :
  _self_id( self_id )
  , _logger( logger )
{
  return;
}

unsigned short k_routing_table::calc_branch( k_node &kn ) // オーバヘッドが多少大きい
{
  node_id self_node_id = _self_id;
  node_id peer_node_id = kn.get_id();
  unsigned short node_xor_distance = calc_node_xor_distance( self_node_id, peer_node_id );
  return (K_NODE_ID_LENGTH*8) - node_xor_distance;
}

void k_routing_table::swap_node( k_node &node_src, k_node node_dest )
{
  k_bucket &target_bucket = this->get_bucket(node_src);
  return target_bucket.swap_node( node_src, node_dest );
}

k_bucket::update_state k_routing_table::auto_update( k_node kn )
{
  // k_node target_node( ep );
  unsigned short branch_idx = calc_branch_index( kn );
  k_bucket& target_bucket = _table[branch_idx];

  auto ret = target_bucket.auto_update( kn );

  #if SS_VERBOSE
  /*
  std::cout << "\x1b[31m" << "[ routing table update ]" << "\x1b[39m ";
  std::cout << "(endpoint): " << kn.get_endpoint();
  std::cout << "| (branch): " << branch_idx + 1;
  std::cout << "| (state): ";
  if( ret == k_bucket::update_state::added_back ) std::cout << "\x1b[32m" << "add back" << "\x1b[39m" << "\n";
  else if( ret == k_bucket::update_state::moved_back ) std::cout << "\x1b[34m" << "move back" << "\x1b[39m" << "\n";
  else if( ret == k_bucket::update_state::not_found ) std::cout << "not found" << "\n";
  else if ( ret == k_bucket::update_state::overflow ) std::cout << "\x1b[31m" << "overflow" << "\x1b[39m" << "\n";
  else std::cout << "error" << "\n";
  */  
  #endif

  return ret;
}

std::vector<k_node> k_routing_table::get_node_front( k_node& root_node, std::size_t count, const std::vector<k_node> &ignore_nodes )
{
   k_bucket &target_bucket = this->get_bucket( root_node );
  return target_bucket.get_node_front( count, ignore_nodes );
}

std::vector<k_node> k_routing_table::get_node_back( k_node& root_node, std::size_t count, const std::vector<k_node> &ignore_nodes )
{
   k_bucket &target_bucket = this->get_bucket( root_node );
  return target_bucket.get_node_back( count, ignore_nodes );
}


bool k_routing_table::is_exist( k_node &kn )
{
  k_bucket &target_bucket = this->get_bucket( kn );
  return target_bucket.is_exist(kn);
}

std::vector<k_node> k_routing_table::collect_node( k_node& root_node, std::size_t max_count, const std::vector<k_node> &ignore_nodes )
{
  std::vector<k_node> ret = std::vector<k_node>();

  int root_branch = this->calc_branch(root_node);
  std::size_t remaining_nodes_count = max_count - ret.size();
  auto &root_bucket = this->get_bucket(root_branch);

  // センター(ルート)バケットの処理
  auto nodes = root_bucket.get_node_back( remaining_nodes_count, ignore_nodes );
  ret.insert( ret.end(), nodes.begin(), nodes.end() );

  bool upper_flag = false ;
  bool lower_flag = false;
  unsigned short count = 1;

  // 上下バケットの処理
  while( max_count > ret.size() && !((upper_flag) && (lower_flag)) )
  {
	if( (root_branch-count) >= 1 )
	{
	  remaining_nodes_count = max_count - ret.size(); // 残りノード数の再計算
	  auto& upper_bucket = this->get_bucket( root_branch - count );
	  nodes = upper_bucket.get_node_back( remaining_nodes_count, ignore_nodes );
	  ret.insert( ret.end(), nodes.begin(), nodes.end() );
	}
	else{
	  upper_flag = true;
	}

	if( (root_branch+count) <= 160 )
	{
	  remaining_nodes_count = max_count - ret.size(); // 残りノード数の再計算
	  auto& lower_bucket = this->get_bucket( root_branch + count );
	  nodes = lower_bucket.get_node_back( remaining_nodes_count, ignore_nodes );
	  ret.insert( ret.end(), nodes.begin(), nodes.end() );
	}
	else{
	  lower_flag = true;
	}
	count++;
  }

  return ret;
}

unsigned short k_routing_table::calc_branch_index( k_node &kn )
{
  const auto branch = this->calc_branch(kn);
  return std::max( branch -1 , 0 );
}

std::size_t k_routing_table::get_node_count()
{
  std::size_t ret = 0;
  for( auto &itr : _table )
	ret += itr.get_node_count();

  return ret;
}

k_bucket& k_routing_table::get_bucket( k_node &kn )
{
  const auto branch_idx = calc_branch_index(kn);
  return (_table[branch_idx]);
}

k_bucket& k_routing_table::get_bucket( unsigned short branch )
{
  return (_table[ std::max(branch-1,0) ]);
}

k_bucket_iterator k_routing_table::get_begin_bucket_iterator()
{
  k_bucket_iterator ret( this, _table.begin(), 1/*branchのスタートは1から*/ );
  return ret;
}

void k_routing_table::print( int start_branch )
{
  for( int i=start_branch; i<=_table.size(); i++ )
  {
	auto &target_bucket = this->get_bucket(i);
	for( int i=0; i< get_console_width(); i++ ) printf("=");
	std::cout << "| k-bucket (" << i << ") ";
	std::cout << "| c: " << target_bucket.get_node_count() << " |" << "\n";
	for( int i=0; i<get_console_width(); i++ ) printf("-");
	target_bucket.print_horizontal();
	std::cout << "\n";
  }
}


std::vector<k_node> eps_to_k_nodds( std::vector<ip::udp::endpoint> eps )
{
  std::vector<k_node> ret;
  for( auto itr : eps ) ret.push_back( k_node(itr) );
  return ret;
}


k_bucket_iterator::k_bucket_iterator()
{
  return;
}

k_bucket_iterator::k_bucket_iterator( k_routing_table *routing_table, k_routing_table::routing_table::iterator bucket_itr, unsigned short branch ) :
  _routing_table( routing_table )
  , _bucket_itr( bucket_itr )
  , _branch( branch )
{
  return;
}

k_bucket_iterator& k_bucket_iterator::operator++() // 前置
{
  _branch++;

  if( is_invalid() )
  {
	_branch = K_BUCKET_COUNT + 1;
	// _bucket = nullptr;
	return *this;
  }
  // _bucket = &( (_routing_table->get_bucket(_branch)) );
  _bucket_itr++;

  return *this;
}
k_bucket_iterator k_bucket_iterator::operator++(int)
{
  k_bucket_iterator ret = *this;
  _branch++;

  if( is_invalid() )
  {
	_branch = K_BUCKET_COUNT + 1;
	// _bucket = nullptr;
	return ret;
  }
  // _bucket = &(_routing_table->get_bucket(_branch));
  _bucket_itr++;

  return ret;
}

k_bucket_iterator& k_bucket_iterator::operator--()
{
  _branch--;

  if( is_invalid() )
  {
	_branch = 0;
	// _bucket = nullptr;
	return *this;
  }
  // _bucket = &(_routing_table->get_bucket(_branch));
  _bucket_itr++;

  return *this;
}
k_bucket_iterator k_bucket_iterator::operator--(int)
{
  k_bucket_iterator ret = *this;
  _branch--;

  if( is_invalid() )
  {
	_branch = 0;
	// _bucket = nullptr;
	return ret;
  }
  // _bucket = &(_routing_table)->get_bucket(_branch);
  _bucket_itr++;

  return ret;
}
k_bucket& k_bucket_iterator::operator*()
{
  return (*_bucket_itr);
}

k_bucket& k_bucket_iterator::get_raw()
{
  return *_bucket_itr;
}

bool k_bucket_iterator::is_invalid() const
{
  if( _branch <= 0 || _branch >= 161 ) return true;
  return false;
}

k_bucket_iterator& k_bucket_iterator::to_begin()
{
  std::cout << "エラー出るかも" << "\n";
  std::advance( _bucket_itr, (-_branch-1) );
  _branch = 1;
  // _bucket = &((_routing_table)->get_bucket(_branch));
  return *this;
}

void k_bucket_iterator::_print_() const
{
  std::cout << "branch :: " << _branch << "\n";
  // if( _bucket == nullptr ) std::cout << "bucket :: nullptr" << "\n";
  // else printf("bucket :: %p\n", _bucket );
  std::cout << "node counts :: " << (*_bucket_itr).get_node_count() << "\n";
  std::cout << "is_invalid :: " << is_invalid() << "\n";
}

unsigned short k_bucket_iterator::get_branch()
{
  return _branch;
}

k_bucket_iterator k_bucket_iterator::invalid()
{
  k_bucket_iterator ret;
  return ret;
}

std::vector<k_node> k_bucket_iterator::get_nodes()
{
  if( is_invalid() ) return std::vector<k_node>();
  return (*_bucket_itr).get_nodes();
}

/* k_bucket* k_bucket_iterator::get_bucket()
{
  return _bucket;
} */


};
};
