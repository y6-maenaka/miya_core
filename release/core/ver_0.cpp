#include "ver_0.h"

#include "../../shared_components/cipher/ecdsa_manager.h"
#include "../../shared_components/middle_buffer/middle_buffer.h"

#include "../../miya_chain/transaction/tx/tx_in.h"
#include "../../miya_chain/transaction/tx/tx_out.h"
#include "../../miya_chain/transaction/p2pkh/p2pkh.h"
#include "../../miya_chain/transaction/script/signature_script.h"

#include "../../miya_chain/transaction/txcb_table_manager/txcb_table_manager.h"
#include "../../miya_chain/transaction/txcb_table_manager/txcb_table/txcb_table.h"

#include "openssl/evp.h"

#include "../../ekp2p/ekp2p.h"
// #include "../../ekp2p/kademlia/k_routing_table.h"

#include "../../control_interface/control_interface.h"


int main()
{
	std::cout << " WELCOME TO MIYA COIN CLIENT [ MIYA_CORE ] " << "\n";


	cipher::ECDSAManager ecdsaManager;
	ecdsaManager.init( (unsigned char *)"hello", 5 );
	ecdsaManager.printPkey( ecdsaManager.myPkey() );


	/*
	ekp2p::EKP2P *p2pManager = new ekp2p::EKP2P;
	p2pManager->init();
	p2pManager->start();
	*/


	ControlInterface interface;
	std::shared_ptr<tx::P2PKH> loadedP2PKH = interface.createTxFromJsonFile("../control_interface/tx_origin/payment_tx_info_0000.json");

	for( auto itr : loadedP2PKH->ins() ) // 入力に対する秘密鍵の設定
		itr->pkey( ecdsaManager.myPkey() );



	loadedP2PKH->sign();

	std::shared_ptr<unsigned char> rawTx; unsigned int rawTxLength;
	rawTxLength = loadedP2PKH->exportRaw( &rawTx );

	std::cout << rawTxLength << "\n";
	for( int i=0 ;i<rawTxLength ; i++)
	{
		printf("%02X", rawTx.get()[i]);
	} std::cout << "\n";

	return 0;

	auto p2pManager = std::make_shared<ekp2p::EKP2P>();
	std::thread p2pManagerTH([p2pManager](){
		p2pManager->init();
		p2pManager->start();
	});


	sleep(3);





	/*
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


