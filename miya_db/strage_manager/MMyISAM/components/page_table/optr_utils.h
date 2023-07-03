#ifndef FDDF8152_AB99_4673_A5E8_9F98C130F129
#define FDDF8152_AB99_4673_A5E8_9F98C130F129


#include <cstddef>

#include "./overlay_ptr.h"



namespace miya_db
{

class optr;
class OverlayMemoryManager;




optr *omemcpy( optr *dest , optr *src , size_t n )
{
	for( int i=0; i<n ; i++ )
		dest[i] = src[i];

	return dest;
}



optr* omemcpy( optr *dest , void *src , size_t n )
{

	for( int i=0; i<n; i++)	 (*dest)[i] = static_cast<unsigned char*>(src)[i]

	return nullptr;
};




void* omemcpy( void* dest , optr *src, size_t n )
{
	for( int i=0; i<n; i++)
		*(static_cast<unsigned char*>(dest)[i]) = src[i];
}


// offset + n でframeを跨がない場合は,内部でmemcpyする




}; // close miya_db namespace


#endif // FDDF8152_AB99_4673_A5E8_9F98C130F129



