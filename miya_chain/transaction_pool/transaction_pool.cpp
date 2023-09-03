#include "transaction_pool.h"

#include "../transaction/p2pkh/p2pkh.h"

#include "./txcb.h"
#include "./txcb_table.h"


namespace miya_chain
{





TransactionPool::TransactionPool()
{
	_rootTable = new TxCBTable( 0 );
	return;
}




std::shared_ptr<TxCB> TransactionPool::find( std::shared_ptr<tx::P2PKH> target )
{
	if( target == nullptr ) return nullptr;

	std::shared_ptr<TxCB> searchTargetTxCB = std::make_shared<TxCB>( target );
	
	return _rootTable->find( searchTargetTxCB );
}




void TransactionPool::store( std::shared_ptr<tx::P2PKH> target )
{
	std::shared_ptr<TxCB> storeTargetTxCB = std::make_shared<TxCB>( target );

}




};
