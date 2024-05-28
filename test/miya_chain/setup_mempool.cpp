#include <chain/transaction_pool/mempool.hpp>
#include <chain/transaction/p2pkh/p2pkh.h>
#include <chain/transaction/tx.hpp>
#include "boost/asio.hpp"
#include "./common.h"
#include "../../core/core.hpp"
#include <control_interface/control_interface.h>
#include <cipher/ecdsa_manager.h>
#include <memory>


int setup_mempool()
{

  boost::asio::io_context io_ctx;
  chain::mempool mpool( io_ctx );

  std::cout << "mempool tx count :: " << mpool.get_tx_count() << "\n";

  return 10;
}
