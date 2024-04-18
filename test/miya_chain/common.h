#ifndef A75EEE80_3058_412B_9609_FE7A8B2E7AAC
#define A75EEE80_3058_412B_9609_FE7A8B2E7AAC


#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../../share/stream_buffer/stream_buffer.h"
#include "../../share/stream_buffer/stream_buffer_container.h"
#include "../../share/cipher/ecdsa_manager.h"

#include "../../chain/utxo_set/utxo.h"
#include "../../chain/utxo_set/utxo_set.h"

#include "../../chain/chain_manager.h"

#include "../../core/core.hpp"
#include "../../control_interface/control_interface.h"

#include "../../chain/transaction/p2pkh/p2pkh.h"
#include "../../chain/transaction/tx/tx_in.h"
#include "../../chain/transaction/tx/tx_out.h"
#include "../../chain/transaction/coinbase/coinbase.hpp"



namespace core
{
  class MiyaCore;
  struct MiyaCoreContext;
};
namespace chain
{
  class MiyaChainManager;
};
namespace cipher
{
  class ECDSAManager;
};
namespace tx
{
    struct P2PKH;
    struct Coinbase;
}

class ControlInterface;


void LaunchMiyaChain(
                      std::shared_ptr<core::MiyaCore> *miyaCore ,
                      std::shared_ptr<ControlInterface> *controlInterface,
                      std::shared_ptr<cipher::ECDSAManager> *ecdsaManager ,
                      std::shared_ptr<chain::MiyaChainManager> *miyaChainManager
);


std::pair< bool , std::shared_ptr<tx::P2PKH> > CreateTxFromJsonFile( std::string pathToJson , std::shared_ptr<ControlInterface> controlInterface ,std::shared_ptr<cipher::ECDSAManager> ecdsaManager );
std::shared_ptr<tx::Coinbase> CreateCoinbase( unsigned int height ,std::string coinbaseText , std::shared_ptr<unsigned char> pubKeyHash , const std::shared_ptr<core::MiyaCoreContext> mcContext );

int build_sample_chain_p1( std::shared_ptr<core::MiyaCore> miyaCore ,std::shared_ptr<ControlInterface> controlInterface,std::shared_ptr<cipher::ECDSAManager> ecdsaManager ,std::shared_ptr<chain::MiyaChainManager> miyaChainManager );


#endif //  A75EEE80_3058_412B_9609_FE7A8B2E7AAC
