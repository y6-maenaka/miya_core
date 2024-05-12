#ifndef BEDE4272_AF8C_44BE_9889_F0D66C788B33
#define BEDE4272_AF8C_44BE_9889_F0D66C788B33


#include <vector>


inline std::vector<std::uint8_t> convert_uint32_to_vector_uint8( const std::uint32_t &from )
{
  return std::vector<std::uint8_t>( (std::uint8_t*)(&from), (std::uint8_t*)(&from) );
}

inline std::size_t vector_uint8_to_size_t( const std::vector<std::uint8_t> &from ){
  return 10;
}


#endif
