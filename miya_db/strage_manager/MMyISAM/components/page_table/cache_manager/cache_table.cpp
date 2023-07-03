#include "cache_table.h"

#include "./mapper/mapper.h"




/*

 [ whoel ]	
 65536 * 10 = 655360 [ bytse ] 

 [ 1 block ] 
 65536 [ bytes ]

*/





namespace miya_db{







short CacheTable::cacheFind( unsigned short frame )
{
	for( unsigned int i=0; i<CACHE_BLOCK_COUNT; i++) // とりあえず線形探索で
		if( _cacheList._cacheList[i] == frame ) return i;

	return -1;		
}








void CacheTable::LRU::incrementReferencePtr()
{
	_invalidCacheList[_referencePtr] = false;
	_referencePtr = ( (_referencePtr+1) % CACHE_BLOCK_COUNT );
	_referencePtr = (_referencePtr+1) % CACHE_BLOCK_COUNT;

}



void CacheTable::LRU::reference( unsigned short idx )
{
	_invalidCacheList[idx] = true;
}


unsigned short CacheTable::LRU::outPage()
{
	unsigned int candidateIdx;

	int i =_referencePtr + 1;
	while( i != _referencePtr )
	{
		if( _invalidCacheList[ (i%CACHE_BLOCK_COUNT) ] == false ) return (i%CACHE_BLOCK_COUNT);
		i++;
	}

	return ( _referencePtr + 1 ) % CACHE_BLOCK_COUNT;
}




	




CacheTable::CacheTable( int fd )
{
	_mapper = new Mapper( fd );

	memset( &_cacheList, 0x00 , sizeof(_cacheList) );
	return;
	
};




void* CacheTable::convert( unsigned short frame )
{
	unsigned char* ret; short idx;
	if( (idx = cacheFind( frame )) < 0 ) idx = pageFault( frame );
	ret = static_cast<unsigned char*>(_cacheList._mappingList[idx]);

	// if page not existed -> pagefault
	_LRU._invalidCacheList[idx] = false;
	_LRU.incrementReferencePtr();
	
	return (void *)ret;

}


/* この辺りは無闇に触らない */

unsigned short CacheTable::pageFault( unsigned int frame )
{
	unsigned int outPageIdx = _LRU.outPage();
	pageOut(  outPageIdx );
	pageIn( outPageIdx , frame );
	
	return outPageIdx; // 要修正
}



void CacheTable::pageOut( unsigned short idx ) 
{
	_mapper->unmap( _cacheList._mappingList[idx] );
}


void CacheTable::pageIn( unsigned short idx  ,unsigned int frame ) // 入れ替えるキャッシュブロックインデックス, 入れるフレーム番号
{
	_cacheList._mappingList[idx] = _mapper->map( frame );
}
/* =================================================================== */


}; // close miya_db namespace




