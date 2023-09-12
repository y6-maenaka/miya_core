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

	// トランザクションプールへの追加
	_rootTable->add( storeTargetTxCB ); // 再帰的に追加

	// 暫定utxoの追加 -> ※　必要なの基本的にTxCB内部のトランザクション本体のみ
	_pUTxOCache->add( storeTargetTxCB ); //　保存したトランザクション内のtx_outを暫定utxoに追加
}





void TransactionPool::remove( std::shared_ptr<TxCB> target )
{
	std::shared_ptr<TxCBBucket> targetBucket = _rootTable->findBucket( target );
	if( targetBucket == nullptr ) return;

	targetBucket->remove( target ); // トランザクションプールからの削除
	_pUTxOCache->remove( target ); // 暫定utxoからの削除
	// target.reset(); // 要素を消さないほうがいいかもしれない
}






// 呼び出しはブロック検証時のみ ブロック検証トランザクションの場合は二重支払いがあった場合はプール内のトランザクションを削除する
// その他、単純トランザクション受信時は自身のプール内のトランザクションを優先する
// 該当するトランザクションをプールから完全に削除するので
std::vector<TxCB> autoResolveDoubleSpends( std::shared_ptr<tx::P2PKH> target ) 
{
	/*
	 < 二重支払いチェックフロー >
	 1. 二重支払いが発生するトランザクションを検出する
	 2. 対象のトランザクションをトランザクションプールから削除する
	 */
}


};
