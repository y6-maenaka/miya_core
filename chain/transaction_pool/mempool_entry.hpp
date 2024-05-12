#ifndef FCA0EE09_D238_46EA_98BE_624F13DD8DAD
#define FCA0EE09_D238_46EA_98BE_624F13DD8DAD


#include <memory>

#include <chain/transaction/p2pkh/p2pkh.h>


namespace chain
{


class mempool_entry
{
private:
  std::reference_wrapper<const struct P2PKH> _rtx;
 
  tx::tx_id _id;

  struct meta
  {
	std::uint32_t _fee;
	std::uint32_t _bytes; // 本構造体パラメータも含むサイズ
	std::time_t _entryed_at; // 作成された時間(mempoolに挿入された時間の方がいいのでは)
	
	meta( std::uint32_t fee, std::uint32_t bytes, std::time_t entryed_at );
  } _meta;

public:
  mempool_entry( const struct P2PKH &tx, std::uint32_t fee, std::uint32_t bytes, std::time_t entryed_at );

  struct P2PKH &get_tx();
  tx::tx_id get_id();
};


}; // namespace chain

#endif 


// reference : bitcoin core(mempool_entry.h)
