#include "cache_table.h"

#include "./mapper/mapper.h"




/*

 [ whoel ]	
 65536 * 10 = 655360 [ bytse ] 

 [ 1 block ] 
 65536 [ bytes ]

*/





namespace miya_db{



CacheTable::CacheTable( int fd )
{
				
	_mapper = new Mapper( fd );
	
	return;
	
}



}; // close miya_db namespace




