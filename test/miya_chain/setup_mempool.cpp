#include <chain/transaction_pool/mempool.hpp>
#include "boost/asio.hpp"


int setup_mempool()
{

  boost::asio::io_context io_ctx;

  chain::mempool mpool( io_ctx );
 

  std::cout << mpool.get_tx_count() << "\n";


  return 10;
}
