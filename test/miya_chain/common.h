#ifndef A75EEE80_3058_412B_9609_FE7A8B2E7AAC
#define A75EEE80_3058_412B_9609_FE7A8B2E7AAC


#include <iostream>
#include <memory>
#include <string>
#include <vector>


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
