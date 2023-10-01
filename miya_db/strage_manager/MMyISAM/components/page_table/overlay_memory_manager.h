#ifndef B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40
#define B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40


#include <string>
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
	OverlayMemoryAllocator *_memoryAllocator;

	int init( int dataFileFD , int freeListFileFD );

public:
	OverlayMemoryManager( int dataFileFD , int freeListFileFD );
	OverlayMemoryManager( const char* targetFilePath );
	OverlayMemoryManager( std::string fileName );

	OverlayMemoryAllocator* memoryAllocator(){ return _memoryAllocator; };

	std::shared_ptr<optr> allocate( unsigned long size );
	void deallocate( optr *target );

	std::shared_ptr<optr> get( unsigned char* oAddr );
};


}; // close miya_db namespace


#endif // B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40


;
