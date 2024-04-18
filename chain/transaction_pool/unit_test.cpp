#include "unit_test.h"


#include "transaction_pool.h"
#include "txcb.h"
#include "txcb_bucket.h"
#include "txcb_table.h"


namespace chain
{



void transaction_pool_whole_unit_test()
{
	
	TransactionPool txPool;
	std::cout << "transaction pool initialized" << "\n";


	std::shared_ptr<TxCB> txcb_1 = std::make_shared<TxCB>(nullptr);
	std::shared_ptr<unsigned char> txcb_tx_id_1 = std::shared_ptr<unsigned char>( new unsigned char[20] );
	memcpy( txcb_tx_id_1.get(), "aaaaaaaaaaaaaaaaaaaa", 20  ); // 61 61
	txcb_1->txID( txcb_tx_id_1 );
	txPool.add( txcb_1 );



	std::shared_ptr<TxCB> txcb_2 = std::make_shared<TxCB>(nullptr);
	std::shared_ptr<unsigned char> txcb_tx_id_2 = std::shared_ptr<unsigned char>( new unsigned char[20] );
	memcpy( txcb_tx_id_2.get(), "abbbbbbbbbbbbbbbbbbb", 20  ); // 61 62
	txcb_2->txID( txcb_tx_id_2 );
	txPool.add( txcb_2 );


	std::shared_ptr<TxCB> txcb_3 = std::make_shared<TxCB>(nullptr);
	std::shared_ptr<unsigned char> txcb_tx_id_3 = std::shared_ptr<unsigned char>( new unsigned char[20] );
	memcpy( txcb_tx_id_3.get(), "accccccccccccccccccc", 20  ); // 61 63
	txcb_3->txID( txcb_tx_id_3 );
	txPool.add( txcb_3 );


	std::shared_ptr<TxCB> txcb_4 = std::make_shared<TxCB>(nullptr);
	std::shared_ptr<unsigned char> txcb_tx_id_4 = std::shared_ptr<unsigned char>( new unsigned char[20] );
	memcpy( txcb_tx_id_4.get(), "addddddddddddddddddd", 20  ); // 61 64
	txcb_4->txID( txcb_tx_id_4 );
	txPool.add( txcb_4 );


	std::shared_ptr<TxCB> txcb_5 = std::make_shared<TxCB>(nullptr);
	std::shared_ptr<unsigned char> txcb_tx_id_5 = std::shared_ptr<unsigned char>( new unsigned char[20] );
	memcpy( txcb_tx_id_5.get(), "77777777777777777777", 20  ); // 61 64
	txcb_5->txID( txcb_tx_id_5 );
	txPool.add( txcb_5 );




	std::cout << " ================= In TramsactinoPool Below ====================" << "\n";
	auto layer0_table{ std::get_if<std::shared_ptr<TxCBTable>>(&((txPool._rootTable)->_containerArray[6])) };



	auto layer1_table{ std::get_if<std::shared_ptr<TxCBTable>>(&((*layer0_table))->_containerArray[1]) };



	auto layer2_table{ std::get_if<std::shared_ptr<TxCBTable>>(&((*layer1_table))->_containerArray[6]) };



	for( int i=0; i<16; i++ ){
		auto layer_2_bucket{ std::get_if<std::shared_ptr<TxCBBucket>>( &((*layer2_table)->_containerArray[i]) ) };
	}


	txPool.remove( txcb_1 );

	txPool.add( txcb_1 );
	std::cout << "-----------------" << "\n";
	txPool.add( txcb_1 );



	/*
	auto moreMoreTable{ std::get_if<std::shared_ptr<TxCBTable>>(&((*moreTable)->_containerArray[1])) };
	(*moreMoreTable)->printTable();
	*/	



}


};
