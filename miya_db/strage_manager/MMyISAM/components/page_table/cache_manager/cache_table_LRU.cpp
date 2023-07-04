#include "cache_table_LRU.h"

#include "cache_table.h"


namespace miya_db
{




void LRU::incrementReferencePtr() // 参照ポインタをインクリメントする
{
	_invalidCacheList[_referencePtr] = false; // 参照フラグを下げる
	_referencePtr = (_referencePtr+1) % CACHE_BLOCK_COUNT; // 参照ポインタを進めす
}



void LRU::reference( unsigned short idx )
{
	_invalidCacheList[idx] = true; // 参照ビットを立てる
}




unsigned short LRU::targetOutPage()
{
	int i =_referencePtr; // referencePtrは次に参照する箇所を参照しているため,一つ進める
	do
	{
		if( _invalidCacheList[ (i%CACHE_BLOCK_COUNT) ] == false ) return (i%CACHE_BLOCK_COUNT);
		i++;
	}while(i != ((_referencePtr - 1 + CACHE_BLOCK_COUNT)%CACHE_BLOCK_COUNT));

	return _referencePtr; // 全ての参照ビットが立っていたら次の参照箇所をページアウト対象とする
}


LRU::LRU()
{
	std::fill( _invalidCacheList, _invalidCacheList + CACHE_BLOCK_COUNT , false);
}



}; // close miya_db namespace
