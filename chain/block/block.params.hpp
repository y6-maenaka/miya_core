#ifndef C72396FE_215D_4575_8AE0_7126D224E120
#define C72396FE_215D_4575_8AE0_7126D224E120


namespace chain
{


constexpr unsigned int BLOCK_HASH_BITS_LENGTH = 256;
constexpr unsigned int BLOCK_HASH_BYTES_LENGTH = BLOCK_HASH_BITS_LENGTH / 8;
constexpr unsigned int MERKLE_ROOT_BITS_LENGTH = 256;

using BLOCK_HEADER_HASH = std::array< std::uint8_t, BLOCK_HASH_BITS_LENGTH/8>;
using BLOCK_ID = BLOCK_HEADER_HASH;
using BASE_ID = BLOCK_ID;
using MERKLE_ROOT = std::array< std::uint8_t, MERKLE_ROOT_BITS_LENGTH/8>;


};


#endif
