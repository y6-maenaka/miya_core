#include "overlay_memory_manager.h"

#include "./cache_manager/cache_table.h"
#include "./overlay_memory_allocator.h"


namespace miya_db{





OverlayMemoryManager::OverlayMemoryManager( int dataFileFD , int freeListFileFD )
{
	init( dataFileFD , freeListFileFD );
}


OverlayMemoryManager::OverlayMemoryManager( std::string filePath )
{
	//std::shared_ptr<char> dataFilePath = std::shared_ptr<char>( new char[PATH_MAX] );
	//std::shared_ptr<char> freeListFilePath = std::shared_ptr<char>( new char[PATH_MAX] );
	//sprintf( dataFilePath.get() ,  "../miya_db/table_files/%s/%s.oswap", filePath.c_str(), filePath.c_str() );
	//sprintf( freeListFilePath.get() ,  "../miya_db/table_files/%s/%s.ofl", filePath.c_str(), filePath.c_str() );
	
	std::string dataFilePath = filePath + ".oswap";
	std::string freeListFilePath = filePath + ".ofl";

	//int dataFd = open( dataFilePath.get(), O_RDWR | O_CREAT , (mode_t)0600 ); // セーフにするには新規作成は行わない方が良い
	int dataFd = open( dataFilePath.c_str() , O_RDWR | O_CREAT , (mode_t)0600 ); // セーフにするには新規作成は行わない方が良い
	//int freeListFd = open( freeListFilePath.get(), O_RDWR | O_CREAT , (mode_t)0600 ); 
	int freeListFd = open( freeListFilePath.c_str() , O_RDWR | O_CREAT , (mode_t)0600 ); 


	std::cout << "----------------------------------" << "\n";
	std::cout << dataFilePath.c_str() << "\n";
	std::cout << dataFd << "\n";
	std::cout << "----------------------------------" << "\n";
	std::cout << freeListFilePath.c_str() << "\n";
	std::cout << freeListFd << "\n";
	std::cout << "-----------------------------------" << "\n";


	init( dataFd , freeListFd );
}



int OverlayMemoryManager::init( int dataFileFD , int freeListFileFD )
{
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





std::shared_ptr<optr> OverlayMemoryManager::wrap( std::shared_ptr<optr> target )
{
	target->cacheTable( _memoryAllocator->dataCacheTable() );

	return target;
}



void OverlayMemoryManager::clear()
{
	_memoryAllocator->clear();
}



std::shared_ptr<CacheTable> OverlayMemoryManager::dataCacheTable()
{
	return _memoryAllocator->dataCacheTable();
}


std::shared_ptr<CacheTable> OverlayMemoryManager::freeListCacheTable()
{
	return _memoryAllocator->freeListCacheTable();
}





}; // close miya_db namespace
