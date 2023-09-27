#include "overlay_memory_manager.h"

#include "./cache_manager/cache_table.h"
#include "./overlay_memory_allocator.h"


namespace miya_db{





OverlayMemoryManager::OverlayMemoryManager( const char* targetFilePath )
{


	// ファイル名から２つのパスを生成する

	//int fd = open( oswapFilePath , O_RDWR, (mode_t)0600 );

	//init( fd );


}


OverlayMemoryManager::OverlayMemoryManager( int dataFileFD , int freeListFileFD )
{
	init( dataFileFD , freeListFileFD );
}




int OverlayMemoryManager::init( int dataFileFD , int freeListFileFD )
{
	/*
	if( targetFD >= 0 )
	{
		_cacheTable = new CacheTable( targetFD );

		// primaryOptrを作成する

		optr *primaryOptr = new optr;
		primaryOptr->cacheTable( _cacheTable );

		_memoryAllocator = new OverlayMemoryAllocator( primaryOptr );

		return 0;
	}
	*/




	_memoryAllocator = new OverlayMemoryAllocator( dataFileFD , freeListFileFD );


	return 0;
};







std::shared_ptr<optr> OverlayMemoryManager::allocate( unsigned long size )
{
	/* フリーメモリリンクリストの先頭にアクセス -> 通常0x000000 */

	return _memoryAllocator->allocate( size );
}



void OverlayMemoryManager::deallocate( optr* targetOptr )
{
	return _memoryAllocator->deallocate( targetOptr );
}



std::shared_ptr<optr> OverlayMemoryManager::get( unsigned char* oAddr )
{
	return _memoryAllocator->get(oAddr);
}


}; // close miya_db namespace
