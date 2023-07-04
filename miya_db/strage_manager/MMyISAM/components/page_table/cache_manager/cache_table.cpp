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
	for( unsigned int i=0; i<CACHE_BLOCK_COUNT; i++){ // とりあえず線形探索で
		if( _cacheList._cacheingList[i] == frame ) return i;
	}

	return -1;		
}






CacheTable::CacheTable( int fd )
{
	_mapper = new Mapper( fd );

	memset( &_cacheList, 0x00 , sizeof(_cacheList) );
	std::fill( _cacheList._cacheingList, _cacheList._cacheingList + CACHE_BLOCK_COUNT, -1);
	return; 
};




void* CacheTable::convert( optr *src )
{
	std::cout << "Process in CacheTable::convert()" << "\n";
	unsigned short frame = src->frame();

	unsigned char* ret; short idx;
	std::cout << "cacheFind -> " << cacheFind(frame) << "\n";
	if( (idx = cacheFind( frame )) < 0 ){
		idx = pageFault( frame ); // キャッシュテーブルに対象フレームのキャッシュブロックが存在しなかったら -> Page Fault
	} 
	ret = static_cast<unsigned char*>(_cacheList._mappingList[idx]);
	ret += src->offset();

	// LRUの更新
	_cacheList._LRU.reference( idx );
	_cacheList._LRU.incrementReferencePtr();
	
	return (void *)ret;

}


/* この辺りは無闇に触らない */

unsigned short CacheTable::pageFault( unsigned int frame )
{
	std::cout << "[ PAGE FAULT ]" << "\n";
	unsigned int outPageIdx = _cacheList._LRU.targetOutPage();
	std::cout << "target out page -> " << outPageIdx << "\n";

	pageOut(  outPageIdx ); std::cout << "[ Page Out ] :: " << outPageIdx << "\n";
	pageIn( outPageIdx , frame ); std::cout << "[ Page In ] :: " << outPageIdx << "\n";
	
	return outPageIdx; // 要修正
}



int CacheTable::pageOut( unsigned short outIdx ) 
{
	_mapper->unmap( _cacheList._mappingList[outIdx] );
	_cacheList._mappingList[outIdx] = nullptr;

	_cacheList._cacheingList[outIdx] = -1;
	_cacheList._LRU._invalidCacheList[outIdx] = false;

	return static_cast<int>(outIdx);
}


int CacheTable::pageIn( unsigned short outIdx ,unsigned int inFrame ) // 入れ替えるキャッシュブロックインデックス, 入れるフレーム番号
{
	_cacheList._mappingList[outIdx] = _mapper->map( inFrame );
	_cacheList._cacheingList[outIdx] = inFrame; // キャッシュしているフレームの更新
	_cacheList._LRU._invalidCacheList[outIdx] = true;

	return static_cast<int>(outIdx);
}
/* =================================================================== */



void CacheTable::cacheingList()
{
	std::cout << "==== [ CACHING STATUS ] ====" << "\n";
	for( int i=0; i<CACHE_BLOCK_COUNT; i++)
	{
		std::cout << _cacheList._cacheingList[i] << " |";
	} std::cout << "\n";
	std::cout << "==== ==== ==== ==== ====" << "\n";
}



void CacheTable::invalidList()
{
	std::cout << "==== [ INVALID STATUS ] ====" << "\n";
	std::cout << "referencePtr -> " << _cacheList._LRU._referencePtr << "\n";
	for( int i=0; i<CACHE_BLOCK_COUNT; i++)
	{
		std::cout <<  _cacheList._LRU._invalidCacheList[i] << " |";
	} std::cout << "\n";
	std::cout << "==== ==== ==== ==== ====" << "\n";

}

}; // close miya_db namespace




