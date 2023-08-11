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
	unsigned short frame = src->frame();

	unsigned char* ret; short idx;

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



void* convert( unsigned char* src )
{
	unsigned short frame = 0;
	unsigned short offset = 0;

	unsigned short exponentialList[5] = {64, 32, 16, 8, 0};


	frame += static_cast<unsigned long>( src[0]) * pow(2, exponentialList[0] );
	frame += static_cast<unsigned long>( src[1]) * pow(2, exponentialList[1] );
	frame += static_cast<unsigned long>( src[2]) * pow(2, exponentialList[2] );

	offset += static_cast<unsigned long>( src[3]) * pow(2, exponentialList[0] );
	offset += static_cast<unsigned long>( src[4]) * pow(2, exponentialList[1] );

}


/* この辺りは無闇に触らない */

unsigned short CacheTable::pageFault( unsigned int frame )
{
	unsigned int outPageIdx = _cacheList._LRU.targetOutPage();

	pageOut(  outPageIdx );
	pageIn( outPageIdx , frame );
	
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




