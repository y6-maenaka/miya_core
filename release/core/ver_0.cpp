#include "ver_0.h"

#include "../../shared_components/cipher/ecdsa_manager.h"
#include "../../shared_components/stream_buffer/test/unit_test.h"
#include "../../shared_components/json.hpp"
#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"
#include "../../shared_components/hash/sha_hash.h"

#include "../../miya_chain/block/block.h"
#include "../../miya_chain/message/__test.h"

#include "../../miya_chain/mining/simple_mining.h"

#include "../../miya_chain/transaction/tx/tx_in.h"
#include "../../miya_chain/transaction/tx/tx_out.h"
#include "../../miya_chain/transaction//tx/prev_out.h"
#include "../../miya_chain/transaction/p2pkh/p2pkh.h"
#include "../../miya_chain/transaction/coinbase/coinbase.h"
#include "../../miya_chain/transaction/coinbase/coinbase_tx_in.h"
#include "../../miya_chain/transaction/script/signature_script.h"
#include "../../miya_chain/utxo_set/utxo_set.h"
#include "../../miya_chain/utxo_set/utxo.h"

#include "../../miya_chain/transaction/txcb_table_manager/txcb_table_manager.h"
#include "../../miya_chain/transaction/txcb_table_manager/txcb_table/txcb_table.h"

#include "../../miya_chain/transaction_pool/unit_test.h"
#include "../../miya_chain/transaction_pool/transaction_pool.h"
#include "../../miya_chain/transaction_pool/txcb.h"
#include "../../miya_chain/transaction_pool/txcb_bucket.h"
#include "../../miya_chain/transaction_pool/txcb_table.h"

#include "../../miya_chain/miya_chain_manager.h"
#include "../../miya_chain/miya_coin/local_strage_manager.h"


#include "openssl/evp.h"

#include "../../ekp2p/ekp2p.h"
// #include "../../ekp2p/kademlia/k_routing_table.h"

#include "../../control_interface/control_interface.h"


#include "../../miya_db/miya_db/database_manager.h"

#include "../../miya_core/miya_core.h"
#include "../../miya_chain/virtual_chain/virtual_chain.h"

#include "../../test/miya_chain/common.cpp"
#include "../../test/miya_chain/virtual_chain_p1.cpp"
#include "../../test/miya_chain/virtual_chain_p2.cpp"
#include "../../test/miya_chain/virtual_chain_p3.cpp"
#include "../../test/miya_chain/virtual_chain_p4.cpp"
#include "../../test/miya_chain/virtual_chain_p5.cpp"
#include "../../test/miya_chain/build_sample_chain_p1.cpp"


int main()
{
	virtual_chain_p5();
	return 0;


	std::cout << " WELCOME TO MIYA COIN CLIENT [ MIYA_CORE ] " << "\n";


	/*
	miya_core::MiyaCore	miyaCore;
	cipher::ECDSAManager ecdsaManager;
	std::shared_ptr<unsigned char> pemPass; size_t pemPassLength;
	pemPassLength = miyaCore.context()->pemPass( &pemPass );
	ecdsaManager.init( pemPass.get() , pemPassLength );


	EVP_PKEY *pkey;
	pkey = ecdsaManager.myPkey();
	std::shared_ptr<unsigned char> rawPubKey; size_t rawPubKeyLength;
	rawPubKeyLength = cipher::ECDSAManager::toRawPubKey( pkey , &rawPubKey );

	std::shared_ptr<unsigned char> rawPubKeyHash; size_t rawPubKeyHashLength;
	rawPubKeyHashLength = hash::SHAHash( rawPubKey , rawPubKeyLength , &rawPubKeyHash , "sha1" );




	// ジェネシスブロックの作成
	std::shared_ptr<unsigned char> coinbaseText = std::shared_ptr<unsigned char>( new unsigned char[43] );
	memcpy( coinbaseText.get(), "(FRB) keeps policy interest rates unchanged" , 43 );
	tx::Coinbase coinbase( 0 , coinbaseText , 43 , rawPubKeyHash, miyaCore.context() );

	block::Block block;
	block.coinbase( std::make_shared<tx::Coinbase>(coinbase) );
	std::shared_ptr<unsigned char> merkleRoot; size_t merkleRootLength;
	merkleRootLength = block.calcMerkleRoot( &merkleRoot );
	block.header()->merkleRoot( merkleRoot );

	uint32_t nBits = 532390001; // 簡易的にマイニングの実行
	block.header()->nBits( nBits );

	uint32_t nonce = miya_chain::simpleMining( nBits , block.header() ,false );
	block.header()->nonce( nonce );
	block.header()->print();

	std::shared_ptr<unsigned char> blockHash; size_t blockHashLength;
	blockHashLength = block.blockHash( &blockHash );


	miya_chain::MiyaChainManager miyaChainManager;
	std::shared_ptr<StreamBufferContainer> toEKP2PBrokerDummySBC = std::make_shared<StreamBufferContainer>();
	miyaChainManager.init( toEKP2PBrokerDummySBC );


	std::shared_ptr<miya_chain::LightUTXOSet> utxoSet;
	utxoSet = miyaChainManager.utxoSet();
	std::shared_ptr<unsigned char> txID; size_t txIDLength;
	txIDLength = coinbase.calcTxID( &txID );
	utxoSet->add( coinbase.txOut() , txID , 0  );

	miyaChainManager.localStrageManager()->writeBlock( std::make_shared<block::Block>(block) );
	std::shared_ptr<block::Block> readedBlock = miyaChainManager.localStrageManager()->readBlock( blockHash );

	miyaChainManager.chainState()->update( blockHash , 0 );

	std::cout << readedBlock->header()->verify() << "\n";
	std::cout << "Done" << "\n";

	std::cout << "PrevOut :: " << "\n";
	block.coinbase()->txIn()->prevOut()->print();
	std::cout << "--------------" << "\n";
	miyaChainManager.utxoSet()->get( txID , 0  );


	std::mutex mtx;
	std::condition_variable cv;
	std::unique_lock<std::mutex> lock(mtx);
	cv.wait( lock );
	*/








	// -------- [ 必須セットアップ ] ------------------------------------------------------------------------------------------------------
	miya_core::MiyaCore miyaCore;

	ControlInterface interface;

	cipher::ECDSAManager ecdsaManager; // 暗号関係マネージャーの起動
	std::shared_ptr<unsigned char> pemPass; size_t pemPassLength;
	pemPassLength = miyaCore.context()->pemPass( &pemPass );
	ecdsaManager.init( pemPass.get() , pemPassLength ); // priKeyには鍵がかかっているので
	std::shared_ptr<unsigned char> selfAddress; size_t selfAddressLength;
	EVP_PKEY *pkey = ecdsaManager.myPkey();
	selfAddressLength = cipher::ECDSAManager::toRawPubKey( pkey , &selfAddress );


	miya_chain::MiyaChainManager miyaChainManager;
	std::shared_ptr<StreamBufferContainer> toEKP2PBrokerDummySBC = std::make_shared<StreamBufferContainer>();
	miyaChainManager.init( toEKP2PBrokerDummySBC );

	std::shared_ptr<miya_chain::BlockLocalStrageManager> localStrageManager;
	localStrageManager = miyaChainManager.localStrageManager();

	std::shared_ptr<miya_chain::LightUTXOSet> utxoSet;
	utxoSet = miyaChainManager.utxoSet();
	// ------------------------------------------------------------------------------------------------------------------



	// --------------------------------------------------------------------------------------------------------------------------
	std::cout << "\n\n\n\n\n \x1b[32m";
    printf(
        " M   M III Y   Y   A     CCCC  OOO  III  N   N\n"
        "  MM MM  I   Y Y   A A   C     O   O  I   NN  N\n"
        "  M M M  I    Y   A A A  C     O   O  I   N N N\n"
        "  M   M III   Y  A     A  CCCC  OOO  III  N   N\n"
    );
	std::cout << "\x1b[39m \n\n";
	std::cout << "[ PUBKEY ] (" << selfAddressLength << "):: ";
	for( int i=0; i<selfAddressLength; i++ )
		printf("%02X", selfAddress.get()[i] );
	std::cout << "\n";
	std::shared_ptr<unsigned char> pubKeyHash; size_t pubKeyHashLength;
	pubKeyHashLength = hash::SHAHash( selfAddress , selfAddressLength , &pubKeyHash , "sha1" );
	std::cout << "[ ADDRESS ] (" << pubKeyHashLength << ") :: " ;
	for( int i=0; i<pubKeyHashLength; i++ )
		printf("%02X", pubKeyHash.get()[i] );
	std::cout << "\n\n\n\n\n";
	// --------------------------------------------------------------------------------------------------------------------------




	std::shared_ptr<tx::P2PKH> p2pkh_0001 = interface.createTxFromJsonFile("../control_interface/tx_origin/payment_genesis.json");
	for( auto itr : p2pkh_0001->ins() ){
		itr->pkey( ecdsaManager.myPkey() );
	}
	p2pkh_0001->sign();

	std::shared_ptr<unsigned char> coinbase_0001_text = std::shared_ptr<unsigned char>( new unsigned char[10] );
	memcpy( coinbase_0001_text.get() , "HelloWorld" , 10 );
	tx::Coinbase coinbase_0001( 0 , coinbase_0001_text , 10 , selfAddress , miyaCore.context() );


	block::Block block_0001;
	block_0001.coinbase( std::make_shared<tx::Coinbase>(coinbase_0001) );
	block_0001.add( p2pkh_0001 );

	std::cout << "チェックポイント" << "\n";

	std::shared_ptr<unsigned char> merkleRoot_0001; size_t merkleRoot_0001Length;
	std::cout << "< 1 >" << "\n";
	merkleRoot_0001Length = block_0001.calcMerkleRoot( &merkleRoot_0001 );
	std::cout << "< 2 >" << "\n";
	printf("%p\n", block_0001.header().get() );
	block_0001.header()->merkleRoot( merkleRoot_0001 );
	std::cout << "< 3 >" << "\n";

	std::cout << "チェックポイント0" << "\n";
	uint32_t nBits_0001 = 532390001;
	block_0001.header()->nBits( nBits_0001 );
	block_0001.header()->prevBlockHash( nullptr );
	std::cout << "マイングbefore" << "\n";
	uint32_t nonce_0001 = miya_chain::simpleMining( nBits_0001 , block_0001.header(), false );
	std::cout << "マイニングafter" << "\n";
	block_0001.header()->nonce( nonce_0001 );

	std::shared_ptr<unsigned char> blockHash_0001;
	block_0001.blockHash( &blockHash_0001  );


	std::cout << "チェックポイント2" << "\n";



	std::shared_ptr<tx::P2PKH> p2pkh_0002 = interface.createTxFromJsonFile("../control_interface/tx_origin/payment_tx_info_0001.json");
	for( auto itr : p2pkh_0002->ins() ){
		itr->pkey( ecdsaManager.myPkey() );
	}
	p2pkh_0002->sign();
	std::shared_ptr<unsigned char> coinbase_0002_text = std::shared_ptr<unsigned char>( new unsigned char[11] );
	memcpy( coinbase_0002_text.get() , "HelloWorld2", 11 );
	tx::Coinbase coinbase_0002( 1 , coinbase_0002_text , 11 , selfAddress , miyaCore.context() );
	block::Block block_0002;
	block_0002.coinbase( std::make_shared<tx::Coinbase>(coinbase_0002) );
	block_0002.add( p2pkh_0002 );
	std::shared_ptr<unsigned char> merkleRoot_0002; size_t merkleRoot_0002Length;
	merkleRoot_0002Length = block_0002.calcMerkleRoot( &merkleRoot_0002 );
	block_0002.header()->merkleRoot( merkleRoot_0002 );
	uint32_t nBits_0002 = 532390001;
	block_0002.header()->nBits( nBits_0002 );
	block_0002.header()->prevBlockHash( blockHash_0001 );
	uint32_t nonce_0002 = miya_chain::simpleMining( nBits_0002 , block_0002.header(), false );
	block_0002.header()->nonce( nonce_0002 );

	std::shared_ptr<unsigned char> blockHash_0002;
	block_0002.blockHash( &blockHash_0002 );


	std::shared_ptr<tx::P2PKH> p2pkh_0003 = interface.createTxFromJsonFile("../control_interface/tx_origin/payment_tx_info_0002.json");
	for( auto itr : p2pkh_0003->ins() ){
		itr->pkey( ecdsaManager.myPkey() );
	}
	p2pkh_0003->sign();
	std::shared_ptr<unsigned char> coinbase_0003_text = std::shared_ptr<unsigned char>( new unsigned char[11] );
	memcpy( coinbase_0003_text.get() , "HelloWorld3", 11 );
	tx::Coinbase coinbase_0003( 2 , coinbase_0003_text , 11 , selfAddress , miyaCore.context() );
	block::Block block_0003;
	block_0003.coinbase( std::make_shared<tx::Coinbase>(coinbase_0003) );
	block_0003.add( p2pkh_0003 );
	std::shared_ptr<unsigned char> merkleRoot_0003; size_t merkleRoot_0003Length;
	merkleRoot_0003Length = block_0003.calcMerkleRoot( &merkleRoot_0003 );
	block_0003.header()->merkleRoot( merkleRoot_0003 );
	uint32_t nBits_0003 = 532390001;
	block_0003.header()->nBits( nBits_0003 );
	block_0003.header()->prevBlockHash( blockHash_0002 );
	uint32_t nonce_0003 = miya_chain::simpleMining( nBits_0003 , block_0003.header(), false );
	block_0003.header()->nonce( nonce_0003 );

	std::shared_ptr<unsigned char> blockHash_0003;
	block_0003.blockHash( &blockHash_0003 );





	/*
	auto writedUTxOVector = localStrageManager->writeBlock( std::make_shared<block::Block>(block_0001) );

	std::cout << "utxo count :: " << writedUTxOVector.size() << "\n";
	std::shared_ptr<unsigned char> searchTxID; size_t searchTxIndex;
	int i = 0;
	for( auto itr : writedUTxOVector ){
		std::cout << "*" << i << "*" << "\n";
		utxoSet->add( itr );
		searchTxID = itr->txID();
		searchTxIndex = itr->outputIndex();
		utxoSet->get( searchTxID , searchTxIndex);
		i++;
	}

	std::cout << "==============================" << "\n";

	localStrageManager->readBlock( blockHash_0001 );
	localStrageManager->readUndo( blockHash_0001 );
	*/


	/*
	std::cout << "\n\n\n\n========================================================" << "\n";


	miya_chain::BDVirtualChain virtuahChain( miyaChainManager.utxoSet() , localStrageManager ,std::make_shared<block::Block>(block_0001)  );


	virtuahChain.printFilter();
	std::cout << "\n\n\n\n========================================================" << "\n";

	std::vector< std::shared_ptr<block::BlockHeader> > addHeaderVector;
	addHeaderVector.push_back( block_0002.header() );
	virtuahChain.add( addHeaderVector );




	std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << "\n";

	addHeaderVector.clear();
	addHeaderVector.push_back( block_0003.header() );
	virtuahChain.add( addHeaderVector );


	std::cout << "\n\n\n\n========================================================" << "\n";

	virtuahChain.printFilter();
	virtuahChain.printHeaderValidationPendingQueue();
	virtuahChain.printMergePendingQueue();
	virtuahChain.printVirtualChain();

	std::cout << "\n\n\n\n========================================================" << "\n";

	std::vector< std::shared_ptr<block::Block> > addBlockVector;
	addBlockVector.push_back( std::make_shared<block::Block>(block_0002) );
	virtuahChain.add( addBlockVector );

	addBlockVector.clear();
	addBlockVector.push_back( std::make_shared<block::Block>(block_0003) );
	virtuahChain.add( addBlockVector );

	std::cout << "\n\n\n\n========================================================" << "\n";
	virtuahChain.printFilter();

	std::mutex mtx;
	std::condition_variable cv;
	std::unique_lock<std::mutex> lock(mtx);
	cv.wait( lock );
	*/

	return 0;







	/*
	std::shared_ptr<miya_chain::BlockLocalStrageManager> localStrageManager;
	localStrageManager = miyaChainManager.localStrageManager();


	std::shared_ptr<tx::P2PKH> p2pkh_0001 = interface.createTxFromJsonFile("../control_interface/tx_origin/payment_tx_info_0001.json");
	for( auto itr : p2pkh_0001->ins() ){
		itr->pkey( ecdsaManager.myPkey() );
	}
	std::cout << "p2pkh_0001 sign :: " << p2pkh_0001->sign() << "\n";
	std::cout << "p2pkh_0001 txout count :: " << p2pkh_0001->outCount() << "\n";
	std::shared_ptr<unsigned char> p2pkh_0001_ID;
	p2pkh_0001->calcTxID( &p2pkh_0001_ID );


	std::shared_ptr<miya_chain::UTXO> dummyUTXO = std::make_shared<miya_chain::UTXO>( p2pkh_0001->outs().at(0) , p2pkh_0001_ID  ,0 );
	std::cout << "dummy UTXO amout :: " << dummyUTXO->amount() << "\n";
	std::cout << "dummy UTXO txID :: ";
	for( int i=0; i<32; i++){
		printf("%02X", dummyUTXO->txID().get()[i]);
	} std::cout << "\n";


	std::shared_ptr<tx::P2PKH> p2pkh_0002 = interface.createTxFromJsonFile("../control_interface/tx_origin/payment_tx_info_0002.json");
	for( auto itr : p2pkh_0002->ins() ){
		itr->pkey( ecdsaManager.myPkey() );
	}
	std::cout << "p2pkh_0002 sign:: " << p2pkh_0002->sign() << "\n"; // 署名しないと使えない
	std::cout << "p2pkh_0002 txout count :: " << p2pkh_0002->outCount() << "\n";
	std::shared_ptr<unsigned char> coinbase_0002Text = std::shared_ptr<unsigned char>( new unsigned char[10] );
	memcpy( coinbase_0002Text.get() , "HelloWorld", 10 );
	tx::Coinbase coinbase_0002( 2 , coinbase_0002Text , 10 , selfAddress , miyaCore.context() );

	block::Block block_0002;
	block_0002.coinbase( std::make_shared<tx::Coinbase>(coinbase_0002) );
	block_0002.add( p2pkh_0001 );
	block_0002.add( p2pkh_0002 );

	std::shared_ptr<unsigned char> merkleRoot_0002; size_t merkleRoot_0002Length;
	merkleRoot_0002Length = block_0002.calcMerkleRoot( &merkleRoot_0002 );
	block_0002.header()->merkleRoot( merkleRoot_0002 );


	std::shared_ptr<unsigned char> dummyPrevBlockHash = std::shared_ptr<unsigned char>( new unsigned char[32] );
	memcpy( dummyPrevBlockHash.get(), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" , 32 );
	uint32_t nBits_0002 = 532390001;
	block_0002.header()->nBits( nBits_0002 );
	block_0002.header()->previousBlockHeaderHash( dummyPrevBlockHash );
	uint32_t nonce_0002 = miya_chain::simpleMining( nBits_0002 , block_0002.header(), false );
	block_0002.header()->nonce( nonce_0002 );
	block_0002.header()->print();

	std::shared_ptr<unsigned char> block_0002Hash;
	block_0002.blockHash( &block_0002Hash );


	localStrageManager->writeBlock( std::make_shared<block::Block>(block_0002) );
	auto readedBlock = localStrageManager->readBlock( block_0002Hash );
	auto utxoVector = localStrageManager->readUndo( block_0002Hash );



	std::cout << "------------------------------------------" << "\n";


	std::shared_ptr<unsigned char> temp;
	readedBlock->blockHash( &temp );
	for( int i=0; i<32; i++) {
		printf("%02X", temp.get()[i]);
	} std::cout << "\n";


	for( auto itr : utxoVector )
	{
		std::cout << "amount :: " << itr->amount() << "\n";
		std::cout << "outpoutIndex :: " << itr->outputIndex() << "\n";
			std::cout << "txID :: ";
			for( int i=0; i<32; i++ ){
				printf("%02X", itr->txID().get()[i]);
			} std::cout << "\n";
	} std::cout << "\n";
	std::cout << utxoVector.size() << "\n";

	localStrageManager->releaseBlock( block_0002Hash );


	auto readedBlock_2 = localStrageManager->readBlock( block_0002Hash );
	auto utxoVector_2 = localStrageManager->readUndo( block_0002Hash );


	if( readedBlock_2 == nullptr ) std::cout << "readedBlock_2 is nullptr" << "\n";
	std::cout << utxoVector_2.size() << "\n";

	std::mutex mtx;
	std::condition_variable cv;
	std::unique_lock<std::mutex> lock(mtx);
	cv.wait( lock );
	*/



	/*

	// Block0000
	std::cout << "=========================================================" << "\n";
	std::cout << "Block0000" << "\n";
	std::shared_ptr<unsigned char> chainHead;
	chainHead = miyaChainManager.chainState()->chainHead();
	std::shared_ptr<block::Block> block_0000 = miyaChainManager.localStrageManager()->readBlock(chainHead);
	std::shared_ptr<unsigned char> rawBlockHeader_0000; size_t rawBlockHeader_0000Length;
	rawBlockHeader_0000Length = block_0000->header()->exportRaw( &rawBlockHeader_0000 );
	std::cout << "RawBlockHeader_0000 :: " << "\n";
	for( int i=0; i<rawBlockHeader_0000Length ; i++){
		printf("%02X", rawBlockHeader_0000.get()[i] );
	} std::cout << "\n";

	std::shared_ptr<unsigned char> blockHash_0000; size_t blockHash_0000Length;
	blockHash_0000Length = block_0000->blockHash( &blockHash_0000 );

	block_0000->header()->print();
	std::cout << "=========================================================" << "\n";


	// Block0001
	std::cout << "---------------------------------------------------------------------------" << "\n";
	std::shared_ptr<tx::P2PKH> p2pkh_0001 = interface.createTxFromJsonFile("../control_interface/tx_origin/payment_tx_info_0001.json");
	for( auto itr : p2pkh_0001->ins() ){
		itr->pkey( ecdsaManager.myPkey() );
	}
	std::cout << "p2pkh_0001 sign :: " << p2pkh_0001->sign() << "\n";
	std::shared_ptr<unsigned char> dummyBuff; size_t dummyBuffLength;
	dummyBuffLength = p2pkh_0001->exportRaw( &dummyBuff );

	std::cout << "p2pkh_0001 reference prevOut :: " << "\n";
	p2pkh_0001->ins().at(0)->prevOut()->print();

	// block_2 のcoinbase作成
	std::shared_ptr<unsigned char> coinbase_0001Text = std::shared_ptr<unsigned char>( new unsigned char[10] );
	memcpy( coinbase_0001Text.get() , "HelloWorld" , 10 );
	tx::Coinbase coinbase_0001( 1 , coinbase_0001Text , 10 , selfAddress ,miyaCore.context() );

	block::Block block_0001;
	block_0001.coinbase( std::make_shared<tx::Coinbase>(coinbase_0001) );
	std::shared_ptr<unsigned char> merkleRoot; size_t merkleRootLength;
	merkleRootLength = block_0001.calcMerkleRoot( &merkleRoot );
	block_0001.header()->merkleRoot( merkleRoot );

	uint32_t nBits = 532390001; // 簡易的にマイニングの実行
	block_0001.header()->nBits( nBits );
	block_0001.header()->previousBlockHeaderHash( blockHash_0000 );
	uint32_t nonce = miya_chain::simpleMining( nBits , block_0001.header() , false );
	block_0001.header()->nonce( nonce );
	block_0001.header()->print();

	std::cout << "p2pkh_0001 verify :: " << p2pkh_0001->verify( miyaChainManager.utxoSet() ) << "\n";
	std::cout << "block_0001 header verify :: " << block_0001.header()->verify() << "\n";
	std::cout << "---------------------------------------------------------------------------" << "\n";



	uint32_t heigth;
	heigth = block_0001.coinbase()->height();
	std::cout << "heigth :: " << static_cast<size_t>(heigth) << "\n";


	std::cout << "\n\n\n\n\n\n\n-----------------------------------------------------" << "\n";


	// ローカルファイルマネージャーのテスト

	std::mutex mtx;
	std::condition_variable cv;
	std::unique_lock<std::mutex> lock(mtx);
	cv.wait( lock );


	return 0;
	*/


	/*
	miya_db::DatabaseManager headerDBManager;

	// SBコンテナのセットアップ
	std::shared_ptr<StreamBufferContainer> toHeaderDBSBContainer = std::make_shared<StreamBufferContainer>();
	std::shared_ptr<StreamBufferContainer> fromHeaderDBSBContainer = std::make_shared<StreamBufferContainer>();

	std::string localHeaderDBFile = "test_headers";
	headerDBManager.startWithLightMode( toHeaderDBSBContainer , fromHeaderDBSBContainer , localHeaderDBFile );




	miya_db::DatabaseManager txDBManager;

	std::shared_ptr<StreamBufferContainer> toTxDBSBContainer = std::make_shared<StreamBufferContainer>();
	std::shared_ptr<StreamBufferContainer> fromTxDBSBContainer = std::make_shared<StreamBufferContainer>();

	std::string localTxDBFile = "test_txs";
	txDBManager.startWithLightMode( toTxDBSBContainer , fromTxDBSBContainer , localTxDBFile );





	cipher::ECDSAManager ecdsaManager;
	ecdsaManager.init( (unsigned char *)"hello", 5 ); // priKeyには鍵がかかっているので

	// ここまでは必須セットアップ


	ControlInterface interface;
	std::shared_ptr<tx::P2PKH> loadedP2PKH = interface.createTxFromJsonFile("../control_interface/tx_origin/payment_tx_info_0000.json");

	for( auto itr : loadedP2PKH->ins() ){ // 入力に対する秘密鍵の設定
		itr->pkey( ecdsaManager.myPkey() );
	}


	loadedP2PKH->sign(); // トランザクションのTxIn用に署名を作成する
	std::shared_ptr<unsigned char> txID; size_t txIDLength;
	txIDLength = loadedP2PKH->calcTxID( &txID );


	std::shared_ptr<unsigned char> rawTx; size_t rawTxLength;
	rawTxLength = loadedP2PKH->exportRaw( &rawTx );

	miya_db::DatabaseManager dbManager;

	// SBコンテナのセットアップ
	std::shared_ptr<StreamBufferContainer> toDBSBContainer = std::make_shared<StreamBufferContainer>();
	std::shared_ptr<StreamBufferContainer> fromDBSBContainer = std::make_shared<StreamBufferContainer>();


	std::string localFile = "test";
	dbManager.startWithLightMode( toDBSBContainer , fromDBSBContainer , localFile );


	miya_chain::LightUTXOSet utxoSet( toDBSBContainer , fromDBSBContainer );
	// utxoSet.store( loadedP2PKH );

	std::cout << "P2PKH stored" << "\n";


	std::shared_ptr<unsigned char> searchTxID; size_t searchTxIDLength;
	searchTxID = loadedP2PKH->ins().at(0)->prevOut()->txID();



	std::shared_ptr<miya_chain::UTXO> utxo = utxoSet.get( searchTxID , 0 );
	if( utxo == nullptr )
	{
		std::cout << "utxo is nullptr" << "\n";
	}
	else
	{
		std::shared_ptr<unsigned char> exportedPkScript; size_t exportedPkScriptLength;
		exportedPkScriptLength = utxo->_content._pkScript->_script->exportRaw( &exportedPkScript );

	}


	bool flag = loadedP2PKH->verify( std::make_shared<miya_chain::LightUTXOSet>(utxoSet) );
	if( flag ) std::cout << "verify successfully done" << "\n";
	else std::cout << "verify failure" << "\n";

	// トランザクション入力の検証



	return 0;
	*/



	/*
	cipher::ECDSAManager ecdsaManager;
	ecdsaManager.init( (unsigned char *)"hello", 5 ); // priKeyには鍵がかかっているので
	//ecdsaManager.printPkey( ecdsaManager.myPkey() );



	miya_db::DatabaseManager dbManager;

	// SBコンテナのセットアップ
	std::shared_ptr<StreamBufferContainer> toDBSBContainer = std::make_shared<StreamBufferContainer>();
	std::shared_ptr<StreamBufferContainer> fromDBSBContainer = std::make_shared<StreamBufferContainer>();


	//std::string localFile = "test";
	std::string localFile = "../miya_db/table_files/test/test";
	dbManager.startWithLightMode( toDBSBContainer , fromDBSBContainer , localFile );


	miya_chain::LightUTXOSet utxoSet( toDBSBContainer , fromDBSBContainer );
	//utxoSet.store( loadedP2PKH );




	std::shared_ptr<unsigned char> text = std::shared_ptr<unsigned char>( new unsigned char[10] ); memcpy( text.get(), "HelloWorld", 10 );

	tx::Coinbase _coinbase( 10 , text, 10 );


	std::shared_ptr<unsigned char> pubKeyHash = std::shared_ptr<unsigned char>( new unsigned char[20] );
	memcpy( pubKeyHash.get() , "aaaaaaaaaaaaaaaaaaaa", 20 );

	std::shared_ptr<tx::TxOut> coinbaseOutput = std::shared_ptr<tx::TxOut>( new tx::TxOut );
	coinbaseOutput->init( 1000, pubKeyHash );
	_coinbase.add( coinbaseOutput );

	std::shared_ptr<unsigned char> rawCoinbase; unsigned int rawCoinbaseLength;
	rawCoinbaseLength = _coinbase.exportRaw(&rawCoinbase);


	std::shared_ptr<tx::Coinbase> loadedCoinbase = std::shared_ptr<tx::Coinbase>( new tx::Coinbase );
	loadedCoinbase->importRawSequentially( rawCoinbase );




	ControlInterface interface;
	std::shared_ptr<tx::P2PKH> loadedP2PKH = interface.createTxFromJsonFile("../control_interface/tx_origin/payment_tx_info_0000.json");

	for( auto itr : loadedP2PKH->ins() ) // 入力に対する秘密鍵の設定
		itr->pkey( ecdsaManager.myPkey() );


	loadedP2PKH->sign(); // トランザクションのTxIn用に署名を作成する


	std::shared_ptr<unsigned char> rawTx; unsigned int rawTxLength;
	rawTxLength = loadedP2PKH->exportRaw( &rawTx );


	// インポート

	std::shared_ptr<tx::P2PKH> importP2PKH = std::make_shared<tx::P2PKH>();
	importP2PKH->importRawSequentially( rawTx );

	importP2PKH->verify( std::make_shared<miya_chain::LightUTXOSet>(utxoSet) );



	block::Block block;
	block.coinbase( std::make_shared<tx::Coinbase>(_coinbase) );
	block.add( loadedP2PKH );
	block.add( importP2PKH );

	std::shared_ptr<unsigned char> mRoot; unsigned int mRootLength;
	mRootLength = block.calcMerkleRoot( &mRoot );


	std::cout << "GoodNight World" << "\n";


	uint32_t nBits = 522300001;
	block.header()->nBits( nBits );

	uint32_t nonce = miya_chain::simpleMining( nBits , block.header() );
	block.header()->nonce( nonce );
	std::cout << block.header()->nonce() << "\n";
	std::cout << nonce << "\n";
	block.header()->print();

	int headerFlag = miya_chain::verifyBlockHeader(  block.header() );
	std::cout << "header verify flag -> " << headerFlag << "\n";




	std::cout << "\n\n\n---------------------------------------\n\n" << "\n";



	miya_chain::transaction_pool_whole_unit_test();
	//miya_chain::TransactionPool txPool;

	return 0;
	*/




	/*
	auto p2pManager = std::make_shared<ekp2p::EKP2P>();
	std::thread p2pManagerTH([p2pManager](){
		p2pManager->init();
		p2pManager->start();
	});


	sleep(3);





	MiyaCore miya_core;
	miya_core.num  = 10;

	tx::PkScript pkScript;
	pkScript.createP2PKHScript();
//std::cout << pkScript.exportRawSize() << "\n";

	unsigned char* ret;
	unsigned int retSize;
	retSize = pkScript.exportRaw( &ret );

	for( int i=0; i<retSize ; i++){
		printf("%02X ", ret[i]);
	}
	*/



	/*
	cipher::ECDSAManager ecdsa_manager;

	EVP_PKEY *pkey = NULL;
	pkey = ecdsa_manager.generatePkey();
	//ecdsa_manager.printPkey( pkey );

	// ecdsa_manager.saveNewKeyPair( (unsigned char *)("hello") , 5 );

	//ecdsa_manager.readPubKey();
	//ecdsa_manager.readPriKey( (unsigned char *)("hello") );
	ecdsa_manager.init( (unsigned char *)"hello", 5 );
	pkey = ecdsa_manager.myPkey();
	ecdsa_manager.printPkey( pkey );
	*/


	/*
	tx::PrevOut prevOut;
	const char* id = "aaaaaaaaaabbbbbbbbbb";
	prevOut._txID = (unsigned char *)id;
	prevOut._index = htonl( 10 );


	unsigned char* _buff;
	unsigned int _buffSize;
	_buffSize = prevOut.exportRaw( &_buff );

	for( int i=0; i<_buffSize ; i++){
		printf("%02x", _buff[i] );
	}
	std::cout << "\n";



	unsigned char* buff	;
	unsigned int buffSize ;

	tx::TxIn tx_in;
	tx_in._prevOut = &prevOut;
	buffSize = tx_in.exportEmptyRaw( &buff );

	for( int i=0; i<buffSize; i++){
		printf("%02x", buff[i]);
	}

	unsigned int tmp = 100;
	std::cout << "\n" << static_cast<unsigned char>(tmp & 0xFF )	 << "\n";
	printf("%02x", tmp);
	*/



	/*
	cipher::ECDSAManager ecdsaManager;
	ecdsaManager.init( (unsigned char *)"hello", 6);


	tx::PrevOut prevOut;
	const char* id = "aaaaaaaaaabbbbbbbbbb";
	prevOut._txID = (unsigned char *)id;
	prevOut._index = htonl( 10 );



	tx::SignatureScript *signatureScript = new tx::SignatureScript;
	tx::TxIn txIn;
	txIn._prevOut = &prevOut;
	txIn._signatureScript = signatureScript;

	txIn.pkey( ecdsaManager.myPkey() );
	//cipher::ECDSAManager::printPkey( txIn.pkey() );


	tx::P2PKH p2pkh;
	p2pkh.addInput( &txIn );
	printf("addeed signature scipt pointer -> %p \n", txIn.signatureScript() );


	tx::SignatureScript *_signatureScript = new tx::SignatureScript;
	tx::TxIn _txIn;
	_txIn._prevOut = &prevOut;
	_txIn._signatureScript = _signatureScript;
	_txIn.pkey( ecdsaManager.myPkey() );
	p2pkh.addInput( &_txIn );
	printf("addeed signature scipt pointer -> %p \n", _txIn.signatureScript() );


	tx::PkScript *pkScript = new tx::PkScript;
	tx::TxOut txOut(9999);
	txOut._pkScript = pkScript;
	txOut._pkScript->createP2PKHScript( ecdsaManager.myPkey());
	p2pkh.addOutput( &txOut );


	p2pkh.sign();


	unsigned char* p2pkhExportBuff = NULL; unsigned int p2pkhExportBuffSize = 0;
	p2pkhExportBuffSize = p2pkh.exportRaw( &p2pkhExportBuff );



	tx::P2PKH *new_p2pkh = new tx::P2PKH( p2pkhExportBuff , p2pkhExportBuffSize );
	//std::cout << new_p2pkh->TxInCnt() << "\n";


	//new_p2pkh->_body._ins.at(0)->hello();

	if (new_p2pkh->verify()  == true ){
		std::cout << "検証成功" << "\n";
	}else{
		std::cout << "検証失敗" << "\n";
	}

	//std::cout << "トランザクション全体書き出しサイズ :" << p2pkhExportBuffSize << "\n";


	std::cout << "\n\n\n\n\n" << " ========================================== "  << "\n\n\n\n\n";

	new_p2pkh->sign();
	//std::cout << new_p2pkh->exportRawSize() << "\n";


	tx::TxCBTableManager table_manager;
	std::cout << "TxCB Table Manager Created !!!!!" << "\n";

	tx::P2PKH *_new_p2pkh = new tx::P2PKH;
	_new_p2pkh = new_p2pkh;

	_new_p2pkh->_body._ins.at(0)->prevOut()->_index = htonl( 20 );

	std::cout << table_manager.add( new_p2pkh ) << "\n";
	//std::cout << table_manager.add( _new_p2pkh ) << "\n";
	std::cout << table_manager.find( new_p2pkh->txID() ) << "\n";
	*/




}
