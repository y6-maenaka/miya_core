#ifndef D958EFE4_86C9_487B_B8B9_71C83369D194
#define D958EFE4_86C9_487B_B8B9_71C83369D194


#include <iostream>
#include <vector>
#include <memory>
#include <string>
// #include <unordered_map>
#include <map>

#include <chain/block/block.params.hpp>
#include <chain/block/block.hpp>
#include <node_gateway/message/command/command.params.hpp>

// https://en.bitcoin.it/wiki/Protocol_documentation#inv

namespace chain
{


// アクセスフリー
struct inv : public std::enable_shared_from_this<struct inv>
{
  using ref = std::shared_ptr<struct inv>;
  enum class type_id
  {
	MSG_TX = static_cast<int>(COMMAND_TYPE_ID::MSG_TX)
	  , MSG_BLOCK = static_cast<int>(COMMAND_TYPE_ID::MSG_BLOCK)

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


inv::inv( const inv::type_id &id_from, const std::array< std::uint8_t, 32 > &hash_from ) : 
  hash( hash_from )
  , id( id_from )
{
  return;
}

inv::ref inv::to_ref()
{
  return shared_from_this();
}

size_t MiyaCoreMSG_INV::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
  /*
    size_t retRawLength = sizeof(_body._count) + (sizeof(struct inv) * _body._invVector.size() );
    *retRaw = std::shared_ptr<unsigned char>( new unsigned char[retRawLength] );
    this->count( _body._invVector.size() ); // inv数の最終更新

    size_t formatPtr = 0;
    // countの書き出し
    memcpy( (*retRaw).get() + formatPtr , &(_body._count), sizeof(_body._count) ); formatPtr += sizeof(_body._count);

    // 各invの書き出し
    for( auto itr : _body._invVector ){
        memcpy( (*retRaw).get() + formatPtr , &itr, sizeof(itr) ); formatPtr += sizeof(itr);
    }

    return formatPtr;
	*/
}

void MiyaCoreMSG_INV::add( struct inv target )
{
  /*
    _body._invVector.push_back( target );
    this->count( this->count() + 1 );
	*/
}

size_t MiyaCoreMSG_INV::count()
{
  /*
    return static_cast<size_t>(_body._count);
	*/
}

void MiyaCoreMSG_INV::__print()
{
  /*
    printf(" .... %s .... \n" , MiyaCoreMSG_INV::command );

    int i=0;
    for( int i=0; i<_body._invVector.size(); i++ ){
        std::cout << "(" << i << ") ";
        std::cout << "type :: " << _body._invVector.at(i)._typeID << "\n";
        auto _ = _body._invVector.at(i);
        for( int j=0; j<32; j++ ){
            printf("%02X", _.hash()[j] );
        } std::cout << "\n\n";
    }
	*/
}

void MiyaCoreMSG_INV::count( size_t count )
{
    // _body._count = static_cast<uint32_t>( count );
}

void MiyaCoreMSG_INV::addTx( std::shared_ptr<unsigned char> hash )
{
  /*
    struct inv target = inv( static_cast<unsigned short>(TypeID::MSG_TX), hash );
    // target._typeID = static_cast<unsigned short>(TypeID::MSG_TX);
    // target.hash( hash );
    this->add( target );
	*/
}


void MiyaCoreMSG_INV::addBlock( std::shared_ptr<unsigned char> hash )
{
  /*
    struct inv target = inv( static_cast<unsigned short>(TypeID::MSG_BLOCK) , hash );
    // target._typeID = static_cast<unsigned short>(TypeID::MSG_BLOCK);
    // target.hash( hash );
    this->add( target );
  */
}

std::vector<struct inv> MiyaCoreMSG_INV::invVector()
{
    // return _body._invVector;
}

MiyaCoreMSG_INV::iterator MiyaCoreMSG_INV::begin()
{
  return _invs.begin();
}

MiyaCoreMSG_INV::iterator MiyaCoreMSG_INV::end()
{
  return _invs.end();
  // return inv_ref_vector.end();
}

MiyaCoreMSG_INV::iterator MiyaCoreMSG_INV::itr()
{
  return _invs.begin();
}

std::vector<inv::ref> MiyaCoreMSG_INV::pick_inv_by_key( const inv::type_id &id_as_key )
{
  std::vector<inv::ref> ret;
  auto range = _invs.equal_range(id_as_key);
  for( auto itr = range.first; itr != range.second; ++itr ){
	ret.push_back(itr->second);
  }
}


};


#endif // D958EFE4_86C9_487B_B8B9_71C83369D194
