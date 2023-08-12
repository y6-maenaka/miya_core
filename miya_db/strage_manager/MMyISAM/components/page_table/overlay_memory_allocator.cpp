#include "overlay_memory_allocator.h"
#include "./optr_utils.h" 
//#include "./overlay_ptr.h"
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








std::unique_ptr<FreeBlockControlBlock> MetaBlock::controlBlockHead()
{
	FreeBlockControlBlock *controlBlockHead = new FreeBlockControlBlock( (*_blockOptr + CONTROL_BLOCK_HEAD_OFFSET).get() );

	return controlBlockHead->nextControlBlock();
}















FreeBlockControlBlock::FreeBlockControlBlock( optr *optr ) // コンストラクタ
{
	_blockOptr = optr;
}





std::unique_ptr<optr> FreeBlockControlBlock::blockOptr()
{
	return std::unique_ptr<optr>(_blockOptr);
}




std::unique_ptr<FreeBlockControlBlock> FreeBlockControlBlock::prevControlBlock()
{
	if( _blockOptr == nullptr )  return nullptr;

	// std::unique_ptr<optr> nextControlBlock( _optr );
	return std::unique_ptr<FreeBlockControlBlock>( new FreeBlockControlBlock( _blockOptr) );
}


void FreeBlockControlBlock::prevControlBlock( FreeBlockControlBlock* target )
{
	omemcpy( _blockOptr , target->blockOptr().get() ,PREV_FREE_BLOCK_OPTR_LENGTH );
}


void FreeBlockControlBlock::prevControlBlock( optr* target )
{
	omemcpy( _blockOptr, target , PREV_FREE_BLOCK_OPTR_LENGTH ); // コントロールブロックの戦闘位置がPREV
} 









std::unique_ptr<FreeBlockControlBlock> FreeBlockControlBlock::nextControlBlock()
{
	if( _blockOptr == nullptr )  return nullptr;

	return std::unique_ptr<FreeBlockControlBlock>( new FreeBlockControlBlock( (*_blockOptr + PREV_FREE_BLOCK_OPTR_LENGTH).get() ) );
}


void FreeBlockControlBlock::nextControlBlock( FreeBlockControlBlock* target )
{
	omemcpy( (*_blockOptr + PREV_FREE_BLOCK_OPTR_LENGTH).get() , target->blockOptr().get() , NEXT_FREE_BLOCK_OPTR_LENGTH );
}

void FreeBlockControlBlock::nextControlBlock( optr* target )
{
	omemcpy( (*_blockOptr + PREV_FREE_BLOCK_OPTR_LENGTH).get() , target , NEXT_FREE_BLOCK_OPTR_LENGTH ); // コントロールブロックの戦闘位置がPREV
}





std::unique_ptr<optr> FreeBlockControlBlock::freeBlockEnd()
{	
	if( _blockOptr == nullptr )  return nullptr;

	std::unique_ptr<optr> freeBlockEnd( (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH)).get() );

	return freeBlockEnd;
}

void FreeBlockControlBlock::freeBlockEnd( optr* target )
{
	omemcpy( (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH)).get() , target , FREE_BLOCK_END_OPTR_LENGTH ); // コントロールブロックの戦闘位置がPREV
}




unsigned long FreeBlockControlBlock::freeBlockSize()
{
	if( _blockOptr == nullptr )  return 0;
	
	unsigned short exponentialList[6] = {64, 32, 16, 8, 0}; // 変換用の累乗リスト
	// 現在のポインタ位置とEndPtrの差分を計算する事で得る	
	uint64_t ulControlBlockOptr = 0;  // 現在のFreeBlockControlBlockのポインタ
	uint64_t ulFreeBlockEndOptr = 0; // 終端のFreeBLockControlBlockのポインタ

	unsigned char *currentOAddr = new unsigned char[5];
	currentOAddr = _blockOptr->addr();

	// フリー領域管理ブロックのポインタ
	ulControlBlockOptr += static_cast<uint64_t>(currentOAddr[0]) * pow(2, exponentialList[0]) ;
	ulControlBlockOptr += static_cast<uint64_t>(currentOAddr[1]) * pow(2, exponentialList[1]) ;
	ulControlBlockOptr += static_cast<uint64_t>(currentOAddr[2]) * pow(2, exponentialList[2]) ;
	ulControlBlockOptr += static_cast<uint64_t>(currentOAddr[3]) * pow(2, exponentialList[3]) ;
	ulControlBlockOptr += static_cast<uint64_t>(currentOAddr[4]) * pow(2, exponentialList[4]) ;
	if( ulControlBlockOptr == 0 ) return std::numeric_limits<unsigned long>::max();

	unsigned char *freeBlockEndOAddr = new unsigned char[5];
	freeBlockEndOAddr = freeBlockEnd()->addr();

	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEndOAddr[0]) * pow(2, exponentialList[0]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEndOAddr[1]) * pow(2, exponentialList[1]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEndOAddr[2]) * pow(2, exponentialList[2]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEndOAddr[3]) * pow(2, exponentialList[3]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEndOAddr[4]) * pow(2, exponentialList[4]) ;

	delete[] currentOAddr; delete[] freeBlockEndOAddr;
	return static_cast<unsigned long>(ulFreeBlockEndOptr - (ulControlBlockOptr - FREE_BLOCK_CONTROL_BLOCK_LENGTH)); // コントロールブロック分のサイズを加算する
}




std::unique_ptr<FreeBlockControlBlock> OverlayMemoryAllocator::findFreeBlock( FreeBlockControlBlock *targetControlBlock, unsigned int allocateSize )
{
	if( targetControlBlock->freeBlockSize() < allocateSize )
		return findFreeBlock( (targetControlBlock->nextControlBlock()).get() , allocateSize );

	//return std::make_unique<FreeBlockControlBlock>(*targetControlBlock);
	return std::unique_ptr<FreeBlockControlBlock>( targetControlBlock );
}



void OverlayMemoryAllocator::placeControlBlock( optr* targetOptr , FreeBlockControlBlock* prevControlBlock, FreeBlockControlBlock *nextControlBlock, optr* freeBlockEnd )
{
	// 一旦optrの位置にControlBlockを配置する
	FreeBlockControlBlock targetControlBlock( targetOptr );
	targetControlBlock.prevControlBlock( prevControlBlock );
	targetControlBlock.nextControlBlock( nextControlBlock );
	targetControlBlock.freeBlockEnd( freeBlockEnd );

	prevControlBlock->nextControlBlock( &targetControlBlock );
	nextControlBlock->prevControlBlock( &targetControlBlock );
	return;
}



std::unique_ptr<FreeBlockControlBlock> getPrevControlBlock( FreeBlockControlBlock* targetControlBlock ,optr* target )
{
	if( ocmp( targetControlBlock->nextControlBlock()->blockOptr().get(), target , 5  ) < 0 && ocmp( targetControlBlock->prevControlBlock()->blockOptr().get(), target , 5 ) < 0 ) return std::unique_ptr<FreeBlockControlBlock>(targetControlBlock);

	if( ocmp( targetControlBlock->nextControlBlock()->blockOptr().get() ,target , 5  ) > 0 && ocmp( targetControlBlock->blockOptr().get() , target , 5 ) < 0 ) return std::unique_ptr<FreeBlockControlBlock>(targetControlBlock);

	return getPrevControlBlock( targetControlBlock->nextControlBlock().get() , target );
}










OverlayMemoryAllocator::OverlayMemoryAllocator( optr *primaryOptr ) : _primaryOptr(primaryOptr)
{	
	_metaBlock = new MetaBlock( const_cast<optr*>(_primaryOptr) );
}




std::unique_ptr<FreeBlockControlBlock> OverlayMemoryAllocator::controlBlockHead()
{
	return _metaBlock->controlBlockHead();
}




void OverlayMemoryAllocator::init()
{
	//　ファイルの検証
	//　管理領域の配置
	//　初めのフリーブロック管理領域を配置
}




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


	std::unique_ptr<FreeBlockControlBlock> __controlBlockHead  = controlBlockHead(); // 先頭のコントロールブロック管理ブロックを取得する

	// 割り当てサイズ以上のコントロールブロックを特定する
	std::unique_ptr<FreeBlockControlBlock> targetControlBlock = findFreeBlock( __controlBlockHead.get() , allocateSize );
	// 新しいフリーメモリ管理ブロックの作成
	unsigned int targetFreeBlockSize = targetControlBlock->freeBlockSize();

	// これから割り当てるフリーブロックにフリーブロック管理ブロックを配置するスペースがない場合は設置しない
	if( FREE_BLOCK_CONTROL_BLOCK_LENGTH + allocateSize <= targetFreeBlockSize )
	{
		targetControlBlock->prevControlBlock()->nextControlBlock( targetControlBlock->nextControlBlock().get() );
		targetControlBlock->nextControlBlock()->prevControlBlock( targetControlBlock->prevControlBlock().get() );
		return targetControlBlock->blockOptr();
	}	

	// 新規にフリーブロックコントロールブロックを作成し配置する
	placeControlBlock( (*(targetControlBlock->blockOptr()) + allocateSize).get() , targetControlBlock->prevControlBlock().get(), targetControlBlock->nextControlBlock().get(), targetControlBlock->freeBlockEnd().get() );

	return targetControlBlock->blockOptr();
}









void OverlayMemoryAllocator::unallocate( optr* target , unsigned int size  )
{
	// フリーメモリ管理ブロックの追加
	FreeBlockControlBlock targetControlBlock( target ); // 解放する領域の初めにフリーブロック管理ブロックを配置する
	std::unique_ptr<FreeBlockControlBlock> insertTargetControlBlock; //= getInsertPrevControlBlock( getHeadControlBlock().get() , target );

	
	auto placeControlBlock = [&]() // 解放した領域に新たなコントロールブロックを配置する
	{
		targetControlBlock.prevControlBlock( insertTargetControlBlock->blockOptr().get() );
		targetControlBlock.nextControlBlock( insertTargetControlBlock->nextControlBlock()->blockOptr().get() );
		targetControlBlock.freeBlockEnd( (*target + size).get() );

		insertTargetControlBlock->nextControlBlock( targetControlBlock.blockOptr().get() );
		insertTargetControlBlock->nextControlBlock()->prevControlBlock( targetControlBlock.blockOptr().get() );

	};

	// 解放した領域にフリーブロックを配置する領域が存在すれば
	if( size > PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH + FREE_BLOCK_CONTROL_BLOCK_LENGTH ) 
		placeControlBlock();

	return;
}




/*
std::unique_ptr<FreeBlockControlBlock> OverlayMemoryAllocator::controlBlockHead()
{
	if( _primaryOptr == nullptr ) return nullptr;

	// ポインタ一が0のoptrを作成する
	//optr optrZero;
	//optrZero.cacheTable( _cacheTable );

	//FreeBlockControlBlock controlBlockZero( &optrZero );
	FreeBlockControlBlock primaryControlBlock( const_cast<optr*>(_primaryOptr) ); // あまりよくない気がする
	
	return primaryControlBlock.nextControlBlock();
}
*/






}; // close miya_db namespace
