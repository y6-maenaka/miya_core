#include "virtual_addr.h"

#include "./page_table.h"



namespace miya_db{




unsigned char *VirtualAddr::frame()
{
	return &_virtualAddr;
}



unsigned char *VirtualAddr::offset()
{
	//return &(_virtualAddr[3]);
	return nullptr;
}


void *VirtualAddr::toPtr( bool isCacheInvalidation ) 
{
	// キャッシュ無視ではない & キャ酒がある
	if( !(isCacheInvalidation) && _addrCache != nullptr )
		return _addrCache;
		
	return _pageTable->inquire( &_virtualAddr );
}


/*
void *VirtualAddr::operator ^() const
{
	return toPtr();
}
*/


}; // close miya_db namesapce
