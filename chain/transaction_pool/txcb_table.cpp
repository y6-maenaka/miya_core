#include "./txcb_table.h"

#include "./txcb.h"
#include "./txcb_bucket.h"

namespace chain
{


TxCBTable::TxCBTable( unsigned int layer ) : _layer(layer)
{
	// 配下に1-Fまでのシンボルを持つbucketをTxCBBucketを作成する
	unsigned char symbol = 0x00;
	for( int i=0; i<16; i++ )
	{
		_containerArray.at(i) = std::shared_ptr<TxCBBucket>( new TxCBBucket( symbol, this ) );
		// printf("Bucket Created With symbol :: %02X\n", symbol );
		symbol =  symbol + 0x01;
	} // テーブルの初期化

}


unsigned int TxCBTable::layer()
{
	return _layer;
}

/*
void TxCBTable::bucketTableList( void* target , int type ,int index )
{
	_bucketTableList.at(index) = std::make_pair( target , type );
}
*/


void TxCBTable::txContainer( size_t index , TxCBContainer target )
{
	if( index >= _containerArray.size() || index < 0 )	return;

	_containerArray.at(index) = target;
}


int TxCBTable::add( std::shared_ptr<TxCB> target )
{
	unsigned char targetByte = target->txID().get()[ (_layer / 2) ]; // シンボルの対象インデックスを求める
	unsigned char targetSymbol = (( _layer % 2 ) == 0) ? ( (targetByte & 0xf0 ) >> 4 ) : (targetByte & 0x0f);
	size_t targetIndex = static_cast<size_t>( targetSymbol );

	auto targetBucket{ std::get_if<std::shared_ptr<TxCBBucket>>( &(_containerArray.at(targetIndex))) }; // 配下にバケットが配置されている場合
	if( targetBucket )
	{
		std::cout << "\x1b[32m" << "TxCBTable::add() 配下Bucket 単純追加" << "\x1b[39m" << "\n";
		return (*targetBucket)->add( target );
	}

	auto targetTable{ std::get_if<std::shared_ptr<TxCBTable>>( &(_containerArray.at(targetIndex))) }; // 配下にさらにテーブルが配置されている場合
	if( targetTable )
	{
		std::cout << "\x1b[34m" << "TxCBTable::add() 配下Table 再帰追加" << "\x1b[39m" << "\n";
		return (*targetTable)->add( target );
	}
	/*
	// 対象リストの要素がバケットの場合単純追加する 
	if( _bucketTableList.at(static_cast<unsigned int>(targetSymbol)).second == 1  )
		return static_cast<TxCBBucket*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->add( target );

	// 対象のリスト要素がテーブルの場合は再起的に追加
	if( _bucketTableList.at( static_cast<unsigned int>(targetSymbol)).second == 2 )
		return static_cast<TxCBTable*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->add( target );
	*/

	return -1	;
}


std::shared_ptr<TxCB> TxCBTable::find( std::shared_ptr<TxCB> target )
{
	unsigned char targetByte = target->txID().get()[ (_layer / 2) ]; // シンボルの対象インデックスを求める
	unsigned char targetSymbol = (( _layer % 2 ) == 0) ? ( (targetByte & 0xf0 ) >> 4 ) : (targetByte & 0x0f);
	size_t targetIndex = static_cast<size_t>( targetSymbol );

	auto targetBucket{ std::get_if<std::shared_ptr<TxCBBucket>>( &(_containerArray.at(targetIndex))) }; // 配下にバケットが配置されている場合
	if( targetBucket )
	{
		std::cout << "\x1b[32m" << "TxCBTable::find() 配下Bucketを単純検索" << "\x1b[39m" << "\n";
		return (*targetBucket)->find( target );
	}

	auto targetTable{ std::get_if<std::shared_ptr<TxCBTable>>( &(_containerArray.at(targetIndex))) }; // 配下にさらにテーブルが配置されている場合
	if( targetTable )
	{
		std::cout << "\x1b[34m" << "TxCBTable::add() 配下Tableを再帰検索" << "\x1b[39m" << "\n";
		return (*targetTable)->find( target );
	}

	/*
	// ( _bucketTableList[static_cast<unsigned int>(targetSymbol)].second ) == 1 ; ->対象はテーブル -> 再帰的に検索する
	if( _bucketTableList.at(static_cast<unsigned int>(targetSymbol)).second == 1  )
		return static_cast<TxCBBucket*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->find( target );


	// ( _bucketTableList[static_cast<unsigned int>(targetSymbol)].second ) == 2 : ->対象はバケット -> 単純に検索する
	if( _bucketTableList.at( static_cast<unsigned int>(targetSymbol)).second == 2 )
		return static_cast<TxCBTable*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->find( target );
	*/

	return nullptr;
}

std::shared_ptr<TxCBBucket> TxCBTable::findBucket( std::shared_ptr<TxCB> target )
{
	unsigned char targetByte = target->txID().get()[ (_layer / 2) ];
	unsigned char targetSymbol = (( _layer % 2 ) == 0) ? ( (targetByte >> 4)& 0x0F ) : (targetByte & 0x0F);


	/*
	// ( _bucketTableList[static_cast<unsigned int>(targetSymbol)].second ) == 1 ; ->対象はテーブル -> 再帰的に検索する
	if( _bucketTableList.at(static_cast<unsigned int>(targetSymbol)).second == 1  )
		return std::make_shared<TxCBBucket>( *(static_cast<TxCBBucket*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)) );


	// ( _bucketTableList[static_cast<unsigned int>(targetSymbol)].second ) == 2 : ->対象はバケット -> 単純に検索する
	if( _bucketTableList.at( static_cast<unsigned int>(targetSymbol)).second == 2 ) // 対象がテーブルだった場合
		return static_cast<TxCBTable*>((_bucketTableList.at(static_cast<unsigned int>(targetSymbol))).first)->findBucket( target );
	*/

	return nullptr;
}

void TxCBTable::remove( std::shared_ptr<TxCB> target )
{
	unsigned char targetByte = target->txID().get()[ (_layer / 2) ]; // シンボルの対象インデックスを求める
	unsigned char targetSymbol = (( _layer % 2 ) == 0) ? ( (targetByte & 0xf0 ) >> 4 ) : (targetByte & 0x0f);
	size_t targetIndex = static_cast<size_t>( targetSymbol );


	auto targetBucket{ std::get_if<std::shared_ptr<TxCBBucket>>( &(_containerArray.at(targetIndex))) }; // 配下にバケットが配置されている場合
	if( targetBucket )
	{
		std::cout << "\x1b[32m" << "TxCBTable::remove() 配下Bucketを単純削除" << "\x1b[39m" << "\n";
		return (*targetBucket)->remove( target );
	}


	auto targetTable{ std::get_if<std::shared_ptr<TxCBTable>>( &(_containerArray.at(targetIndex))) }; // 配下にさらにテーブルが配置されている場合
	if( targetTable )
	{
		std::cout << "\x1b[34m" << "TxCBTable::remove() 配下Tableを再帰削除" << "\x1b[39m" << "\n";
		return (*targetTable)->remove( target );
	}
}

void TxCBTable::printTable()
{
	for( int i=0; i<16; i++ )
	{

		auto targetBucket{ std::get_if<std::shared_ptr<TxCBBucket>>( &(_containerArray.at(i))) }; // 配下にバケットが配置されている場合
		if( targetBucket )
		{
			printf("|%d| Bucket\n", i );
			continue;
		}
		else{
			printf("|%d| Table\n", i );
			continue;
		}
	}
}

};
