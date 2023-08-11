#include "overlay_memory_allocator.h"
#include "./optr_utils.h" 
#include "./overlay_ptr.h"
#include "./cache_manager/cache_table.h"



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

/*
-------------------------------------------------------------------------------------------
先頭のフリーブロックへのポインタ(controlBlockZero) | 先頭フリーブロック(controlBlockHead) | 
-------------------------------------------------------------------------------------------
*/

/*
	先頭フリーブロックのネクストとプレブが自身を指すことがある
*/





FreeBlockControlBlock::FreeBlockControlBlock( optr *optr )
{
	_optr = optr;
}





std::unique_ptr<optr> FreeBlockControlBlock::Optr()
{
	return std::unique_ptr<optr>(_optr);
}




std::unique_ptr<FreeBlockControlBlock> FreeBlockControlBlock::prevControlBlock()
{
	if( _optr == nullptr )  return nullptr;

	// std::unique_ptr<optr> nextControlBlock( _optr );
	return std::unique_ptr<FreeBlockControlBlock>( new FreeBlockControlBlock(_optr) );
}

void FreeBlockControlBlock::prevControlBlock( optr* target )
{
	omemcpy( _optr, target , PREV_FREE_BLOCK_OPTR_LENGTH ); // コントロールブロックの戦闘位置がPREV
} 









std::unique_ptr<FreeBlockControlBlock> FreeBlockControlBlock::nextControlBlock()
{
	if( _optr == nullptr )  return nullptr;

	//return std::make_unique<FreeBlockControlBlock>(FreeBlockControlBlock( _optr + 5 ));
	return std::unique_ptr<FreeBlockControlBlock>( new FreeBlockControlBlock(_optr + 5 ) );
}

void FreeBlockControlBlock::nextControlBlock( optr* target )
{
	omemcpy( _optr + PREV_FREE_BLOCK_OPTR_LENGTH , target , NEXT_FREE_BLOCK_OPTR_LENGTH ); // コントロールブロックの戦闘位置がPREV
}





std::unique_ptr<optr> FreeBlockControlBlock::freeBlockEnd()
{	
	if( _optr == nullptr )  return nullptr;

	std::unique_ptr<optr> freeBlockEnd( _optr+10 );
	return freeBlockEnd;
}

void FreeBlockControlBlock::freeBlockEnd( optr* target )
{
	omemcpy( _optr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH) , target , FREE_BLOCK_END_OPTR_LENGTH ); // コントロールブロックの戦闘位置がPREV
}




unsigned long FreeBlockControlBlock::freeBlockSize()
{
	if( _optr == nullptr )  return 0;
	
	unsigned short exponentialList[6] = {64, 32, 16, 8, 0};
	// 現在のポインタ位置とEndPtrの差分を計算する事で得る	
	uint64_t ulControlBlockOPtr = 0; 
	uint64_t ulFreeBlockEndOptr = 0;

	// フリー領域管理ブロックのポインタ
	ulControlBlockOPtr += static_cast<uint64_t>(_optr->addr()[0]) * pow(2, exponentialList[0]) ;
	ulControlBlockOPtr += static_cast<uint64_t>(_optr->addr()[1]) * pow(2, exponentialList[1]) ;
	ulControlBlockOPtr += static_cast<uint64_t>(_optr->addr()[2]) * pow(2, exponentialList[2]) ;
	ulControlBlockOPtr += static_cast<uint64_t>(_optr->addr()[3]) * pow(2, exponentialList[3]) ;
	ulControlBlockOPtr += static_cast<uint64_t>(_optr->addr()[4]) * pow(2, exponentialList[4]) ;
	if( ulControlBlockOPtr == 0 ) return std::numeric_limits<unsigned long>::max();


	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->addr()[0]) * pow(2, exponentialList[0]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->addr()[1]) * pow(2, exponentialList[1]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->addr()[2]) * pow(2, exponentialList[2]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->addr()[3]) * pow(2, exponentialList[3]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->addr()[4]) * pow(2, exponentialList[4]) ;

	return static_cast<unsigned long>(ulFreeBlockEndOptr - (ulControlBlockOPtr+15)); // コントロールブロック分のサイズを加算する
}




std::unique_ptr<FreeBlockControlBlock> findFreeBlock( FreeBlockControlBlock *targetControlBlock, unsigned int allocateSize )
{
	if( targetControlBlock->freeBlockSize() < allocateSize )
		return findFreeBlock( (targetControlBlock->nextControlBlock()).get() , allocateSize );

	return std::make_unique<FreeBlockControlBlock>(*targetControlBlock);
}


std::unique_ptr<FreeBlockControlBlock> getPrevControlBlock( FreeBlockControlBlock* targetControlBlock ,optr* target )
{
	if( ocmp( targetControlBlock->nextControlBlock()->Optr().get(), target , 5  ) < 0 && ocmp( targetControlBlock->prevControlBlock()->Optr().get(), target , 5 ) < 0 ) return std::unique_ptr<FreeBlockControlBlock>(targetControlBlock);

	if( ocmp( targetControlBlock->nextControlBlock()->Optr().get() ,target , 5  ) > 0 && ocmp( targetControlBlock->Optr().get() , target , 5 ) < 0 ) return std::unique_ptr<FreeBlockControlBlock>(targetControlBlock);

	return getPrevControlBlock( targetControlBlock->nextControlBlock().get() , target );
}







/*
void FreeBlockControlBlock::placeControlBlock( optr* targetPlaceOptr , optr* prevControlBlockOptr, optr* nextControlBlockOptr, optr* endFreeBlockOptr )
{
	omemcpy( *targetPlaceOptr , *prevControlBlockOptr , PREV_FREE_BLOCK_OPTR_LENGTH );
	//omemcpy( *(*targetPlaceOptr + PREV_FREE_BLOCK_OPTR_LENGTH) , *nextControlBlockOptr , NEXT_FREE_BLOCK_OPTR_LENGTH );
	//omemcpy( *(*targetPlaceOptr + PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH) , *endFreeBlockOptr , END_FREE_BLOCK_OPTR_LENGTH );

}
void FreeBlockControlBlock::placeControlBlock( std::unique_ptr<optr> targetPlaceOptr , std::unique_ptr<optr> prevControlBlockOptr, std::unique_ptr<optr> nextControlBlockOptr, std::unique_ptr<optr> endFreeBlockOptr )
{
	//omemcpy( *targetPlaceOptr , prevControlBlockOptr , PREV_FREE_BLOCK_OPTR_LENGTH );
	//omemcpy( *(*targetPlaceOptr + PREV_FREE_BLOCK_OPTR_LENGTH) , nextControlBlockOptr , NEXT_FREE_BLOCK_OPTR_LENGTH );
	//omemcpy( *(*targetPlaceOptr + PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH) , endFreeBlockOptr , END_FREE_BLOCK_OPTR_LENGTH );
}
*/








// どの仮想ファイルかを正確にする必要がある
std::unique_ptr<optr> OverlayMemoryAllocator::allocate( unsigned int allocateSize )
{
	// 一旦allocateSizeをunsigned char型に変換する
	unsigned char ucAllocateSize[5];
	ucAllocateSize[0] = 0;
	ucAllocateSize[1] = (allocateSize >> 24 ) & 0xFF;
	ucAllocateSize[2] = (allocateSize >> 16 ) & 0xFF;
	ucAllocateSize[3]	= (allocateSize >> 8 ) & 0xFF;
	ucAllocateSize[4] = (allocateSize) & 0xFF;


	std::unique_ptr<FreeBlockControlBlock> controlBlockHead = getHeadControlBlock();

	std::unique_ptr<FreeBlockControlBlock> ret = findFreeBlock( controlBlockHead.get() , allocateSize );
	// 新しいフリーメモリ管理ブロックの作成

	unsigned int targetFreeBlockSize = ret->freeBlockSize();

	// これから割り当てるフリーブロックにフリーブロック管理ブロックを配置するスペースがない場合は設置しない
	if( allocateSize <  targetFreeBlockSize +  FREE_BLOCK_CONTROL_BLOCK_LENGTH )  
		return ret->Optr();

	// 新規にフリーブロックコントロールブロックを作成し配置する
	auto reLinkControlBlock = [&]()
	{
		omemcpy( (*ret->Optr() + allocateSize).get() , ret->prevControlBlock().get() , PREV_FREE_BLOCK_OPTR_LENGTH );
		omemcpy( (*ret->Optr() + (allocateSize + PREV_FREE_BLOCK_OPTR_LENGTH) ).get() , ret->nextControlBlock().get() , NEXT_FREE_BLOCK_OPTR_LENGTH );
		omemcpy( (*ret->Optr() + (allocateSize + PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH) ).get() , ret->freeBlockEnd().get() , FREE_BLOCK_END_OPTR_LENGTH );

		// 前のコントロールブロックのネクストポインタを新規のコントロールブロックに
		(ret->prevControlBlock())->nextControlBlock( ret->Optr().get() );

		// 次のコントロールブロックのプレブポインタを新規のコントロールブロックに
		(ret->nextControlBlock())->prevControlBlock( ret->Optr().get() );
	};
	reLinkControlBlock();
	
	return nullptr;
}









void OverlayMemoryAllocator::unallocate( optr* target , unsigned int size  )
{
	// フリーメモリ管理ブロックの追加
	FreeBlockControlBlock targetControlBlock( target ); // 解放する領域の初めにフリーブロック管理ブロックを配置する
	std::unique_ptr<FreeBlockControlBlock> insertTargetControlBlock = getInsertPrevCOntrolBlock( getHeadControlBlock().get() , target );

	
	auto placeControlBlock = [&]() // 解放した領域に新たなコントロールブロックを配置する
	{
		targetControlBlock.prevControlBlock( insertTargetControlBlock->Optr().get() );
		targetControlBlock.nextControlBlock( insertTargetControlBlock->nextControlBlock()->Optr().get() );
		targetControlBlock.freeBlockEnd( (*target + size).get() );

		insertTargetControlBlock->nextControlBlock( targetControlBlock.Optr().get() );
		insertTargetControlBlock->nextControlBlock()->prevControlBlock( targetControlBlock.Optr().get() );

	};

	// 解放した領域にフリーブロックを配置する領域が存在すれば
	if( size > PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH + FREE_BLOCK_CONTROL_BLOCK_LENGTH ) 
		placeControlBlock();

	return;
}





std::unique_ptr<FreeBlockControlBlock> OverlayMemoryAllocator::getHeadControlBlock()
{
	if( _primaryOptr == nullptr ) return nullptr;

	// ポインタ一が0のoptrを作成する
	//optr optrZero;
	//optrZero.cacheTable( _cacheTable );

	//FreeBlockControlBlock controlBlockZero( &optrZero );
	FreeBlockControlBlock primaryControlBlock( const_cast<optr*>(_primaryOptr) ); // あまりよくない気がする
	
	return primaryControlBlock.nextControlBlock();
}






}; // close miya_db namespace
