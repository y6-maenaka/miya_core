#ifndef B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40
#define B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40


#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <filesystem>
#include <chrono>


#include "overlay_ptr.h"


namespace miya_db{


class CacheTable;
class OverlayMemoryAllocator;






class OverlayMemoryManager
{
private:
	OverlayMemoryAllocator *_memoryAllocator;

	int init( int dataFileFD , int freeListFileFD );
protected:
	void dbState( std::shared_ptr<unsigned char> target );

public:
	OverlayMemoryManager( int dataFileFD , int freeListFileFD );
	OverlayMemoryManager( std::string filePath );

	OverlayMemoryAllocator* memoryAllocator(){ return _memoryAllocator; };

	std::shared_ptr<optr> allocate( unsigned long size );
	void deallocate( optr *target );

	std::shared_ptr<optr> get( unsigned char* oAddr );
	std::shared_ptr<optr> wrap( std::shared_ptr<optr> target );

	void clear();

	std::shared_ptr<CacheTable> dataCacheTable();
	std::shared_ptr<CacheTable> freeListCacheTable();

	std::shared_ptr<unsigned char> dbState();
	size_t dbState( std::shared_ptr<unsigned char> *ret );
	void syncDBState(); // 現段階では修正が入った時間を元に簡易的にstateCodeを生成する
};





}; // close miya_db namespace


#endif // B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40


;
