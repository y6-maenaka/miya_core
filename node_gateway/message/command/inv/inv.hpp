#ifndef D958EFE4_86C9_487B_B8B9_71C83369D194
#define D958EFE4_86C9_487B_B8B9_71C83369D194


#include <iostream>
#include <vector>
#include <memory>
#include <string>
// #include <unordered_map>
#include <map>

#include <chain/block/block.params.hpp>

// https://en.bitcoin.it/wiki/Protocol_documentation#inv

namespace chain
{


// アクセスフリー
struct inv : public std::enable_shared_from_this<struct inv>
{
  using ref = std::shared_ptr<struct inv>;
  enum class type_id
  {
	MSG_TX = 0
	  , MSG_BLOCK

	  , None // 無設定
  };

  const type_id id = type_id::None;
  const BASE_ID hash; 
  ref to_ref();

  // inv() = default;
  inv( std::pair< type_id, BASE_ID> from );
  inv( const inv::type_id &id_from, const std::array< std::uint8_t, 32 > &hash_from );

  struct Hash;
};

struct inv::Hash
{
  typedef std::size_t result_type;
  std::size_t operator()( const inv::type_id &id ) const;
  bool operator()( const inv::type_id &id_1, const inv::type_id &id_2 ) const;
};

inline std::size_t inv::Hash::operator()( const inv::type_id &id ) const
{
  return static_cast<std::size_t>(id);
}

inline bool inv::Hash::operator()( const inv::type_id &id_1, const inv::type_id &id_2 ) const 
{
  return true;
}

inv::inv( std::pair< inv::type_id, BASE_ID> from ) : 
  id( from.first )
  , hash( from.second )
{
  return;
}


struct MiyaCoreMSG_INV
{
  // using inv_ref_vector = std::vector< inv::ref >;
  // inv_ref_vector _invs;
  using inv_ref_map = std::multimap< inv::type_id, inv::ref, inv::Hash >;
  inv_ref_map _invs;

protected:
  void add( struct inv target );

public:
  // using iterator = inv_ref_vector::iterator;
  using iterator = inv_ref_map::iterator;
  iterator (begin)();
  iterator (end)();
  iterator (itr)();

  static constexpr char command[12] = "inv";

  size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );

  /* Getter */
  size_t count();

  /* Setter */
  void count( size_t count );
  void addTx( std::shared_ptr<unsigned char> hash );
  void addBlock( std::shared_ptr<unsigned char> hash );

  std::vector<inv::ref> pick_inv_by_key( const inv::type_id &id_as_key );

  std::vector<struct inv> invVector();
  void __print();

  std::vector< std::uint8_t > export_to_binary() const;
};



};


#endif // D958EFE4_86C9_487B_B8B9_71C83369D194
