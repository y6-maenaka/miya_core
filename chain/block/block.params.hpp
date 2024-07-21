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


template < std::size_t N = BASE_ID_BYTES_LENGTH > class base_id
{
public:
  using ret = std::shared_ptr<base_id>;
  using value_type = std::uint8_t;

  template< typename Container > base_id( const Container &container );
  base_id( void* from, std::size_t from_size );
  base_id();
  
  std::string to_string() const;
  template < typename T > std::vector<T> to_vector() const;
  template < typename T, std::size_t M > std::array<T, M> to_array() const;

  void fill( unsigned char hex );

  static base_id<N> none();
  bool operator == ( const base_id<N> &id );

private:
  std::array< value_type, N > _body;
};

template < std::size_t N > template < typename Container > base_id<N>::base_id( const Container &container ) : 
  _body{}
{
  std::copy_n( container.begin(), std::min( container.size(), N ), _body.begin() );
}

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

template < std::size_t N > base_id<N>::fill( unsigned char hex )
{
  std::fill( _body.begin(), _body.end(), hex );
}

template < std::size_t N > class union_id : public base_id<N>
{
  // as alias
}; 

class transaction_id : public base_id<TX_ID_BITS_LENGTH>
{
  // as alias
};

class block_id : public base_id<BLOCK_HASH_BYTES_LENGTH>
{
  // as alias
  static block_id (invalid)();
};


};


#endif
