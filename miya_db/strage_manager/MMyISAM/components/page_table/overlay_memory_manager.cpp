#include "overlay_memory_manager.h"

#include "./cache_manager/cache_table.h"
#include "./overlay_memory_allocator.h"


namespace miya_db{





OverlayMemoryManager::OverlayMemoryManager( const char* oswapFilePath )
{
	int fd = open( oswapFilePath , O_RDWR, (mode_t)0600 );
	
	init( fd );
}


OverlayMemoryManager::OverlayMemoryManager( int targetFD )
{
	init( targetFD );
}




int OverlayMemoryManager::init( int targetFD )
{
	if( targetFD >= 0 )
	{
		_cacheTable = new CacheTable( targetFD );


		const char str[5] = { 0 , 0 , 0 , 0 ,1 };
		optr _tmp(  const_cast<unsigned char *>(reinterpret_cast<const unsigned char*>(str)) );
		_tmp.cacheTable( _cacheTable );
		printf("%c\n", _tmp.value() );
		return 0;
	

		// primaryOptrを作成する
		
		optr *primaryOptr = new optr;
		primaryOptr->cacheTable( _cacheTable );
		primaryOptr->addr(0);

		_memoryAllocator = new OverlayMemoryAllocator( primaryOptr );
		

		return 0;
	}

	
	return -1;
};





OverlayMemoryManager::~OverlayMemoryManager()
{
	delete _cacheTable;
}




optr* OverlayMemoryManager::allocate( unsigned long size )
{
	/* フリーメモリリンクリストの先頭にアクセス -> 通常0x000000 */
	return nullptr;
}


void OverlayMemoryManager::deallocate( optr* targetOptr , unsigned long size ){

}



}; // close miya_db namespace
