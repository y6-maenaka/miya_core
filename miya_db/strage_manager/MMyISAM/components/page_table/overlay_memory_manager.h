#ifndef B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40
#define B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40


#include <unistd.h>
#include <fcntl.h>
#include <filesystem>


#include "overlay_ptr.h"


namespace miya_db{


class CacheTable;
class OverlayMemoryAllocator;






class OverlayMemoryManager
{
private:
	CacheTable *_cacheTable;
	OverlayMemoryAllocator *_memoryAllocator;

public:
	OverlayMemoryManager( const char* oswapFilePath );
	~OverlayMemoryManager();

	optr* newOptr( unsigned int size );
	void deleteOptr( optr *target );

};


}; // close miya_db namespace


#endif // B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40


;
