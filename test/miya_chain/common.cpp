#include "common.h"

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

/*
 本テストプログラム実行前に必ず,関連データベースファイルは削除しておくこと
*/


void LaunchMiyaChain(
                      std::shared_ptr<miya_core::MiyaCore> *miyaCore ,
                      std::shared_ptr<ControlInterface> *controlInterface,
                      std::shared_ptr<cipher::ECDSAManager> *ecdsaManager ,
                      std::shared_ptr<miya_chain::MiyaChainManager> *miyaChainManager
)
{
  std::cout << "LAUNCH MIYA CHAIN" << "\n";

  *miyaCore = std::shared_ptr<miya_core::MiyaCore>( new miya_core::MiyaCore{} );
  *controlInterface = std::shared_ptr<ControlInterface>( new ControlInterface{} );

  *ecdsaManager = std::shared_ptr<cipher::ECDSAManager>( new cipher::ECDSAManager{} );
  std::shared_ptr<unsigned char> pemPass; size_t pemPassLength;
  pemPassLength = (*miyaCore)->context()->pemPass( &pemPass );
  (*ecdsaManager)->init( pemPass.get() , pemPassLength );

  *miyaChainManager = std::shared_ptr<miya_chain::MiyaChainManager>( new miya_chain::MiyaChainManager{} );
  std::shared_ptr<StreamBufferContainer> toEKP2PBrokerDummySBC = std::make_shared<StreamBufferContainer>();
  (*miyaChainManager)->init( toEKP2PBrokerDummySBC );

  std::shared_ptr<miya_chain::BlockLocalStrageManager> localStrageManager; // ブロック保存用のファイルマネージャー
  localStrageManager = (*miyaChainManager)->localStrageManager();

  std::shared_ptr<miya_chain::LightUTXOSet> utxoSet; // UTXOのセットアップ
  utxoSet = (*miyaChainManager)->utxoSet();

  std::shared_ptr<unsigned char> selfAddress; size_t selfAddressLength;
  EVP_PKEY* pkey = (*ecdsaManager)->myPkey();
  selfAddressLength = cipher::ECDSAManager::toRawPubKey( pkey, &selfAddress );
  std::shared_ptr<unsigned char> pubKeyHash; size_t pubKeyHashLength;
  pubKeyHashLength = hash::SHAHash( selfAddress , selfAddressLength, &pubKeyHash , "sha1" );

  for( int i=0; i<pubKeyHashLength; i++ ){
      printf("%02X", pubKeyHash.get()[i]);
  }
}


std::pair< bool , std::shared_ptr<tx::P2PKH> > CreateTxFromJsonFile( std::string pathToJson , std::shared_ptr<ControlInterface> controlInterface ,std::shared_ptr<cipher::ECDSAManager> ecdsaManager )
{
  std::shared_ptr<tx::P2PKH> ret = controlInterface->createTxFromJsonFile( pathToJson.c_str() );
  if( ret == nullptr ) return std::make_pair( false , nullptr );
  for( auto itr : ret->ins() ){
    itr->pkey( ecdsaManager->myPkey() );
  }
  ret->sign();
  return std::make_pair( ret->sign(), ret );
}


std::shared_ptr<tx::Coinbase> CreateCoinbase( unsigned int height ,std::string coinbaseText ,std::shared_ptr<unsigned char> pubKeyHash , const std::shared_ptr<miya_core::MiyaCoreContext> mcContext )
{
  std::shared_ptr<unsigned char> text = std::shared_ptr<unsigned char>( new unsigned char[coinbaseText.size()] ); size_t textLength = coinbaseText.size();
  memcpy( text.get(), coinbaseText.c_str(), coinbaseText.size() );
  std::shared_ptr<tx::Coinbase> ret = std::shared_ptr<tx::Coinbase>( new tx::Coinbase( height , text , textLength, pubKeyHash , mcContext ) );
  return ret;
}
