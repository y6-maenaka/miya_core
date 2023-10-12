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
	// _rootTable = new TxCBTable( 0 );
	_rootTable = std::shared_ptr<TxCBTable>( new  TxCBTable{0u} );
	//_pUTxOCache = new ProvisionalUTxOCache;
	return;
}



std::shared_ptr<TxCB> TransactionPool::find( std::shared_ptr<TxCB> target )
{
	return this->find( target );
}


std::shared_ptr<TxCB> TransactionPool::find( std::shared_ptr<tx::P2PKH> target )
{
	if( target == nullptr ) return nullptr;

	std::shared_ptr<unsigned char> txID;  target->calcTxID( &txID );
	return this->find( txID );
}



std::shared_ptr<TxCB>	TransactionPool::find( std::shared_ptr<unsigned char> targetTxID )
{
	std::shared_ptr<TxCB> searchTargetTxCB = std::make_shared<TxCB>(); // 検索用のTxCB
	searchTargetTxCB->txID( targetTxID );

	return _rootTable->find( searchTargetTxCB );
}




int TransactionPool::add( std::shared_ptr<tx::P2PKH> target )
{
	std::shared_ptr<TxCB> storeTargetTxCB = std::make_shared<TxCB>( target );

	return this->add( std::make_shared<TxCB>(target) );

	// トランザクションプールへの追加
	//return _rootTable->add( storeTargetTxCB ); // 再帰的に追加

	/*
	暫定utxoの追加 -> ※　必要なの基本的にTxCB内部のトランザクション本体のみ
	 _pUTxOCache->add( storeTargetTxCB ); //　保存したトランザクション内のtx_outを暫定utxoに追加
	*/
}



int TransactionPool::add( std::shared_ptr<TxCB> target )
{
	return _rootTable->add( target );
}




void TransactionPool::remove( std::shared_ptr<TxCB> target )
{

	return _rootTable->remove( target );
	/*	
	//_pUTxOCache->remove( target ); // 暫定utxoからの削除
	// target.reset(); // 要素を消さないほうがいいかもしれない
	*/
}

void TransactionPool::remove( std::shared_ptr<tx::P2PKH> target )
{
	std::shared_ptr<unsigned char> txID; target->calcTxID( &txID );

	return this->remove( txID );
}




void TransactionPool::remove( std::shared_ptr<unsigned char> targetTxID )
{
	std::shared_ptr<TxCB> targetTxCB = std::make_shared<TxCB>();
	targetTxCB->txID( targetTxID );
	return this->remove( targetTxCB );
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




size_t TransactionPool::bucketSymbolToIndex( const unsigned char symbol )
{
	return static_cast<size_t>( symbol );
}




};
