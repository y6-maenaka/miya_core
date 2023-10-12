#include "txcb_bucket.h"

#include "./txcb.h"
#include "./txcb_table.h"
#include "./transaction_pool.h"


namespace miya_chain
{




TxCBBucket::TxCBBucket( unsigned char symbol , TxCBTable* parentTable ) : _bucketSymbol(symbol) , _txCount(0)
{
	//_bucketSymbol = symbol;
	_parentTable = parentTable;
	_scaleSize = DEFAULT_SCALE_SIZE;
	_txcbHead = nullptr;
	// _txCount = 0;
}





int TxCBBucket::add( std::shared_ptr<TxCB> target )
{

	std::cout << "|--------------------------------------|" << "\n";
	printf("| TxCBBucket Symbol[ %02X ]\n", _bucketSymbol );
	std::cout << "| [ Add ] :: ";
	for( int i=0; i<20; i++ )
	{
		printf("%02X", target->txID().get()[i]);
	} std::cout << "\n";
	std::cout << "|--------------------------------------|" << "\n";

	std::shared_ptr<TxCB> tailTxCB = _txcbHead;

	if( tailTxCB == nullptr )
	{
		target->prev( target );
		target->next( target );
		_txcbHead = target; // 先頭がセットされていない時にスケールが発生することはない
		_txCount += 1;
		return 0;
	}

	tailTxCB = _txcbHead->prev();
	std::cout << "\x1b[32m" << "prev() :: ";
	for( int i=0; i<20; i++ ){
		printf("%02X", tailTxCB->txID().get()[i]);
	}
	std::cout << "\x1b[39m" << "\n";

	target->next( tailTxCB->next() );
	target->prev( tailTxCB );

	tailTxCB->next()->prev( target );
	tailTxCB->next( target );


	_txCount += 1;


	printf("Bucket  Pointer:: %p\n", this );
	std::cout << "Bucket Size is -> " << _txCount << "\n";
	printf("Bucket symbol ::  %02X\n", _bucketSymbol );
	std::cout << "Add ID :: ";
	for( int i=0; i<20; i++ ){
		printf("%02X", target->txID().get()[i]);
	} std::cout << "\n";


	if( _txCount >= DEFAULT_SCALE_SIZE )
		autoScaleUp( _txcbHead );

	

	return 0;
	
	/*
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
	*/
}





void TxCBBucket::remove( std::shared_ptr<TxCB> target )
{
	std::cout << "TxCBBucket::remove() called" << "\n";

	std::shared_ptr<TxCB> thisTarget = this->find( target ); // targetは検索用のTxCBなので本来のTxCBを取得する 実際にはstatusで判断した方が良い

	if( thisTarget == nullptr ) return; // 対象がバケットにストアされていない場合

	if( thisTarget == _txcbHead || thisTarget->next() == thisTarget || thisTarget->prev() == thisTarget ) { // 対象がバケットヘッドの場合
		_txcbHead = nullptr;
		return;
	}


	thisTarget->prev()->next( thisTarget->next() );
	thisTarget->next()->prev( thisTarget->prev() );

	return;
}




std::shared_ptr<TxCB> TxCBBucket::find( std::shared_ptr<TxCB> target )
{
	std::shared_ptr<TxCB> _currentTxCB = _txcbHead;
	if( _currentTxCB == nullptr ) return nullptr; // 要素が存在しない


	do // 重複して追加されるTxCBは比較的最近プールに追加されている可能性が高いため，後方から検索する
	{
		if( memcmp( _currentTxCB->txID().get() , target->txID().get(), 20 ) == 0 )
			return _currentTxCB;

		_currentTxCB = _currentTxCB->prev();

	} while( _currentTxCB != _txcbHead );


	return nullptr; 
}





void TxCBBucket::autoScaleUp( std::shared_ptr<TxCB> txcbHead )
{
	std::cout << "==============================================" << "\n";
	std::cout << "\x1b[36m" << "Auto scale up started with layer -> " << _parentTable->layer() + 1<< "\x1b[39m" << "\n";
	// TxCBTable *newTable = new TxCBTable( _parentTable->layer() + 1 );
	std::shared_ptr<TxCBTable> newTable = std::make_shared<TxCBTable>( _parentTable->layer() + 1 );

	
	std::shared_ptr<TxCB> currentTxCB = txcbHead;
	std::shared_ptr<TxCB> nextTxCB = currentTxCB;

	std::cout << "nextTxCB::TxID ::  ";
	printBucket();
 
	do
	{
		currentTxCB = nextTxCB;
		nextTxCB = currentTxCB->next(); // currentTxCBが追加されたらnext情報が消失するため事前に確保しておく
		
		std::cout << "\x1b[33m" <<  "Auto Scale And Add target :: ";
		for( int i=0; i<20; i++ ){
			printf("%02X", currentTxCB->txID().get()[i] );
		} std::cout << "\x1b[39m" << "\n";

		newTable->add( currentTxCB );
		std::cout << "## auto scale up done" << "\n";

		// currentTxCB = nextTxCB;
	}
	while( nextTxCB != _txcbHead );
	std::cout << "\x1b[35m" << "WhileBreaked" << "\x1b[39m" << "\n";

	_parentTable->txContainer( TransactionPool::bucketSymbolToIndex(_bucketSymbol) , newTable );
	
	std::cout << "========================================" << "\n";

	// _parentTable->bucketTableList( this , 1 , symbol );


}




void TxCBBucket::printBucket()
{
	std::shared_ptr<TxCB> currentTxCB = _txcbHead;

	int i=0;
	std::cout << ".............................................." << "\n";
	printf("Bucket Symbol :: %02X\n", _bucketSymbol );
	if( _txcbHead == nullptr ){
		std::cout << "  ## empty" << "\n";
		return;
	}
	do
	{
		std::cout << "  - ("<< i << ")Tx :: ";
		for( int i=0; i<20; i++)
		{
			printf("%02X", currentTxCB->txID().get()[i]);
		} std::cout << "\n";
		currentTxCB = currentTxCB->next();
		i++;
	} while( currentTxCB != _txcbHead );
	std::cout << ".............................................." << "\n";
}


};

