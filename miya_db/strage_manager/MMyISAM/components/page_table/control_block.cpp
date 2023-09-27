#include "overlay_memory_allocator.h"


#include "./optr_utils.h"

namespace miya_db
{






/* テストOK　動作確認済み */
ControlBlock::ControlBlock( std::shared_ptr<optr> optr )
{
	_blockOptr = optr;

}

std::shared_ptr<optr> ControlBlock::blockOptr()
{
	return _blockOptr;
}



unsigned char* ControlBlock::blockAddr()
{
	return _blockOptr->addr();
}




/* ---------------------------------------------------------------------------------- */
std::unique_ptr<ControlBlock> ControlBlock::prevControlBlock()
{

	if( _blockOptr == nullptr )  return nullptr;

	unsigned char retOptrAddr[5] = {0,0,0,0,0};
	omemcpy( retOptrAddr , _blockOptr , CONTROL_BLOCK_LENGTH  );

	//optr *retOptr  = new optr( retOptrAddr ); retOptr->cacheTable( _blockOptr->cacheTable() );
	std::shared_ptr<optr> retOptr = std::shared_ptr<optr>( new optr(retOptrAddr) ); retOptr->cacheTable( _blockOptr->cacheTable() );
	ControlBlock retControlBlock( retOptr );
	

	return std::make_unique<ControlBlock>( retControlBlock );
}


void ControlBlock::prevControlBlock( ControlBlock* target )
{
	omemcpy( _blockOptr , target->blockAddr() , PREV_FREE_BLOCK_OPTR_LENGTH );
}


/* ---------------------------------------------------------------------------------- */








/* ---------------------------------------------------------------------------------- */
std::unique_ptr<ControlBlock> ControlBlock::nextControlBlock()
{
	if( _blockOptr == nullptr )  return nullptr;

	unsigned char retOptrAddr[5] = {0,0,0,0,0};
	omemcpy( retOptrAddr ,(*_blockOptr + PREV_FREE_BLOCK_OPTR_LENGTH).get() , CONTROL_BLOCK_LENGTH );
	//optr* retOptr = new optr( retOptrAddr ); retOptr->cacheTable( _blockOptr->cacheTable() );
	std::shared_ptr<optr> retOptr = std::shared_ptr<optr>( new optr(retOptrAddr) ); retOptr->cacheTable( _blockOptr->cacheTable() );
	
	ControlBlock retControlBlock( retOptr );
	return std::make_unique<ControlBlock>( retControlBlock );
}


void ControlBlock::nextControlBlock( ControlBlock* target )
{
	omemcpy( (*_blockOptr + PREV_FREE_BLOCK_OPTR_LENGTH).get() , target->blockAddr() , NEXT_FREE_BLOCK_OPTR_LENGTH );
}

/* ---------------------------------------------------------------------------------- */





/* ---------------------------------------------------------------------------------- */
std::unique_ptr<optr> ControlBlock::mappingOptr()
{
	if( _blockOptr == nullptr )
	{
		return nullptr;
	}

	unsigned char* retAddr = new unsigned char[5];
	omemcpy( retAddr , (*_blockOptr + MAPPING_OPTR_OFFSET).get() , CONTROL_BLOCK_LENGTH );

	optr *retOptr = new optr( retAddr	); retOptr->cacheTable( _blockOptr->cacheTable() );

	return std::make_unique<optr>( *retOptr );
}


void ControlBlock::mappingOptr( optr* target )
{

	omemcpy( (*_blockOptr + MAPPING_OPTR_OFFSET).get() , target->addr() , MAPPING_OPTR_LENGTH ); // コントロールブロックの戦闘位置がPREV
}
/* ---------------------------------------------------------------------------------- */





/* ---------------------------------------------------------------------------------- */
std::unique_ptr<optr> ControlBlock::freeBlockEnd()
{
	if( _blockOptr == nullptr )
	{
		return nullptr;
	}

	unsigned char addrZero[5] = {0, 0, 0, 0, 0};
	if( ocmp( (*_blockOptr + (FREE_BLOCK_END_OPTR_OFFSET)).get() , addrZero , FREE_BLOCK_END_OPTR_LENGTH ) == 0 )
	{
		return nullptr;
	}

 
	unsigned char* retAddr = new unsigned char[5]; memset( retAddr , 0x00 , 5 );
	omemcpy( retAddr , (*_blockOptr + (FREE_BLOCK_END_OPTR_OFFSET)).get(), FREE_BLOCK_END_OPTR_LENGTH );
	optr *ret = new optr( retAddr ); ret->cacheTable( _blockOptr->cacheTable() );

	
	return std::make_unique<optr>( *ret );
}



void ControlBlock::freeBlockEnd( optr* target )
{
	if( target == nullptr )
	{
		unsigned char addrZero[5] = {0,0,0,0,0};
		omemcpy( (*_blockOptr + (FREE_BLOCK_END_OPTR_OFFSET)).get() , addrZero , FREE_BLOCK_END_OPTR_LENGTH );
	}
	else
	{
		//omemcpy( (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH)).get() , target , FREE_BLOCK_END_OPTR_LENGTH );
		omemcpy( (*_blockOptr + (FREE_BLOCK_END_OPTR_OFFSET)).get() , target->addr()  , FREE_BLOCK_END_OPTR_LENGTH );
	}
	return;
}
/* ---------------------------------------------------------------------------------- */








unsigned long ControlBlock::freeBlockSize()
{
	unsigned short exponentialList[5] = {64, 32, 16, 8, 0}; // 変換用の累乗リスト
	// 現在のポインタ位置とEndPtrの差分を計算する事で得る
	uint64_t ulControlBlockOptr = 0;  // 現在のFreeBlockControlBlockのポインタ
	uint64_t ulFreeBlockEndOptr = 0; // 終端のFreeBLockControlBlockのポインタ


	unsigned char mappingOptrAddr[5]; memset( mappingOptrAddr , 0x00 , 5 );
	memcpy( mappingOptrAddr, mappingOptr()->addr() , 5 );

	// フリー領域管理ブロックのポインタ
	ulControlBlockOptr += static_cast<uint64_t>( mappingOptrAddr[0] ) * pow(2, exponentialList[0]) ;
	ulControlBlockOptr += static_cast<uint64_t>( mappingOptrAddr[1] ) * pow(2, exponentialList[1]) ;
	ulControlBlockOptr += static_cast<uint64_t>( mappingOptrAddr[2] ) * pow(2, exponentialList[2]) ;
	ulControlBlockOptr += static_cast<uint64_t>( mappingOptrAddr[3] ) * pow(2, exponentialList[3]) ;
	ulControlBlockOptr += static_cast<uint64_t>( mappingOptrAddr[4] ) * pow(2, exponentialList[4]) ;


	if( freeBlockEnd() == nullptr  )
	{
		return 0;
	}

	unsigned char freeBlockEndOptrAddr[5]; memset( freeBlockEndOptrAddr , 0x0 , 5 );
	memcpy( freeBlockEndOptrAddr , freeBlockEnd()->addr() , 5 ); 

	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEndOptrAddr[0]) * pow(2, exponentialList[0]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEndOptrAddr[1]) * pow(2, exponentialList[1]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEndOptrAddr[2]) * pow(2, exponentialList[2]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEndOptrAddr[3]) * pow(2, exponentialList[3]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEndOptrAddr[4]) * pow(2, exponentialList[4]) ;

	return static_cast<unsigned long>(ulFreeBlockEndOptr - (ulControlBlockOptr)); // コントロールブロック分のサイズを加算する
}







}
