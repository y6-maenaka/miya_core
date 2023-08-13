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





/*
	 return std::uniqeu_ptr<optr> //
	 return std::make_unique<optr> // 新たなunique_ptrで割り当てる
*/



void MetaBlock::controlBlockHead( FreeBlockControlBlock *targetControlBlock )
{
	omemcpy( (*_blockOptr + CONTROL_BLOCK_HEAD_OFFSET).get() , targetControlBlock->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH );
}


std::unique_ptr<FreeBlockControlBlock> MetaBlock::controlBlockHead()
{
	FreeBlockControlBlock *controlBlockHead = new FreeBlockControlBlock( (*_blockOptr + CONTROL_BLOCK_HEAD_OFFSET).get() );

	return controlBlockHead->nextControlBlock();
}




bool MetaBlock::isFileFormatted() // ファイル先頭のフォーマットIDが一致したら初期化されている
{


	for( int i=0; i<15; i++ )
	{
		std::cout << (*_blockOptr + i)->offset() << " : " << (*_blockOptr + i )->frame() << "   ";
		printf(" -> %c\n", (*_blockOptr + i)->value() );
	} std::cout << "\n";

	if( ocmp( _blockOptr , (unsigned char *)(FORMAT_ID) , 15 ) == 0 )  {
		puts("ATTENTION!");
		std::cout << "このファイルはフォーマットされて[ います ]" << "\n";
		return true;
	}

	puts("ATTENTION!");
	std::cout << "このファイルはフォーマットされて[ いません ]" << "\n";
	return false;
}







/* テストOK　動作確認済み */
FreeBlockControlBlock::FreeBlockControlBlock( optr *optr ) 
{
	_blockOptr = optr;
}




std::unique_ptr<optr> FreeBlockControlBlock::blockOptr()
{
	//return std::unique_ptr<optr>(_blockOptr);
	return std::make_unique<optr>( *_blockOptr );
}




std::unique_ptr<FreeBlockControlBlock> FreeBlockControlBlock::prevControlBlock()
{
	if( _blockOptr == nullptr )  return nullptr;

	// std::unique_ptr<optr> nextControlBlock( _optr );
	return std::unique_ptr<FreeBlockControlBlock>( new FreeBlockControlBlock( _blockOptr) );
}


void FreeBlockControlBlock::prevControlBlock( FreeBlockControlBlock* target )
{
		omemcpy( _blockOptr , target->blockOptr().get()->addr() , PREV_FREE_BLOCK_OPTR_LENGTH );
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
	if( target == nullptr )
		omemcpy( (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH)).get() , (unsigned char*)"000000" , FREE_BLOCK_END_OPTR_LENGTH ); 
																																																																				
	else
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

	if( ulFreeBlockEndOptr == 0 ) return 0;

	delete[] currentOAddr; delete[] freeBlockEndOAddr;
	return static_cast<unsigned long>(ulFreeBlockEndOptr - (ulControlBlockOptr - FREE_BLOCK_CONTROL_BLOCK_LENGTH)); // コントロールブロック分のサイズを加算する
}




std::unique_ptr<FreeBlockControlBlock> OverlayMemoryAllocator::findFreeBlock( FreeBlockControlBlock *targetControlBlock, unsigned int allocateSize )
{
	if( targetControlBlock->freeBlockSize() == 0 ) 
		return std::unique_ptr<FreeBlockControlBlock>( targetControlBlock );

	if( targetControlBlock->freeBlockSize() < allocateSize )
		return findFreeBlock( (targetControlBlock->nextControlBlock()).get() , allocateSize );

	//return std::make_unique<FreeBlockControlBlock>(*targetControlBlock);
	return std::unique_ptr<FreeBlockControlBlock>( targetControlBlock );
}



std::unique_ptr<FreeBlockControlBlock> OverlayMemoryAllocator::targetOptrPrevControlBlock( FreeBlockControlBlock *targetControlBlock , optr *targetOptr )
{

	/* パターン1 (Prevが存在しない : targetが先頭 ) */
	// 1. Head > target
	if( memcmp( targetOptr->addr(), controlBlockHead()->blockOptr()->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) <= 0 )
		return nullptr;

	/* パターン2 (Prevが存在する : targetが領域最後尾) */
	// 1. 再帰中に,nextControlBlockがHead
	if( memcmp( controlBlockHead()->blockOptr()->addr(), targetControlBlock->nextControlBlock()->blockOptr()->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0 )
		return std::unique_ptr<FreeBlockControlBlock>(targetControlBlock);

	/* パターン3 (Prevが存在する : targetが領域中間) */
	// 1. 再帰中に,nextControlBlock > target
	// 2. 同　ControlBlock < target
	if( memcmp( targetOptr->addr(), targetControlBlock->nextControlBlock()->blockOptr()->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) <= 0 && memcmp( targetOptr->addr(), targetControlBlock->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) >= 0 )
		return std::unique_ptr<FreeBlockControlBlock>(targetControlBlock);


	return targetOptrPrevControlBlock( targetControlBlock->nextControlBlock().get() , targetOptr );
}


std::unique_ptr<FreeBlockControlBlock> OverlayMemoryAllocator::placeControlBlock( optr* targetOptr , FreeBlockControlBlock* prevControlBlock, FreeBlockControlBlock *nextControlBlock, optr* freeBlockEnd )
{

	FreeBlockControlBlock targetControlBlock( targetOptr ); // 対象のコントロールブロック

	targetControlBlock.prevControlBlock( prevControlBlock );
	targetControlBlock.nextControlBlock( nextControlBlock );

	if( freeBlockEnd == nullptr )
		targetControlBlock.freeBlockEnd( nullptr );
	else
		targetControlBlock.freeBlockEnd( freeBlockEnd );


	prevControlBlock->nextControlBlock( &targetControlBlock );
	nextControlBlock->prevControlBlock( &targetControlBlock );

	return std::make_unique<FreeBlockControlBlock>( targetControlBlock );
}



std::unique_ptr<FreeBlockControlBlock> getPrevControlBlock( FreeBlockControlBlock* targetControlBlock ,optr* target )
{
	if( ocmp( targetControlBlock->nextControlBlock()->blockOptr().get(), target , 5  ) < 0 && ocmp( targetControlBlock->prevControlBlock()->blockOptr().get(), target , 5 ) < 0 ) return std::unique_ptr<FreeBlockControlBlock>(targetControlBlock);

	if( ocmp( targetControlBlock->nextControlBlock()->blockOptr().get() ,target , 5  ) > 0 && ocmp( targetControlBlock->blockOptr().get() , target , 5 ) < 0 ) return std::unique_ptr<FreeBlockControlBlock>(targetControlBlock);

	return getPrevControlBlock( targetControlBlock->nextControlBlock().get() , target );
}










OverlayMemoryAllocator::OverlayMemoryAllocator( optr *primaryOptr ) : _primaryOptr(primaryOptr)
{	
	std::cout << __PRETTY_FUNCTION__ << "\n";

	//std::cout << primaryOptr->offset()	 << "\n";

	//printf("%c\n", (*primaryOptr + 1)->value() );


	_metaBlock = new MetaBlock( const_cast<optr*>(_primaryOptr) );

	std::cout << _metaBlock->blockOptr()->value() << "\n";


	if( !(_metaBlock->isFileFormatted() )) // ファイルのフォーマットを行う
		init();

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
	std::cout << __PRETTY_FUNCTION__  << " STARTED!" << "\n";

	// =========== 感領域を作成する前に,最初のコントロールブロックを作成する =====================
	FreeBlockControlBlock firstControlBlock(  ( *_primaryOptr + META_BLOCK_SIZE  ).get() );

	placeControlBlock( (*_primaryOptr + META_BLOCK_SIZE).get(), &firstControlBlock , &firstControlBlock , nullptr );


	_metaBlock->controlBlockHead( &firstControlBlock );


	//std::cout << _metaBlock->isFileFormatted() << "\n";
	
	omemcpy( const_cast<optr *>(_primaryOptr) , (unsigned char*)(FORMAT_ID) , 15 ); // フォーマットIDのセット
	
	std::cout << __PRETTY_FUNCTION__ << " DONE!" << "\n";

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










/*
 解放時の挙動
 1. 単純に解放した領域にControlBlockを配置する
 2. 前のフリーブロックのEndPtrをずらす
*/
void OverlayMemoryAllocator::unallocate( optr* target , unsigned int size  )
{

	/* 割り当て解除されるOptrの直前のコントロールブロックを得る */
	std::unique_ptr<FreeBlockControlBlock> __controlBlockHead = controlBlockHead();
	std::unique_ptr<FreeBlockControlBlock> insertTargetPrevControlBlock = targetOptrPrevControlBlock( __controlBlockHead.get() ,target );

	if( insertTargetPrevControlBlock == nullptr )
 	{
		if( size >= FREE_BLOCK_CONTROL_BLOCK_LENGTH ) // コントロールブロックを配置する
		{
			std::unique_ptr<FreeBlockControlBlock> ret = placeControlBlock(  target , controlBlockHead()->prevControlBlock().get() , controlBlockHead().get() , (*target + size).get() );
			mergeControlBlock( ret.get() );
		}
		return;

	}

	// 直前のフリーブロックと兼用できるか検証する
	if( memcmp( target->addr(), insertTargetPrevControlBlock->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH )  ==  0 )
	{ // 併用可能 -> 直前のフリーブロックのfreeBlockEndを拡張する
		insertTargetPrevControlBlock->freeBlockEnd( (*(insertTargetPrevControlBlock->freeBlockEnd()) + size).get() );
		mergeControlBlock( insertTargetPrevControlBlock.get() );
	}

	return;
}




void OverlayMemoryAllocator::mergeControlBlock( FreeBlockControlBlock *targetControlBlock )
{
	if( memcmp( targetControlBlock->freeBlockEnd()->addr() , targetControlBlock->nextControlBlock()->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0 )
	{
		// targetの次のコントロールブロックが削除される
		if( memcmp( targetControlBlock->blockOptr()->addr() , controlBlockHead()->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) != 0 )
		{
			targetControlBlock->freeBlockEnd( targetControlBlock->nextControlBlock()->freeBlockEnd().get() );
			targetControlBlock->nextControlBlock()->nextControlBlock()->prevControlBlock( targetControlBlock );
			targetControlBlock->nextControlBlock( targetControlBlock->nextControlBlock()->nextControlBlock().get() );

			return mergeControlBlock( targetControlBlock->nextControlBlock().get() );
		}
	}
	return;
}






}; // close miya_db namespace
