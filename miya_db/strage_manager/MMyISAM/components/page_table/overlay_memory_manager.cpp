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


OverlayMemoryManager::OverlayMemoryManager( std::string fileName )
{
	std::shared_ptr<char> dataFilePath = std::shared_ptr<char>( new char[256] );
	std::shared_ptr<char> freeListFilePath = std::shared_ptr<char>( new char[256] );
	sprintf( dataFilePath.get() ,  "../miya_db/table_files/%s/%s.oswap", fileName.c_str(), fileName.c_str() );
	sprintf( freeListFilePath.get() ,  "../miya_db/table_files/%s/%s.ofl", fileName.c_str(), fileName.c_str() );


	std::cout << dataFilePath.get() << "\n";
	std::cout << freeListFilePath.get() << "\n";


	int dataFd = open( dataFilePath.get(), O_RDWR , (mode_t)0600 );
	int freeListFd = open( freeListFilePath.get(), O_RDWR , (mode_t)0600 );

	init( dataFd , freeListFd );
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

	printf("%p\n", _memoryAllocator );
	std::cout << "allocate() called from memory manager" << "\n";
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
