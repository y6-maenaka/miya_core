#include "txcb_table.h"

#include "../txcb/txcb.h"
#include "../txcb_table_manager.h"
#include "../txcb_bucket/txcb_bucket.h"


namespace tx{





void TxCBTable::swapGenericListElem( std::pair<int,void*> elem , int index )
{
	_genericList[index] = elem;
}




unsigned int TxCBTable::layer()
{
	return _layer;
}



TxCBTable::TxCBTable( unsigned int layer )
{
	_layer = layer;

	// とりあえず作成された初めは全てbucketで初期化する
	for( unsigned int i=0; i<16; i++){
		// int to symbol( usnigned char ( 1 - F ))
		TxCBBucket *newBucket = new TxCBBucket( static_cast<unsigned char>( i & 0x0F ), this );

		_genericList[i] = std::make_pair( static_cast<int>(GENERIC_LIST_ELEM_TYPE::BUCKET)  , newBucket  );
	}
	return;
}





bool TxCBTable::add( TxCB* txcb )
{
	unsigned char symbol_C = TxCBTableManager::getSymbol( txcb->txID() , _layer );
	unsigned int symbol_I = TxCBTableManager::convertSymbol( symbol_C );


	// 下にある要素がバケットである場合
	if( (_genericList[ symbol_C ]).first ==  static_cast<int>(GENERIC_LIST_ELEM_TYPE::BUCKET)  )
	{
		return ((TxCBBucket *)((_genericList[ symbol_C ]).second))->push( txcb );
	}


	// 下にある要素がテーブルである場合 
	return ((TxCBTable *)(((_genericList[ symbol_C ]).second)))->add( txcb ); // 再帰的に呼び出す
}









TxCB *TxCBTable::find( unsigned char *txID )
{
	
	unsigned char symbol_C = TxCBTableManager::getSymbol( txID , _layer );
	unsigned int symbol_I = TxCBTableManager::convertSymbol( symbol_C );

	if( (_genericList[ symbol_C ]).first ==  static_cast<int>(GENERIC_LIST_ELEM_TYPE::BUCKET)  )
	{
		return ((TxCBBucket *)((_genericList[ symbol_C ]).second))->find( txID );
	}


	// 下にある要素がテーブルである場合 
	return ((TxCBTable *)(((_genericList[ symbol_C ]).second)))->find( txID ); // 再帰的に呼び出す
}





}; // close tx namespace
