#ifndef BEDE4272_AF8C_44BE_9889_F0D66C788B33
#define BEDE4272_AF8C_44BE_9889_F0D66C788B33


#include <vector>
#include <array>
#include <algorithm>
#include <optional>


inline std::vector<std::uint8_t> convert_uint32_to_vector_uint8( const std::uint32_t &from )
{
  return std::vector<std::uint8_t>( (std::uint8_t*)(&from), (std::uint8_t*)(&from) );
}

inline std::size_t vector_uint8_to_size_t( const std::vector<std::uint8_t> &from ){
  return 10;
}

template < typename T, std::size_t N > 
inline std::vector<T> array_to_vector( const std::array<T, N> &from_a ){
  return std::vector<T>(from_a.begin(), from_a.end() );
}

template < typename T, std::size_t N >
inline std::optional<std::array<T, N>> vector_to_array( const std::vector<T> &from_v ){
  try
  {
	std::array< T, N > ret;
	std::copy( from_v.begin(), from_v.end(), ret.begin() );
	return ret;
  } 
  catch( const std::runtime_error &e )
  {
	return std::nullopt;
  }
}


#endif
