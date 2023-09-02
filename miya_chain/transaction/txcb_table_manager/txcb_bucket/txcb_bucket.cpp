#include "txcb_bucket.h"

#include "../../p2pkh/p2pkh.h"
#include "../txcb_table_manager.h"
#include "../txcb/txcb.h"



namespace tx{




TxCBBucket::TxCBBucket( unsigned char symbol, TxCBTable *parentTable ) : _startCB( nullptr )
{
	_symbol = symbol;
	_parentTable = parentTable;

	_scaleSize = DEFAULT_SCALE_SIZE;
}



bool TxCBBucket::push( TxCB *txcb  )
{ 
	TxCB *currentTxCB = _startCB;

	// バケット最初の登録だった場合
	if( currentTxCB == nullptr )
	{
		_startCB = txcb;
		return true; 
	}


	int cnt = 0;
	// ここで重複がないか確認する
	while( currentTxCB->next() != nullptr && currentTxCB != txcb )	{ 
		currentTxCB = currentTxCB->next();
		cnt++;
	}

	if( currentTxCB == nullptr ){
		currentTxCB->next( txcb );
		txcb->prev( currentTxCB );
		goto isAutoScale;
	}

	// すでに登録されていた場合 リスト方式であるから登録できないことは基本的にない
	return false;


	isAutoScale:
	if( cnt >= _scaleSize ){
		autoScale( _startCB );
		delete this;
	}

	return true;
}




TxCB *TxCBBucket::find( unsigned char *txID )
{

	TxCB *currentTxCB = _startCB;

	while( currentTxCB != nullptr )
	{
		if( TxCBTableManager::txcmp( txID , currentTxCB->txID() ) == true ) return currentTxCB;
		currentTxCB = currentTxCB->next();
	}

	return nullptr;
}



void TxCBBucket::autoScale( TxCB *startCB )
{

	TxCBTable *newTxCBTable = new TxCBTable( _parentTable->layer() + 1 );
	
	TxCB *currentTxCB = startCB;

	while( currentTxCB->next() != nullptr ){
		newTxCBTable->add( currentTxCB );
		currentTxCB = currentTxCB->next();
	}


	_parentTable->swapGenericListElem( std::make_pair( static_cast<int>(GENERIC_LIST_ELEM_TYPE::TABLE), newTxCBTable)  ,  TxCBTableManager::convertSymbol(_symbol)) ;
	return;
}



}; // close tx namespace
