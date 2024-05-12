#include <chain/transaction_pool/mempool_entry.hpp>


namespace chain
{


mempool_entry::meta::meta( std::uint32_t fee, std::uint32_t bytes, std::time_t entryed_at ) :
  _fee(fee)
  , _bytes(bytes)
  , _entryed_at(entryed_at)
{
  return;
}

mempool_entry::mempool_entry( const struct P2PKH &tx, std::uint32_t fee, std::uint32_t bytes, std::time_t entryed_at ) : 
  _rtx( std::ref(tx) )
  , _meta(fee, bytes, entryed_at)
{
  return;
}


};
