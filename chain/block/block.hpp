#ifndef B5C82F86_2840_49C2_A60D_B47ECCF7B547
#define B5C82F86_2840_49C2_A60D_B47ECCF7B547


#include <array>
#include <vector>
#include <memory>

#include <chain/block/block.params.hpp>
#include <chain/block/block_header.hpp>
#include <chain/transaction/p2pkh/p2pkh.h>
#include <chain/transaction/coinbase/coinbase.hpp>


namespace chain
{


struct block // 今後はこっちを使う
{
public:
  using ref = std::shared_ptr<struct block>;
  using block_hash = block_header::header_hash;
  using id = block_hash;
  static constexpr std::size_t id_length = 32; // 仮

  block_hash get_block_hash() const; 
  block_hash get_prev_block_hash() const;
  id get_id() const; // alias
  id get_prev_block_id() const; // alias
  std::vector<std::uint8_t> export_to_binary();

  bool check_pow() const; // 重要(ブロックハッシュの検証)
protected:
  // std::vector<std::uint8_t> export_txs_to_binary() const;
private:
  class block_header _header;
  std::vector< struct tx::P2PKH > _tx_v; // transactions
  // struct tx::coinbase _coinbase; // 後で修正
};


};


#endif 


