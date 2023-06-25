#include "overlay_memory_manager.h"

#include "./overlay_ptr.h"



namespace miya_db
{








void *OverlayMemoryManager::convert( optr *src )
{
	unsigned char* ret;

	ret = (*_cacheTable)[src->frame()];

	ret += src->offset();

	return (void *)ret;
}














};
