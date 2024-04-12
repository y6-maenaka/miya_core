#include "overlay_memory_manager.h"

#include "./cache_manager/cache_table.h"
#include "./overlay_memory_allocator.h"

#include "../../../../../share/hash/sha_hash.h"


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


void OverlayMemoryManager::dbState( std::shared_ptr<unsigned char> target )
{
  MetaBlock* metaBlock = _memoryAllocator->metaBlock();
  return metaBlock->dbState( target );
}

std::shared_ptr<unsigned char> OverlayMemoryManager::dbState()
{
  MetaBlock* metaBlock = _memoryAllocator->metaBlock();
  std::shared_ptr<unsigned char> ret;
  metaBlock->dbState( &ret );
  return ret;
}

size_t OverlayMemoryManager::dbState( std::shared_ptr<unsigned char> *ret )
{
  MetaBlock* metaBlock = _memoryAllocator->metaBlock();
  return metaBlock->dbState( ret );
}

void OverlayMemoryManager::syncDBState()
{
  uint32_t timestamp = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
  unsigned char ctimestamp[sizeof(timestamp)];
  ctimestamp[0] = static_cast<unsigned char>((timestamp>>24) & 0xFF);
  ctimestamp[1] = static_cast<unsigned char>((timestamp>>16) & 0xFF);
  ctimestamp[2] = static_cast<unsigned char>((timestamp>>8) & 0xFF);
  ctimestamp[3] = static_cast<unsigned char>((timestamp) & 0xFF);
  
  std::shared_ptr<unsigned char> digest; size_t digestLength;
  digestLength = hash::SHAHash( ctimestamp , sizeof(timestamp) , &digest , "sha1" );

  MetaBlock* metaBlock = _memoryAllocator->metaBlock();
  metaBlock->dbState( digest );
}




}; // close miya_db namespace
