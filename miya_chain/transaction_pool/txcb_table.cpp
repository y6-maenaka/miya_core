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
	unsigned char targetByte = target->txID().get()[ (_layer / 2) ];
	unsigned char targetSymbol = (( _layer % 2 ) == 0) ? ( (targetByte >> 4)& 0x0F ) : (targetByte & 0x0F);


	// 対象リストの要素がバケットの場合単純追加する 
	if( _bucketTableList.at(static_cast<unsigned int>(targetSymbol)).second == 1  )	
		return static_cast<TxCBBucket*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->add( target );

	// 対象のリスト要素がテーブルの場合は再起的に追加
	if( _bucketTableList.at( static_cast<unsigned int>(targetSymbol)).second == 2 )
		return static_cast<TxCBTable*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->add( target );

	return -1	;
}




std::shared_ptr<TxCB> TxCBTable::find( std::shared_ptr<TxCB> target )
{
	unsigned char targetByte = target->txID().get()[ (_layer / 2) ];
	unsigned char targetSymbol = (( _layer % 2 ) == 0) ? ( (targetByte >> 4)& 0x0F ) : (targetByte & 0x0F);

	
	// ( _bucketTableList[static_cast<unsigned int>(targetSymbol)].second ) == 1 ; ->対象はテーブル -> 再帰的に検索する
	if( _bucketTableList.at(static_cast<unsigned int>(targetSymbol)).second == 1  )	
		return static_cast<TxCBBucket*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->find( target );


	// ( _bucketTableList[static_cast<unsigned int>(targetSymbol)].second ) == 2 : ->対象はバケット -> 単純に検索する
	if( _bucketTableList.at( static_cast<unsigned int>(targetSymbol)).second == 2 )
		return static_cast<TxCBTable*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->find( target );

	return nullptr;
}




std::shared_ptr<TxCBBucket> TxCBTable::findBucket( std::shared_ptr<TxCB> target )
{
	unsigned char targetByte = target->txID().get()[ (_layer / 2) ];
	unsigned char targetSymbol = (( _layer % 2 ) == 0) ? ( (targetByte >> 4)& 0x0F ) : (targetByte & 0x0F);

	// ( _bucketTableList[static_cast<unsigned int>(targetSymbol)].second ) == 1 ; ->対象はテーブル -> 再帰的に検索する
	if( _bucketTableList.at(static_cast<unsigned int>(targetSymbol)).second == 1  )	
		return std::make_shared<TxCBBucket>( *(static_cast<TxCBBucket*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)) );


	// ( _bucketTableList[static_cast<unsigned int>(targetSymbol)].second ) == 2 : ->対象はバケット -> 単純に検索する
	if( _bucketTableList.at( static_cast<unsigned int>(targetSymbol)).second == 2 ) // 対象がテーブルだった場合
		return static_cast<TxCBTable*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->findBucket( target );

	return nullptr;
}


};

