#include <iostream>
#include <memory>

#include "./common.h"

#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"
#include "../../shared_components/cipher/ecdsa_manager.h"
#include "../../shared_components/hash/sha_hash.h"

#include "../../miya_chain/utxo_set/utxo.h"
#include "../../miya_chain/utxo_set/utxo_set.h"

#include "../../miya_chain/miya_chain_manager.h"

#include "../../miya_core/miya_core.hpp"
#include "../../control_interface/control_interface.h"

#include "../../miya_chain/transaction/p2pkh/p2pkh.h"
#include "../../miya_chain/transaction/tx/tx_in.h"
#include "../../miya_chain/transaction/tx/tx_out.h"
#include "../../miya_chain/transaction/coinbase/coinbase.h"

#include "../../miya_chain/block/block.h"
#include "../../miya_chain/block/block_header.h"

#include "../../miya_chain/mining/simple_mining.h"
#include "../../miya_chain/miya_chain_manager.h"
#include "../../miya_chain/miya_coin/local_strage_manager.h"
#include "../../miya_chain/block_chain_iterator/block_chain_iterator.h"


int build_sample_chain_p1
(
  std::shared_ptr<miya_core::MiyaCore> miyaCore,
  std::shared_ptr<ControlInterface> controlInterface,
  std::shared_ptr<cipher::ECDSAManager> ecdsaManager,
  std::shared_ptr<miya_chain::MiyaChainManager> miyaChainManager
)
{
  std::shared_ptr<unsigned char> selfAddress; size_t selfAddressLength;
  std::shared_ptr<unsigned char> pubKeyHash; size_t pubKeyHashLength;
  EVP_PKEY* pkey = ecdsaManager->myPkey();
  selfAddressLength = cipher::ECDSAManager::toRawPubKey( pkey, &selfAddress );
  pubKeyHashLength = hash::SHAHash( selfAddress , selfAddressLength , &pubKeyHash , "sha1" );





  std::shared_ptr<tx::P2PKH> p2pkh_0000 = nullptr;
  auto created_p2pkh = CreateTxFromJsonFile( "../control_interface/tx_origin/payment_genesis.json", controlInterface , ecdsaManager );
  if( created_p2pkh.first ) p2pkh_0000 = created_p2pkh.second;
  std::shared_ptr<tx::Coinbase> coinbase_0000 = CreateCoinbase( 0 ,"HelloWorld", pubKeyHash , miyaCore->context() );

  std::shared_ptr<block::Block> block_0000 = std::shared_ptr<block::Block>( new block::Block{} );
  block_0000->coinbase( coinbase_0000 );
  block_0000->add( p2pkh_0000 );

  std::shared_ptr<unsigned char> merkleRoot_0000; size_t merkleRootLength_0000;
  merkleRootLength_0000 = block_0000->calcMerkleRoot( &merkleRoot_0000 );
  block_0000->header()->merkleRoot( merkleRoot_0000 );

  uint32_t nBits_0000 = 532390001;
  block_0000->header()->nBits( nBits_0000 );
  block_0000->header()->prevBlockHash( nullptr );
  uint32_t nonce_0000 = miya_chain::simpleMining( nBits_0000 , block_0000->header(), false );
  block_0000->header()->nonce( nonce_0000 );

  std::shared_ptr<unsigned char> blockHash_0000;
  block_0000->blockHash( &blockHash_0000 );






  std::shared_ptr<tx::P2PKH> p2pkh_0001 = nullptr;
  created_p2pkh = CreateTxFromJsonFile("../control_interface/tx_origin/payment_tx_info_0001.json", controlInterface , ecdsaManager );
  if( created_p2pkh.first ) p2pkh_0001 = created_p2pkh.second;
  std::shared_ptr<tx::Coinbase> coinbase_0001 = CreateCoinbase(1, "HelloWorld1", pubKeyHash , miyaCore->context());

  std::shared_ptr<block::Block> block_0001 = std::shared_ptr<block::Block>( new block::Block{} );
  block_0001->coinbase( coinbase_0001 );
  block_0001->add( p2pkh_0001 );

  std::shared_ptr<unsigned char> merkleRoot_0001; size_t merkleRootLength_0001;
  merkleRootLength_0001 = block_0001->calcMerkleRoot( &merkleRoot_0001 );
  block_0001->header()->merkleRoot( merkleRoot_0001 );

  uint32_t nBits_0001 = 532390001;
  block_0001->header()->nBits( nBits_0001 );
  block_0001->header()->prevBlockHash( blockHash_0000 );
  uint32_t nonce_0001 = miya_chain::simpleMining( nBits_0001 , block_0001->header(), false );
  block_0001->header()->nonce( nonce_0001 );

  std::shared_ptr<unsigned char> blockHash_0001;
  block_0001->blockHash( &blockHash_0001 );






  std::shared_ptr<tx::P2PKH> p2pkh_0002 = nullptr;
  created_p2pkh = CreateTxFromJsonFile("../control_interface/tx_origin/payment_tx_info_0002.json", controlInterface , ecdsaManager );
  if( created_p2pkh.first ) p2pkh_0002 = created_p2pkh.second;
  std::shared_ptr<tx::Coinbase> coinbase_0002 = CreateCoinbase( 2, "HelloWorld2", pubKeyHash , miyaCore->context());

  std::shared_ptr<block::Block> block_0002 = std::shared_ptr<block::Block>( new block::Block{} );
  block_0002->coinbase( coinbase_0002 );
  block_0002->add( p2pkh_0002 );

  std::shared_ptr<unsigned char> merkleRoot_0002; size_t merkleRootLength_0002;
  merkleRootLength_0002 = block_0002->calcMerkleRoot( &merkleRoot_0002 );
  block_0002->header()->merkleRoot( merkleRoot_0002 );

  uint32_t nBits_0002 = 532390001;
  block_0002->header()->nBits( nBits_0002 );
  block_0002->header()->prevBlockHash( blockHash_0001 );
  uint32_t nonce_0002 = miya_chain::simpleMining( nBits_0002 , block_0002->header(), false );
  block_0002->header()->nonce( nonce_0002 );

  std::shared_ptr<unsigned char> blockHash_0002;
  block_0002->blockHash( &blockHash_0002 );






std::shared_ptr<miya_chain::BlockLocalStrageManager> localStrageManager;
  localStrageManager = miyaChainManager->localStrageManager();

  std::shared_ptr<miya_chain::MiyaChainState> chainState = miyaChainManager->chainState();




  auto writedUTxOVector = localStrageManager->writeBlock( block_0000 );
  std::cout << "(1) Block Writed >> "; block_0000->__printBlockHash();
  writedUTxOVector = localStrageManager->writeBlock( block_0001 );
  std::cout << "(2) Block Writed >> "; block_0001->__printBlockHash();
  writedUTxOVector = localStrageManager->writeBlock( block_0002 );
  std::cout << "(3) Block Writed >> "; block_0002->__printBlockHash();

  chainState->update( blockHash_0002, 2 );


  std::cout << "Build Sampel Chain Pattern1 Done" << "\n";



  return 0;
}
