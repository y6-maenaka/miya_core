#include "overlay_memory_allocator.h"
#include "./optr_utils.h"



namespace miya_db
{




/*
 ( free block control block ) fbcb [15bytes]
 - prevFreeBlockControlBLockPtr
 - nextFreeBlockControlBlockPtr (5) bytes
 - freeBlockEndPtr
*/



/*
 -----------------------------------------------------------
	管理領域 | 先頭フリーブロック | フリーブロック管理ブロック
 -----------------------------------------------------------
*/





FreeBlockControlBlock::FreeBlockControlBlock( optr *optr )
{
	_optr = optr;
}







std::unique_ptr<FreeBlockControlBlock> FreeBlockControlBlock::prevControlBlock()
{
	if( _optr == nullptr )  return nullptr;

	// std::unique_ptr<optr> nextControlBlock( _optr );
	return std::unique_ptr<FreeBlockControlBlock>( new FreeBlockControlBlock(_optr) );
}





std::unique_ptr<FreeBlockControlBlock> FreeBlockControlBlock::nextControlBlock()
{
	if( _optr == nullptr )  return nullptr;

	// ユニークポインタの引数に動的に確保したアドレスを指定する必要がある
	//std::unique_ptr<optr> prevControlBlock( _optr+5 ); // 先頭から5バイト進めた位置
	//return prevControlBlock;
	
	return std::unique_ptr<FreeBlockControlBlock>( new FreeBlockControlBlock(_optr + 5 ) );
}




std::unique_ptr<optr> FreeBlockControlBlock::freeBlockEnd()
{	
	if( _optr == nullptr )  return nullptr;

	std::unique_ptr<optr> freeBlockEnd( _optr+10 );
	return freeBlockEnd;
}




unsigned int FreeBlockControlBlock::freeBlockSize()
{
	if( _optr == nullptr )  return 0;
	
	unsigned short exponentialList[6] = {64, 32, 16, 8, 0};
	// 現在のポインタ位置とEndPtrの差分を計算する事で得る	
	uint64_t ulControlBlockOPtr = 0; 
	uint64_t ulFreeBlockEndOptr = 0;

	// フリー領域管理ブロックのポインタ
	ulControlBlockOPtr += static_cast<uint64_t>(_optr->optr()[0]) * pow(2, exponentialList[0]) ;
	ulControlBlockOPtr += static_cast<uint64_t>(_optr->optr()[1]) * pow(2, exponentialList[1]) ;
	ulControlBlockOPtr += static_cast<uint64_t>(_optr->optr()[2]) * pow(2, exponentialList[2]) ;
	ulControlBlockOPtr += static_cast<uint64_t>(_optr->optr()[3]) * pow(2, exponentialList[3]) ;
	ulControlBlockOPtr += static_cast<uint64_t>(_optr->optr()[4]) * pow(2, exponentialList[4]) ;


	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->optr()[0]) * pow(2, exponentialList[0]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->optr()[1]) * pow(2, exponentialList[1]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->optr()[2]) * pow(2, exponentialList[2]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->optr()[3]) * pow(2, exponentialList[3]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->optr()[4]) * pow(2, exponentialList[4]) ;

	return static_cast<unsigned long>(ulFreeBlockEndOptr - (ulControlBlockOPtr+15)); // コントロールブロック分のサイズを加算する
}
















std::unique_ptr<optr> OverlayMemoryAllocator::allocate( unsigned int allocateSize )
{
	// 一旦allocateSizeをunsigned char型に変換する
	unsigned char ucAllocateSize[5];
	ucAllocateSize[0] = 0;
	ucAllocateSize[1] = (allocateSize >> 24 ) & 0xFF;
	ucAllocateSize[2] = (allocateSize >> 16 ) & 0xFF;
	ucAllocateSize[3]	= (allocateSize >> 8 ) & 0xFF;
	ucAllocateSize[4] = (allocateSize) & 0xFF;


	std::unique_ptr<FreeBlockControlBlock> ret = findFreeBlock( std::unique_ptr<FreeBlockControlBlock>( new FreeBlockControlBlock( new optr(0)) ) , allocateSize );

	return nullptr;
}




std::unique_ptr<FreeBlockControlBlock> findFreeBlock( std::unique_ptr<FreeBlockControlBlock> targetControlBlock, unsigned int allocateSize )
{
	if( targetControlBlock->freeBlockSize() < allocateSize )
		return findFreeBlock( targetControlBlock->nextControlBlock() , allocateSize );

	return targetControlBlock;
}







void OverlayMemoryAllocator::unallocate( unsigned char* target , unsigned int size  )
{


	return;
}





unsigned int getFreeBlockSize( optr *freeBlockControlBlockHead )
{

	unsigned char uFreeBlockStartPtr[5];	
	unsigned char uFreeBlockEndPtr[5];

	memcpy( uFreeBlockStartPtr , freeBlockControlBlockHead->optr() , 5 );


	return 0;
}




unsigned int freeBlockSize( unsigned char *currentOptr , unsigned char *retOptr )
{	
	if( retOptr == nullptr ) return 99999999;// 最大値を返却する
																	
	return 0;
}










}; // close miya_db namespace
