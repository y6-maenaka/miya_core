#include "overlay_ptr_resolver.h"

#include "./overlay_ptr.h"
#include "./cache_manager/cache_table.h"


namespace miya_db
{



void* OverlayPtrResolver::resolve( optr *src )
{
	unsigned char* ret;

	ret = static_cast<unsigned char*>(_cacheTable->convert(src->frame()));
	ret += src->offset();

	return (void*)ret;
	
};


};
