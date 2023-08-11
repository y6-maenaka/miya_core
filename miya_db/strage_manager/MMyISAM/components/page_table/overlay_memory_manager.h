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

	int init( int targetFD );

public:
	OverlayMemoryManager( int targetFD );
	OverlayMemoryManager( const char* oswapFilePath );
	~OverlayMemoryManager();

	optr* allocate( unsigned long size );
	void deallocate( optr *target , unsigned long size );

};


}; // close miya_db namespace


#endif // B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40


;
