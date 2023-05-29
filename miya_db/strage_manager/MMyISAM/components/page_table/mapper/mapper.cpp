#include "mapper.h"



namespace miya_db{

/*
 
	基本的にsystemPageサイズの倍数になっていると思う

*/




/*
Mapper::Mapper( int fd )
{
	_fd = fd;
	_currentFileSize = lseek( fd , 0 , SEEK_END );

	_systemFileSize = sysconf( _SC_PAGESIZE ):
}



void* Mapper::map( unsigned int frame )
{
	if( _currentFileSize < (frame * pow(2,8*2)) ) { // 既存のファイルサイズをframeが超えるようであればファイルを拡張する
		 ftruncate(  _fd , frame * pow(2, 8*2) ); // ファイルの拡張
	}


	return mmap( NULL , pow(2,16) , PROT_READ | PROT_WRITE , MAP_SHARED , fd , (frame * pow(2,16)) );
	
}


void Mapper::unmap( void *targetAddr )
{
	// エラー発生したらどうするか
	munmap( targetAddr , pow(2,16) );
}
*/


}; // close miya_db namespace
