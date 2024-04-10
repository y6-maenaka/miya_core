#ifndef AF9B0283_5634_4772_8A61_13FD01658E4E
#define AF9B0283_5634_4772_8A61_13FD01658E4E


#include <vector>
#include <memory>
#include <shared_mutex>
#include <condition_variable>
#include <algorithm>

#include "./k_node.hpp"


namespace ss
{
namespace kademlia
{


constexpr unsigned short DEFAULT_K = 4;


class k_bucket
{
private:
  // std::shared_mutex _smtx;

  std::vector< k_node > _nodes;

public:
  k_bucket();

  enum update_state
  {
	added_back , 
	moved_back ,
	not_found ,
	overflow ,
	error
  };
  update_state auto_update( k_node kn );

  std::vector<k_node> get_node_front( std::size_t count = 1, const std::vector<k_node> &ignore_nodes = std::vector<k_node>() );
  std::vector<k_node> get_node_back( std::size_t count = 1, const std::vector<k_node> &ignore_nodes = std::vector<k_node>() );
  std::vector<k_node> get_nodes(); // 全てのノードを取得する

  void add_back( k_node kn );
  void move_back( k_node &kn );
  void delete_node( k_node &kn ); // 基本的に使用しない
  void swap_node( k_node &node_src, k_node node_dest );
  bool is_exist( k_node &kn ) const;
  bool is_full() const;
  std::size_t get_node_count() const;
  // k_bucket& operator=( const k_bucket &bucket );

  void print_vertical() const; // 縦表示
  void print_horizontal() const; // 横表示
};

  
};
};


#endif


