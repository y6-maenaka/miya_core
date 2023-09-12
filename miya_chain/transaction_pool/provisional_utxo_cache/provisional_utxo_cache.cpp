#include "provisional_utxo_cache.h"

#include "../txcb.h"
#include "../../transaction/p2pkh/p2pkh.h"
#include "../../transaction/tx/tx_in.h"
#include "../../transaction/tx/tx_out.h"


namespace miya_chain
{






std::shared_ptr<TxCB> ProvisionalUTxOCache::find( std::shared_ptr<unsigned char> txID , unsigned short index )
{
	std::shared_ptr<OutPointPair> targetPair = std::make_shared<OutPointPair>(std::make_pair(txID, index));
	auto retItr = _pUTxOMap.find( targetPair ); 

	if( _pUTxOMap.end() == retItr ) return nullptr; 
	return retItr->second;
};



std::vector< std::shared_ptr<TxCB>> ProvisionalUTxOCache::find( std::shared_ptr<tx::P2PKH> target )
{
	std::vector<std::shared_ptr<TxCB>> retVector;
	std::shared_ptr<TxCB> hitedTxCB;

	for( auto itr : target->_body._ins )
	{
		if( ( hitedTxCB = this->find( itr->inTxID(), itr->inIndex())) != nullptr )
			retVector.push_back( hitedTxCB ); // 検索対象のtxにトランザクションプール内のtxと同じutxoを参照している場合
	}

	return retVector;
}






void ProvisionalUTxOCache::remove( std::shared_ptr<unsigned char> txID , unsigned short index )
{
	std::shared_ptr<OutPointPair> targetPair = std::make_shared<OutPointPair>(std::make_pair(txID, index));

	return remove( targetPair );	
}




// これがremove核
void ProvisionalUTxOCache::remove( std::shared_ptr<OutPointPair> targetPair )
{
	auto retItr = _pUTxOMap.find( targetPair ); // 該当する暫定utxoイテレータを取得する
	_pUTxOMap.erase( targetPair ); // この実装では該当する暫定utxoを一つ消すだけで,トランザクション全てを削除することは不可能
																		
}



// 削除対象のトランザクションを指定する -> トランザクションプールマネージャから呼び出す必要がある
void ProvisionalUTxOCache::remove( std::shared_ptr<TxCB> target ) // トランザクションに含まれる暫定utxoを全て削除する
{
	std::shared_ptr<OutPointPair> targetPair;

	for( int i=0; i<target->tx()->TxOutCnt(); i++ )
	{
		targetPair = std::make_shared<OutPointPair>( std::make_pair(target->txID(), i ) );
		remove( targetPair );
	}
}









void ProvisionalUTxOCache::add( std::shared_ptr<TxCB> target )
{
	std::shared_ptr<OutPointPair> targetPair;

	
	for( int i=0; i<target->tx()->TxOutCnt(); i++ ) 
	{
		targetPair = std::make_shared<OutPointPair>(std::make_pair(target->txID(), i )); // coinbaseも含む
		_pUTxOMap.insert( std::make_pair(targetPair, target) );
	}

	// トランザクションが含んでいるtx_out分を追加する必要がある
}





};
