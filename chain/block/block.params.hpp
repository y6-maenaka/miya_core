#ifndef C72396FE_215D_4575_8AE0_7126D224E120
#define C72396FE_215D_4575_8AE0_7126D224E120


#include <array>
#include <string>
#include <vector>


namespace chain
{


constexpr unsigned int BASE_ID_BITS_LENGTH = 256;
constexpr unsigned int BASE_ID_BYTES_LENGTH = BASE_ID_BITS_LENGTH / 8;
constexpr unsigned int BLOCK_HASH_BITS_LENGTH = 256;
constexpr unsigned int BLOCK_HASH_BYTES_LENGTH = BLOCK_HASH_BITS_LENGTH / 8;
constexpr unsigned int BLOCK_ID_BYTES_LENGTH = BLOCK_HASH_BYTES_LENGTH;
constexpr unsigned int MERKLE_ROOT_BITS_LENGTH = 256;
constexpr unsigned int TX_ID_BITS_LENGTH = 256;
constexpr unsigned int TX_ID_BYTES_LENGTH = 256 / 8;


using BLOCK_HEADER_HASH = std::array< std::uint8_t, BLOCK_HASH_BITS_LENGTH/8>;
using BLOCK_ID = BLOCK_HEADER_HASH;
using BASE_ID = BLOCK_ID;
using MERKLE_ROOT = std::array< std::uint8_t, MERKLE_ROOT_BITS_LENGTH/8>;
class block_id;
class transaction_id;


template < std::size_t N = BASE_ID_BYTES_LENGTH > class base_id
{
  friend block_id;
  friend transaction_id;
public:
  using ret = std::shared_ptr<base_id>;
  using value_type = std::uint8_t;

  base_id( const base_id<N> &from ) : _body(from._body){};
  base_id( void* from, std::size_t from_size );
  base_id();
  template< typename Container
	, typename = std::enable_if_t<
	  ( std::is_same<Container, std::vector<typename Container::value_type>>::value || std::is_same<Container, std::array<typename Container::value_type, N>>::value )
	  >
	> base_id( const Container &container )
	{
	  std::copy_n( container.begin(), std::min( container.size(), N ), _body.begin() ); 
	};

  /* template < typename T > struct is_allow_container : std::false_type{};
  template < typename T > struct is_allow_container< std::array<T, N> > : std::true_type{};
  template < typename T > struct is_allow_container< std::vector<T> > : std::true_type{};
  template < typename Container > auto check_container(Container*) -> typename std::enable_if<is_container<Container>::value, void*>::type;
  template< typename Container > base_id( const Container &container ) -> decltype(check_container<Container>(nullptr)); */
  
  std::string to_string() const;
  template < typename T > std::vector<T> to_vector() const;
  template < typename T, std::size_t M > std::array<T, M> to_array() const;

  void fill( unsigned char hex );

  static base_id<N> none();
  bool operator == ( const base_id<N> &id );

  std::array< value_type, N > get_raw_with_array(); // bodyの取り出し

protected:
  std::array< value_type, N > _body;
};

/* template < std::size_t N > template < typename Container > base_id<N>::base_id( const Container &container ) : 
  _body{}
{
  std::copy_n( container.begin(), std::min( container.size(), N ), _body.begin() );
} */

template < std::size_t N > base_id<N>::base_id( void* from, std::size_t from_size )
{
  std::memcpy( _body.data(), from, from_size );
}

template < std::size_t N > base_id<N>::base_id() 
{
  std::fill( _body.begin(), _body.end(), 0x00 );
}

template < std::size_t N > base_id<N> base_id<N>::none()
{
  return base_id<N>();
}

template < std::size_t N > bool base_id<N>::operator ==( const base_id<N> &id )
{
  return std::equal( _body.begin(), _body.end(), id._body.begin() );
}

template < std::size_t N > std::string base_id<N>::to_string() const
{
  std::string ret;
  ret.resize( N );

  std::transform( _body.begin(), _body.begin() + N, ret.begin(), []( const base_id::value_type &v ){
	return static_cast<std::string::value_type>(v);
	  });
  return ret;
}

template < std::size_t N > template < typename T > std::vector<T> base_id<N>::to_vector() const
{
  std::vector<T> ret;
  std::transform( _body.begin(), _body.begin() + _body.size(), ret.begin(), []( const base_id::value_type &v ){
	return static_cast<T>(v);
	  });
  return ret;
}

template < std::size_t N > template < typename T, std::size_t M > std::array<T, M> base_id<N>::to_array() const
{
  std::array< T, M > ret;
  std::size_t copy_size = std::min( N, M );
  
  std::transform( _body.begin(), _body.begin() + copy_size, []( const base_id::value_type &v ){
	return static_cast<T>(v);
	  });
  return ret;
}

template < std::size_t N > void base_id<N>::fill( unsigned char hex )
{
  std::fill( _body.begin(), _body.end(), hex );
}

template < std::size_t N > std::array< typename base_id<N>::value_type, N > base_id<N>::get_raw_with_array()
{
  return _body;
}



template < std::size_t N > class union_id : public base_id<N>
{
public:
  union_id<N>( block_id &id );
  union_id<N>( transaction_id &id );
  template < typename Container > union_id<N>( const Container &container ) : base_id<N>( container ){};

  class block_id to_block_id();
  class transaction_id to_transaction_id();
  // as alias
}; 

class transaction_id : public base_id<TX_ID_BYTES_LENGTH>
{
  friend union_id<TX_ID_BYTES_LENGTH>;
  public:
	transaction_id();
	transaction_id( union_id<TX_ID_BYTES_LENGTH> id_from );
  // as alias
};

class block_id : public base_id<BLOCK_HASH_BYTES_LENGTH>
{
  friend union_id<BLOCK_HASH_BYTES_LENGTH>;
  public:
	block_id();
	block_id( union_id<BLOCK_HASH_BYTES_LENGTH> id_from );
  // as alias
public: 
  static block_id (invalid)();
};

template < std::size_t N > union_id<N>::union_id( block_id &id ) 
{
  std::copy_n( id.get_raw_with_array().begin(), std::min( id.get_raw_with_array().size(), N ), this->_body.begin() ); 
}


};


#endif
