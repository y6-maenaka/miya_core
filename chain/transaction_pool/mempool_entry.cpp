#include <chain/transaction_pool/mempool_entry.hpp>


namespace chain
{


mempool_entry::meta::meta( std::uint32_t fee, std::uint32_t bytes, std::time_t entryed_at ) :
  _fee(fee)
  , _mem_usage_size(bytes)
  , _entryed_at(entryed_at)
{
  return;
}

mempool_entry::mempool_entry( const tx::transaction_ref &tx_ref, std::uint32_t fee, std::uint32_t bytes, std::time_t entryed_at ) : 
  _tx_ref( tx_ref )
  , _meta(fee, bytes, entryed_at)
{
  return;
}

std::size_t mempool_entry::get_tx_fee() const
{
  return 0; // 一旦
}

std::size_t mempool_entry::get_tx_size_bytes() const
{
  return 0; // 一旦
}

std::size_t mempool_entry::get_entry_size_bytes() const
{
  return 0; // 一旦
}


};
