#include "./txcb_table.h"

#include "./txcb.h"
#include "./txcb_bucket.h"

namespace miya_chain
{



unsigned int TxCBTable::layer()
{
	return _layer;
}




TxCBTable::TxCBTable( unsigned int layer )
{
	_layer = layer;
}



void TxCBTable::bucketTableList( void* target , int type ,int index )
{
	_bucketTableList.at(index) = std::make_pair( target , type );
}



int TxCBTable::add( std::shared_ptr<TxCB> target )
{
	unsigned char targetByte = target->txID()[ (_layer / 2) ];
	unsigned char targetSymbol = (( _layer % 2 ) == 0) ? ( (targetByte >> 4)& 0x0F ) : (targetByte & 0x0F);


	return static_cast<TxCBBucket*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->add( target );


	// ( _bucketTableList[static_cast<unsigned int>(targetSymbol)].second ) == 2 : ->対象はバケット -> 単純に検索する
	return static_cast<TxCBTable*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->add( target );

	
}




std::shared_ptr<TxCB> TxCBTable::find( std::shared_ptr<TxCB> target )
{
	unsigned char targetByte = target->txID()[ (_layer / 2) ];
	unsigned char targetSymbol = (( _layer % 2 ) == 0) ? ( (targetByte >> 4)& 0x0F ) : (targetByte & 0x0F);

	
	// ( _bucketTableList[static_cast<unsigned int>(targetSymbol)].second ) == 1 ; ->対象はテーブル -> 再帰的に検索する
	return static_cast<TxCBBucket*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->find( target );


	// ( _bucketTableList[static_cast<unsigned int>(targetSymbol)].second ) == 2 : ->対象はバケット -> 単純に検索する
	return static_cast<TxCBTable*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->find( target );
}




};

