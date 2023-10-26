#include "transaction_store.h"


#include "../../../transaction/p2pkh/p2pkh.h"




namespace miya_chain
{





TransactionStore::TransactionStore( std::shared_ptr<StreamBufferContainer> pushSBContainer , std::shared_ptr<StreamBufferContainer> popSBContainer ) : MiyaDBSBClient( pushSBContainer , popSBContainer )
{
	return ;
}



std::shared_ptr<tx::P2PKH> TransactionStore::get( std::shared_ptr<unsigned char> txID )
{
	std::shared_ptr<unsigned char> rawTx; size_t rawTxLength;
	rawTxLength = MiyaDBSBClient::get( txID , &rawTx );

	std::shared_ptr<tx::P2PKH> retTx = std::make_shared<tx::P2PKH>();
	retTx->importRaw( rawTx , rawTxLength );

	return retTx;
}




bool TransactionStore::add( std::shared_ptr<tx::P2PKH> targetTx )
{
	std::shared_ptr<unsigned char> txID; size_t txIDLength;
	txIDLength = targetTx->calcTxID( &txID );

	std::shared_ptr<unsigned char> exportedTx; size_t exportedTxLength;
	exportedTxLength = targetTx->exportRaw( &exportedTx );

	return MiyaDBSBClient::add( txID , exportedTx , exportedTxLength );
}



















};
