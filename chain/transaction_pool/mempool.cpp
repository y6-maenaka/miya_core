#include <chain/transaction_pool/mempool.hpp>
#include <chain/transaction/p2pkh/p2pkh.h>


namespace chain
{


mempool::mempool( io_context &io_ctx ) : 
  _io_ctx( io_ctx )
  , _refresh_timer( io_ctx )
{ 
  return;
}

mempool::indexed_tx_set::iterator mempool::add_entry( const mempool_entry &entry )
{
  mempool::indexed_tx_set::iterator ret_itr = _indexed_tx_set.insert(entry).first;

  _total_mem_usage_bytes += ret_itr->get_entry_size_bytes();

  _total_tx_size_bytes += ret_itr->get_tx_size_bytes();
  _total_fee = ret_itr->get_tx_fee();

  return ret_itr;
}

void mempool::remove_entry( const struct tx::P2PKH &target )
{
  auto target_id = target.get_id();
  auto target_itr = _indexed_tx_set.find(target_id);

  _total_mem_usage_bytes -= target_itr->get_entry_size_bytes();

  _total_tx_size_bytes -= target_itr->get_tx_size_bytes();
  _total_fee -= target_itr->get_tx_fee();

  _indexed_tx_set.erase( target_id );
}

std::size_t mempool::get_tx_count() const
{
  return _indexed_tx_set.size();
}


};
