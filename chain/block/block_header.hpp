#ifndef FCF438B5_08C1_474D_8EA2_1B37D8544FEA
#define FCF438B5_08C1_474D_8EA2_1B37D8544FEA


#include <array>
#include <memory>
#include <algorithm>
#include <vector>
#include <chain/block/block.params.hpp>


namespace chain
{


class block_header
{
public:
  using header_hash = BLOCK_HEADER_HASH;
  using merkle_root = MERKLE_ROOT;
  using ref = std::shared_ptr<block_header>;

  std::uint32_t get_time() const;
  std::uint32_t get_nonce() const;
  header_hash get_header_hash() const;
  header_hash get_prev_block_hash() const;
  std::vector<std::uint8_t> export_to_binary() const; 
  
  void increment_nonce(); // nonceを1インクリメントする
  bool check_nonce() const; // マイニング用(_target_nonceと_meta._nonceを比較する)
  std::vector<std::uint8_t> self_hash256() const;

  block_header();
protected:
  #if L1_DEBUG
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

  std::uint32_t _target_nonce; // マイニング用
};


};


#endif


