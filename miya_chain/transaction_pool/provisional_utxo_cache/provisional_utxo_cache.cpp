#include "provisional_utxo_cache.h"

#include "../txcb.h"
#include "../../transaction/p2pkh/p2pkh.h"


namespace miya_chain
{






std::shared_ptr<TxCB> ProvisionalUTxOCache::find( std::shared_ptr<unsigned char> txID , unsigned short index )
{
	std::shared_ptr<OutPointPair> targetPair = std::make_shared<OutPointPair>(std::make_pair(txID, index));
	auto retItr = _pUTxOMap.find( targetPair ); 

	if( _pUTxOMap.end() == retItr ) return nullptr; 
	return retItr->second;
};





void ProvisionalUTxOCache::remove( std::shared_ptr<unsigned char> txID , unsigned short index )
{
	std::shared_ptr<OutPointPair> targetPair = std::make_shared<OutPointPair>(std::make_pair(txID, index));

	return remove( targetPair );	
}





void ProvisionalUTxOCache::remove( std::shared_ptr<OutPointPair> targetPair )
{
	auto retItr = _pUTxOMap.find( targetPair );
	_pUTxOMap.erase( targetPair );
}



void ProvisionalUTxOCache::remove( std::shared_ptr<TxCB> target )
{
	std::shared_ptr<OutPointPair> targetPair;

	for( int i=0; i<target->tx()->TxOutCnt(); i++ )
	{
		targetPair = std::make_shared<OutPointPair>(std::make_pair(target->txID(), i ));
		remove( targetPair );
	}
}




void ProvisionalUTxOCache::add( std::shared_ptr<TxCB> target )
{

	std::shared_ptr<OutPointPair> targetPair;

	for( int i=0; i<target->tx()->TxOutCnt(); i++ ) // 
	{
		targetPair = std::make_shared<OutPointPair>(std::make_pair(target->txID(), i ));
		_pUTxOMap.insert( std::make_pair(targetPair, target) );
	}
	// トランザクションが含んでいるtx_out分を追加する必要がある
}





};
