#include "cache_manager.h"



CacheManager::CacheManager( int fd )
{
	_mapper = new Mapper( fd );
	
	return;
}




