#ifndef FCF438B5_08C1_474D_8EA2_1B37D8544FEA
#define FCF438B5_08C1_474D_8EA2_1B37D8544FEA


#include <array>
#include <memory>
#include <algorithm>
#include <crypto_utils/w_sha/sha.hpp>


namespace chain
{


constexpr unsigned int BLOCK_HASH_BITS_LENGTH = 256;
constexpr unsigned int BLOCK_HASH_BYTES_LENGTH = BLOCK_HASH_BITS_LENGTH / 8;


class block_header
{
public:
  using header_hash = std::array< std::uint8_t, BLOCK_HASH_BITS_LENGTH/8>;
  using merkle_root = std::array<std::uint8_t, MERKLE_ROOT_BITS_LENGTH/8>;

  std::uint32_t get_time() const;
  std::uint32_t get_nonce() const;
  header_hash get_header_hash() const;
  std::vector<std::uint8_t> export_to_binary() const;

protected:
  #if DEBUG
  void print_binary() const;
  #endif

private:
  struct meta // target of hash
  {
	std::int32_t _version;
	header_hash _prev_block_hash;
	merkle_root _merkl_root;
	std::uint32_t _time;
	std::uint32_t _n_bits;
	std::uint32_t _nonce;
  } _meta;
 

};




};


#endif

