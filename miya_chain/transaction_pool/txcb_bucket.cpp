#include "txcb_bucket.h"

#include "./txcb.h"
#include "./txcb_table.h"


namespace miya_chain
{




TxCBBucket::TxCBBucket( unsigned char symbol , std::shared_ptr<TxCBTable> parentTable )
{
	_bucketSymbol = symbol;
	_parentTable = parentTable;

	_scaleSize = DEFAULT_SCALE_SIZE;

	_txcbHead = nullptr;
}





int TxCBBucket::add( std::shared_ptr<TxCB> target )
{
	std::shared_ptr<TxCB>	currentTxCB = _txcbHead;
	int count = 0;

	if( currentTxCB == nullptr ) // 先頭ポインタが空の場合は対象のブロックが先頭になる
	{
		_txcbHead = target;
		return count;
	}


	// 最後尾までポインタを送る ※ headにprevがあるならそれを参照しても大丈夫そう
	while( currentTxCB->next() != nullptr && currentTxCB != target ) // リストネクストが存在して，対象のTxCBでない場合
	{ //　要素を重複して挿入することは許可しない
		currentTxCB = currentTxCB->next();
		count++;
	}
		
	currentTxCB->next( target ); // 挿入前最後尾要素のnextを対象ブロックに
	target->prev( currentTxCB ); // 対象ブロックの前ブロックを挿入前最後尾ブロックに　
	_txcbHead->prev( target ); // バケット先頭のprevを対象ブロックに　
	
	
	if( count >= _scaleSize )
	{
		autoScaleUp( _txcbHead );
		delete this;
	}

	return count;
}





void TxCBBucket::remove( std::shared_ptr<TxCB> target )
{
	if( find( target ) == nullptr ) return; // 必要ないかも

	if( _txcbHead == target	)
	{
		_txcbHead = nullptr;
	}
	else
	{
		target->prev()->next( target->next() );
		target->next()->prev( target->prev() );
	}

	// target.reset(); // ここで消してしまうと,暫定UTXOで削除できなくなる
	return;
}




std::shared_ptr<TxCB> TxCBBucket::find( std::shared_ptr<TxCB> target )
{
	std::shared_ptr<TxCB> _currentTxCB = _txcbHead;

	if( _currentTxCB == nullptr ) return nullptr;

	do
	{
		if( memcmp( _currentTxCB->txID().get() , target->txID().get() , 20 ) == 0 ) return _currentTxCB;
		_currentTxCB = _currentTxCB->next();
	}
	while( _currentTxCB->next() != nullptr );

	//if( memcmp( _currentTxCB->txID() , target->txID() , 20 ) == 0 )	 return _currentTxCB; // リスト最後尾の評価

	return nullptr; 
}





void TxCBBucket::autoScaleUp( std::shared_ptr<TxCB> txcbHead )
{
	TxCBTable *newTable = new TxCBTable( _parentTable->layer() + 1 );

	std::shared_ptr<TxCB> currentTxCB = _txcbHead;
	
	do
	{
		newTable->add( currentTxCB );
		currentTxCB = currentTxCB->next();
	}
	while( currentTxCB->next() != nullptr );

	// _parentTable->bucketTableList( this , 1 , symbol );


}






};

