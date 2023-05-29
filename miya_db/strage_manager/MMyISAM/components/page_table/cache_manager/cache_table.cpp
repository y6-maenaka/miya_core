#include "cache_table.h"

#include "../mapper/mapper.h"


namespace miya_db{



CacheTable::CacheTable( int fd )
{
	_mapper = new Mapper( fd );
	
	return;
}



}; // close miya_db namespace




