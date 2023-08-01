#include "overlay_memory_manager.h"

#include "./cache_manager/cache_table.h"


namespace miya_db{


OverlayMemoryManager::OverlayMemoryManager( const char* oswapFilePath )
{
	int fd = open( oswapFilePath , O_RDWR, (mode_t)0600 );
	_cacheTable = new CacheTable( fd );
}



OverlayMemoryManager::~OverlayMemoryManager()
{
	delete _cacheTable;
}




optr* OverlayMemoryManager::newOptr( unsigned int size )
{
	/* フリーメモリリンクリストの先頭にアクセス -> 通常0x000000 */
	
	
}



}; // close miya_db namespace
