#ifndef FCA0EE09_D238_46EA_98BE_624F13DD8DAD
#define FCA0EE09_D238_46EA_98BE_624F13DD8DAD


#include <memory>
#include <chrono>

#include <chain/transaction/p2pkh/p2pkh.h>
#include <chain/transaction/tx.hpp>


namespace chain
{


class mempool_entry
{
public:
  using mempool_entry_ref = std::reference_wrapper<mempool_entry>;

private:
  const tx::transaction_ref _tx_ref;
 
  tx::tx_id _id;

  struct meta
  {
	std::uint32_t _fee;
	std::uint32_t _mem_usage_size; // バイト数
	std::time_t _entryed_at; // 作成された時間(mempoolに挿入された時間の方がいいのでは)
	
	meta( std::uint32_t fee, std::uint32_t bytes, std::time_t entryed_at );
  } _meta;

public:
  mempool_entry( const tx::transaction_ref &tx_ref, std::uint32_t fee, std::uint32_t bytes, std::time_t entryed_at );

  tx::transaction_ref get_tx(); // 本体txの取得

  tx::tx_id get_id();
  std::chrono::seconds get_entryed_at() const;
  std::size_t get_entry_size_bytes() const; // エントリー(もちろんtxも含む)のサイズ取得
  std::size_t get_tx_size_bytes() const; // txのサイズ取得
  std::size_t get_tx_fee() const;
};


}; // namespace chain

#endif 


// reference : bitcoin core(mempool.h)
// reference : bitcoin core(mempool_entry.h)
// # https://github.com/bitcoin/bitcoin