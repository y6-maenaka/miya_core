#include "transaction_pool.h"

#include "../transaction/p2pkh/p2pkh.h"

#include "./txcb.h"
#include "./txcb_table.h"
#include "./txcb_bucket.h"
#include "./provisional_utxo_cache/provisional_utxo_cache.h"


namespace miya_chain
{





TransactionPool::TransactionPool()
{
	_rootTable = new TxCBTable( 0 );
	_pUTxOCache = new ProvisionalUTxOCache;
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
	_rootTable->add( storeTargetTxCB ); // 再帰的に追加
	// 書き残し

	_pUTxOCache->add( storeTargetTxCB ); //　保存したトランザクション内のtx_outを暫定utxoに追加
}





void TransactionPool::remove( std::shared_ptr<TxCB> target )
{
	std::shared_ptr<TxCBBucket> targetBucket = _rootTable->findBucket( target );
	if( targetBucket == nullptr ) return;

	targetBucket->remove( target );
	_pUTxOCache->remove( target );

	// target.reset(); // 消さないほうがいいかもしれない
}






};
