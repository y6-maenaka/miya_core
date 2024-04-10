#ifndef A75EEE80_3058_412B_9609_FE7A8B2E7AAC
#define A75EEE80_3058_412B_9609_FE7A8B2E7AAC


#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"
#include "../../shared_components/cipher/ecdsa_manager.h"

#include "../../miya_chain/utxo_set/utxo.h"
#include "../../miya_chain/utxo_set/utxo_set.h"

#include "../../miya_chain/miya_chain_manager.h"

#include "../../miya_core/miya_core.h"
#include "../../control_interface/control_interface.h"

#include "../../miya_chain/transaction/p2pkh/p2pkh.h"
#include "../../miya_chain/transaction/tx/tx_in.h"
#include "../../miya_chain/transaction/tx/tx_out.h"
#include "../../miya_chain/transaction/coinbase/coinbase.h"



namespace miya_core
{
  class MiyaCore;
  struct MiyaCoreContext;
};
namespace miya_chain
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
                      std::shared_ptr<miya_core::MiyaCore> *miyaCore ,
                      std::shared_ptr<ControlInterface> *controlInterface,
                      std::shared_ptr<cipher::ECDSAManager> *ecdsaManager ,
                      std::shared_ptr<miya_chain::MiyaChainManager> *miyaChainManager
);


std::pair< bool , std::shared_ptr<tx::P2PKH> > CreateTxFromJsonFile( std::string pathToJson , std::shared_ptr<ControlInterface> controlInterface ,std::shared_ptr<cipher::ECDSAManager> ecdsaManager );
std::shared_ptr<tx::Coinbase> CreateCoinbase( unsigned int height ,std::string coinbaseText , std::shared_ptr<unsigned char> pubKeyHash , const std::shared_ptr<miya_core::MiyaCoreContext> mcContext );

int build_sample_chain_p1( std::shared_ptr<miya_core::MiyaCore> miyaCore ,std::shared_ptr<ControlInterface> controlInterface,std::shared_ptr<cipher::ECDSAManager> ecdsaManager ,std::shared_ptr<miya_chain::MiyaChainManager> miyaChainManager );


#endif //  A75EEE80_3058_412B_9609_FE7A8B2E7AAC
