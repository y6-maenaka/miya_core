#include "mapper.h"



namespace miya_db{

/*
 
	基本的にsystemPageサイズの倍数になっていると思う

*/





Mapper::Mapper( int fd )
{

	_fd = fd;
	//_currentFileSize = lseek( fd , 0 , SEEK_END );

	_systemPageSize = sysconf( _SC_PAGESIZE );

}



// ファイル指定位置をメモリに読み込む 
// 読み込むキャッシュインデックス
void* Mapper::map( unsigned int frameIdx ) // mapping file point
{	
	//ftruncate(  _fd , frameIdx * pow(2, 8*2) ); // ファイルの拡張

	// if target area has not allowcated
	// get current size size	
	if( fstat( _fd, &_fileStat ) != 0 ) return nullptr;
	off_t currentFileSize = _fileStat.st_size; // クラス内部で持つのもいいが,万が一があるとファイルが削られるから

	long totalByteCount = (frameIdx+1) * pow(2, 2*8); // 最低確保しなければならないファイルサイズ 最適化していないためframeまでのファイルを確保する必要がある

	
	if( totalByteCount > currentFileSize )
		if( ftruncate( _fd , totalByteCount ) != 0 ) return nullptr; // 新たにファイルに領域を確保する

	if( ( frameIdx * DEFAULT_FRAME_CACHE_SIZE ) % _systemPageSize != 0 ){
		std::cerr << " [ Important Warning ] マッピングするファイルブロックがシステムのページサイズの倍数ではありません" << "\n";
		return nullptr;
	}
	
	void *mappedPtr = nullptr;
	mappedPtr = mmap( NULL , DEFAULT_FRAME_CACHE_SIZE  , PROT_WRITE | PROT_READ , MAP_SHARED , _fd , ( frameIdx * DEFAULT_FRAME_CACHE_SIZE ) );	
	// memset( mappedPtr , 0x00 , DEFAULT_FRAME_CACHE_SIZE ); // ?! リフレッシュの原因

	std::cout << "\x1B[33m" << "(###) FileMapped ( " << mappedPtr << " )"<< "\x1B[0m" << "\n";

	return mappedPtr;
	/* mmap がマッピングするサイズは,offset は sysconf(_SC_PAGE_SIZE) が返すページサイズの倍数でなければならない。*/
}



int Mapper::unmap( void* targetMappedPtr )
{
	// エラー発生したらどうするか
	return munmap( targetMappedPtr , DEFAULT_FRAME_CACHE_SIZE );
}


int Mapper::sync( void* targetMappedPtr )
{
	return msync( targetMappedPtr , DEFAULT_FRAME_CACHE_SIZE , MS_SYNC );
}

/*

	[ 注意点 ]

	・mapするブロックが,既存のファイルサイズを超えるようであれば，先にその領域までファイルを拡大する


*/




}; // close miya_db namespace
