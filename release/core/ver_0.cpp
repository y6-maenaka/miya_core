#include "ver_0.h"

#include "../../shared_components/cipher/ecdsa_manager.h"
#include "../../shared_components/stream_buffer/test/unit_test.h"
#include "../../shared_components/json.hpp"
#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"
#include "../../shared_components/hash/sha_hash.h"

#include "../../miya_chain/block/block.h"

#include "../../miya_chain/mining/simple_mining.h"

#include "../../miya_chain/transaction/tx/tx_in.h"
#include "../../miya_chain/transaction/tx/tx_out.h"
#include "../../miya_chain/transaction//tx/prev_out.h"
#include "../../miya_chain/transaction/p2pkh/p2pkh.h"
#include "../../miya_chain/transaction/coinbase/coinbase.h"
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

#include "../../miya_chain/miya_core_manager.h"
#include "../../miya_chain/miya_coin/local_strage_manager.h"


#include "openssl/evp.h"

#include "../../ekp2p/ekp2p.h"
// #include "../../ekp2p/kademlia/k_routing_table.h"

#include "../../control_interface/control_interface.h"


#include "../../miya_db/miya_db/database_manager.h"


int main()
{
	std::cout << " WELCOME TO MIYA COIN CLIENT [ MIYA_CORE ] " << "\n";

	
	miya_chain::MiyaChainManager miyaChainManager;
	std::shared_ptr<StreamBufferContainer> toEKP2PBrokerDummySBC = std::make_shared<StreamBufferContainer>();
	miyaChainManager.init( toEKP2PBrokerDummySBC );

	std::pair< std::shared_ptr<StreamBufferContainer> , std::shared_ptr<StreamBufferContainer> > blockIndexDBSBCPair;
	blockIndexDBSBCPair = miyaChainManager.blockIndexDBSBCPair();
	miya_chain::BlockLocalStrageManager blockLocalStrageManager( blockIndexDBSBCPair.first, blockIndexDBSBCPair.second );



	// ------- ブロックの作成 ----------

	cipher::ECDSAManager ecdsaManager; // 暗号関係マネージャーの起動
	ecdsaManager.init( (unsigned char *)"hello", 5 ); // priKeyには鍵がかかっているので


	std::pair< std::shared_ptr<StreamBufferContainer> , std::shared_ptr<StreamBufferContainer> > utxoSetDBSBCPair;
	utxoSetDBSBCPair = miyaChainManager.utxoSetDBSBCPair();
	miya_chain::LightUTXOSet utxoSet( utxoSetDBSBCPair.first , utxoSetDBSBCPair.second );

	// CoinBaseの作成
	std::shared_ptr<unsigned char> text = std::shared_ptr<unsigned char>( new unsigned char[10] ); memcpy( text.get(), "HelloWorld", 10 );
	tx::Coinbase _coinbase( 10 , text, 10 );

	// coinbase報酬先アドレスの設定
	std::shared_ptr<unsigned char> pubKeyHash = std::shared_ptr<unsigned char>( new unsigned char[20] ); 
	memcpy( pubKeyHash.get() , "aaaaaaaaaaaaaaaaaaaa", 20 );
	std::shared_ptr<tx::TxOut> coinbaseOutput = std::shared_ptr<tx::TxOut>( new tx::TxOut );
	coinbaseOutput->init( 1000, pubKeyHash );
	_coinbase.add( coinbaseOutput );

	std::shared_ptr<unsigned char> rawCoinbase; size_t rawCoinbaseLength;
	rawCoinbaseLength = _coinbase.exportRaw( &rawCoinbase );
	std::cout << "Raw Coinbase(Hex) :: ";
	for( int i=0; i<rawCoinbaseLength; i++ ){
		printf("%02X", rawCoinbase.get()[i]);
	} std::cout << "\n";

	std::cout << "Raw Coinbase(Binary) :: ";
	for( int i=0; i<rawCoinbaseLength; i++ ){
		printf("%c", rawCoinbase.get()[i]);
	} std::cout << "\n";



	ControlInterface interface; // ローカルファイルからトランザクションを作成するコンポーネントのセットアップ
	std::shared_ptr<tx::P2PKH> loadedP2PKH = interface.createTxFromJsonFile("../control_interface/tx_origin/payment_tx_info_0000.json");
	for( auto itr : loadedP2PKH->ins() ) // 入力に対する秘密鍵の設定
		itr->pkey( ecdsaManager.myPkey() );

	loadedP2PKH->sign(); // トランザクションのTxIn用に署名を作成する


	std::shared_ptr<unsigned char> rawTx; unsigned int rawTxLength; // データを増やす為これをコピーする
	rawTxLength = loadedP2PKH->exportRaw( &rawTx );
	std::shared_ptr<tx::P2PKH> importP2PKH = std::make_shared<tx::P2PKH>();
	importP2PKH->importRawSequentially( rawTx );
	importP2PKH->verify( std::make_shared<miya_chain::LightUTXOSet>(utxoSet) ); // 一応検証する
	importP2PKH->sign();

	std::cout << "-------------------------------------------" << "\n";
	std::cout << "loadedP2PKH length :: " << rawTxLength << "\n";
	std::shared_ptr<unsigned char> rawImportedP2PKH; size_t rawImportedP2PKHLength;
	rawImportedP2PKHLength = importP2PKH->exportRaw( &rawImportedP2PKH );
	std::cout << "rawImportedTx Length :: " << rawImportedP2PKHLength << "\n";
	std::cout << "-------------------------------------------" << "\n";

	// ブロックの作成
	block::Block block;
	block.coinbase( std::make_shared<tx::Coinbase>(_coinbase) );
	block.add( loadedP2PKH ); // ファイルから読み込んだトランザクションを追加
	block.add( importP2PKH ); // コピーしたトランザクションを追加

	std::shared_ptr<unsigned char> mRoot; unsigned int mRootLength; // マークルルートの設定
	mRootLength = block.calcMerkleRoot( &mRoot );

	uint32_t nBits = 532390001; // 簡易的にマイニングの実行
	block.header()->nBits( nBits ); 

	uint32_t nonce = miya_chain::simpleMining( nBits , block.header() );
	block.header()->nonce( nonce );
	block.header()->print();

	int headerFlag = miya_chain::verifyBlockHeader(  block.header() );
	std::cout << "header verify flag -> " << headerFlag << "\n";




	std::shared_ptr<unsigned char> blockHash;  block.blockHash( &blockHash );
	// ローカルファイルへのブロック書き込みテスト
	blockLocalStrageManager.writeBlock( std::make_shared<block::Block>(block) );

	sleep(1);

	std::shared_ptr<block::Block> readedBlock;
	readedBlock = blockLocalStrageManager.readBlock( blockHash );

	std::cout << "------------------------------" << "\n";
	std::shared_ptr<unsigned char> _blockHash; 
	readedBlock->blockHash( &_blockHash );
	std::cout << "Original BlockHash :: ";
	for( int i=0; i<32; i++ ){
		printf("%02X", blockHash.get()[i]);
	} std::cout << "\n";
	std::cout << "readed blockHash :: ";
	for( int i=0; i<32; i++){
		printf("%02X", _blockHash.get()[i]);
	} std::cout << "\n";
	std::cout << "------------------------------" << "\n";
	std::shared_ptr<unsigned char> rawCoinbase1; size_t rawCoinbase1Length;
	std::shared_ptr<unsigned char> rawCoinbase2; size_t rawCoinbase2Length;
	rawCoinbase1Length = block.coinbase()->exportRaw( &rawCoinbase1 );
	rawCoinbase2Length = readedBlock->coinbase()->exportRaw( &rawCoinbase2 );
	for( int i=0; i<rawCoinbase1Length; i++ ){
		printf("%02X", rawCoinbase1.get()[i]);
	} std::cout << "\n";
	for( int i=0; i<rawCoinbase2Length; i++){
		printf("%02X", rawCoinbase2.get()[i]);
	} std::cout << "\n";
	std::cout << "------------------------------" << "\n";
	std::shared_ptr<unsigned char> rawTx1; size_t rawTx1Length;
	std::shared_ptr<unsigned char> rawTx2; size_t rawTx2Length;
	rawTx1Length = block.txVector().at(0)->exportRaw( &rawTx1 );
	readedBlock->txVector().at(0)->sign();
	rawTx2Length = readedBlock->txVector().at(0)->exportRaw( &rawTx2 );
	for( int i=0; i<rawTx1Length; i++ ){
		printf("%02X", rawTx1.get()[i]);
	} 
	std::cout << "===============================" << "\n";
	for( int i=0; i<rawTx2Length; i++){
		printf("%02X", rawTx2.get()[i]);
	} std::cout << "\n";
	std::cout << "------------------------------" << "\n";

	
	std::mutex mtx;
	std::condition_variable cv;
	std::unique_lock<std::mutex> lock(mtx);
	cv.wait( lock );


	return 0;
	





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


