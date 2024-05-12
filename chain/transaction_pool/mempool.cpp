#include <chain/transaction_pool/mempool.hpp>


namespace chain
{


mempool::indexed_tx_set::iterator mempool::add_entry( const mempool_entry &entry )
{
  mempool::indexed_tx_set::iterator ret_itr = _indexed_tx_set.insert(entry).first;

  return ret_itr;
}


};
