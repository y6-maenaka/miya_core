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


/*
 [ 注意 ] 
 freeBlockEndはフリーブロックの終端+1の位置を指す
*/



void MetaBlock::controlBlockHead( FreeBlockControlBlock *targetControlBlock )
{
	omemcpy( (*_blockOptr + CONTROL_BLOCK_HEAD_OFFSET).get() , targetControlBlock->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH );
}


std::unique_ptr<FreeBlockControlBlock> MetaBlock::controlBlockHead() // 要修正
{

	unsigned char* controlBlockHeadAddr = new unsigned char[5];
	omemcpy( controlBlockHeadAddr , (*_blockOptr + CONTROL_BLOCK_HEAD_OFFSET).get() , NEXT_FREE_BLOCK_OPTR_LENGTH );

	optr *controlBlockHeadOptr = new optr( controlBlockHeadAddr );
	controlBlockHeadOptr->cacheTable( _blockOptr->cacheTable() );

	FreeBlockControlBlock controlBlockHead( controlBlockHeadOptr );

	return std::make_unique<FreeBlockControlBlock>( controlBlockHead );
	//return controlBlockHead->nextControlBlock();
}




bool MetaBlock::isFileFormatted() // ファイル先頭のフォーマットIDが一致したら初期化されている
{
	if( ocmp( _blockOptr , (unsigned char *)(FORMAT_ID) , 15 ) == 0 )  {
		std::cout  << "[ ATTENTION ] is Formated" << "\n";
		return true;
	}

	std::cout  << "[ ATTENTION ] is Not Formated" << "\n";
	return false;
}







/* テストOK　動作確認済み */
FreeBlockControlBlock::FreeBlockControlBlock( optr *optr )
{
	_blockOptr = optr;
}


std::unique_ptr<optr> FreeBlockControlBlock::blockOptr()
{
	return std::make_unique<optr>( *_blockOptr );
}




std::unique_ptr<FreeBlockControlBlock> FreeBlockControlBlock::prevControlBlock()
{
	if( _blockOptr == nullptr )  return nullptr;

	// std::unique_ptr<optr> nextControlBlock( _optr );
	//return std::unique_ptr<FreeBlockControlBlock>( new FreeBlockControlBlock( _blockOptr) );

	//FreeBlockControlBlock ret( _blockOptr );
	return std::make_unique<FreeBlockControlBlock>( FreeBlockControlBlock( _blockOptr ) );
}


void FreeBlockControlBlock::prevControlBlock( FreeBlockControlBlock* target )
{
		omemcpy( _blockOptr , target->blockOptr().get()->addr() , PREV_FREE_BLOCK_OPTR_LENGTH );
}


void FreeBlockControlBlock::prevControlBlock( optr* target )
{
	omemcpy( _blockOptr, target->addr() , PREV_FREE_BLOCK_OPTR_LENGTH ); // コントロールブロックの戦闘位置がPREV
}









std::unique_ptr<FreeBlockControlBlock> FreeBlockControlBlock::nextControlBlock()
{
	if( _blockOptr == nullptr )  return nullptr;

	//optr retOptr( (*_blockOptr + PREV_FREE_BLOCK_OPTR_LENGTH).get() );
	optr *retOptr = new optr;
	retOptr->cacheTable( _blockOptr->cacheTable() ); retOptr->addr( (*_blockOptr + PREV_FREE_BLOCK_OPTR_LENGTH).get() );
	FreeBlockControlBlock retControlBlock( retOptr );

	return std::make_unique<FreeBlockControlBlock>( retControlBlock );
}


void FreeBlockControlBlock::nextControlBlock( FreeBlockControlBlock* target )
{
	omemcpy( (*_blockOptr + PREV_FREE_BLOCK_OPTR_LENGTH).get() , target->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH );
}

void FreeBlockControlBlock::nextControlBlock( optr* target )
{
	omemcpy( (*_blockOptr + PREV_FREE_BLOCK_OPTR_LENGTH).get() , target->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH ); // コントロールブロックの戦闘位置がPREV
}





std::unique_ptr<optr> FreeBlockControlBlock::freeBlockEnd()
{
	if( _blockOptr == nullptr )
	{
		return nullptr;
	}

	unsigned char addrZero[5] = {0, 0, 0, 0, 0};
	if( ocmp( (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH )).get() , addrZero , FREE_BLOCK_END_OPTR_LENGTH ) == 0 )
	{
		return nullptr;
	}


	unsigned char* retAddr = new unsigned char[5];
	omemcpy( retAddr , (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH )).get(), FREE_BLOCK_END_OPTR_LENGTH );
	optr *ret = new optr( retAddr );
	ret->cacheTable( _blockOptr->cacheTable() );

	return std::make_unique<optr>( *ret );
}



void FreeBlockControlBlock::freeBlockEnd( optr* target )
{
	if( target == nullptr )
	{
		unsigned char addrZero[5] = {0,0,0,0,0};
		omemcpy( (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH)).get() , addrZero , FREE_BLOCK_END_OPTR_LENGTH );
	}

	else{
		//omemcpy( (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH)).get() , target , FREE_BLOCK_END_OPTR_LENGTH );
		omemcpy( (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH)).get() , target->addr()  , FREE_BLOCK_END_OPTR_LENGTH );
	}
}




unsigned long FreeBlockControlBlock::freeBlockSize()
{
	if( _blockOptr == nullptr )  return 0;


	unsigned short exponentialList[6] = {64, 32, 16, 8, 0}; // 変換用の累乗リスト
	// 現在のポインタ位置とEndPtrの差分を計算する事で得る
	uint64_t ulControlBlockOptr = 0;  // 現在のFreeBlockControlBlockのポインタ
	uint64_t ulFreeBlockEndOptr = 0; // 終端のFreeBLockControlBlockのポインタ


	// フリー領域管理ブロックのポインタ
	ulControlBlockOptr += static_cast<uint64_t>(_blockOptr->addr()[0]) * pow(2, exponentialList[0]) ;
	ulControlBlockOptr += static_cast<uint64_t>(_blockOptr->addr()[1]) * pow(2, exponentialList[1]) ;
	ulControlBlockOptr += static_cast<uint64_t>(_blockOptr->addr()[2]) * pow(2, exponentialList[2]) ;
	ulControlBlockOptr += static_cast<uint64_t>(_blockOptr->addr()[3]) * pow(2, exponentialList[3]) ;
	ulControlBlockOptr += static_cast<uint64_t>(_blockOptr->addr()[4]) * pow(2, exponentialList[4]) ;



	unsigned char addrZero[5] = {0,0,0,0,0};
	if( freeBlockEnd() == nullptr  || ocmp( freeBlockEnd().get() , addrZero , 5 ) == 0  ){
		return 0;
	}


	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->addr()[0]) * pow(2, exponentialList[0]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->addr()[1]) * pow(2, exponentialList[1]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->addr()[2]) * pow(2, exponentialList[2]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->addr()[3]) * pow(2, exponentialList[3]) ;
	ulFreeBlockEndOptr += static_cast<uint64_t>( freeBlockEnd()->addr()[4]) * pow(2, exponentialList[4]) ;

	return static_cast<unsigned long>(ulFreeBlockEndOptr - (ulControlBlockOptr - FREE_BLOCK_CONTROL_BLOCK_LENGTH)); // コントロールブロック分のサイズを加算する
}




std::unique_ptr<FreeBlockControlBlock> OverlayMemoryAllocator::findFreeBlock( FreeBlockControlBlock *targetControlBlock, unsigned int allocateSize )
{
	std::cout << __PRETTY_FUNCTION__ << "\n";

	if( targetControlBlock->freeBlockSize() == 0 ) // フリーブロックが特に指定されていない場合は対象のコントロールブロックを返却する
		return std::make_unique<FreeBlockControlBlock>( *targetControlBlock );

	if( targetControlBlock->freeBlockSize() < allocateSize ) // フリーブロックのサイズが指定の割り当てサイズより小さい場合
		return findFreeBlock( (targetControlBlock->nextControlBlock()).get() , allocateSize );

	// このコントローブルロックを返却する
	return std::make_unique<FreeBlockControlBlock>( *targetControlBlock );
}



std::unique_ptr<FreeBlockControlBlock> OverlayMemoryAllocator::targetOptrPrevControlBlock( FreeBlockControlBlock *targetControlBlock , optr *targetOptr )
{
	/* パターン1 (Prevが存在しない : targetが先頭 ) */
	// 1. Head > target
	if( memcmp( targetOptr->addr(), controlBlockHead()->blockOptr()->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) < 0 )
	{
		std::cout << "パターン 1" << "\n";
		return nullptr;
	}

	/* パターン2 (Prevが存在する : targetが領域最後尾) */
	// 1. 再帰中に,nextControlBlockがHead
	if( memcmp( targetControlBlock->blockOptr()->addr() , targetOptr->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) < 0 && memcmp(  targetControlBlock->blockOptr()->addr() ,targetControlBlock->nextControlBlock()->blockOptr()->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0 )
	{
		std::cout << "パターン 2" << "\n";
		return std::unique_ptr<FreeBlockControlBlock>(targetControlBlock);
	}

	/* パターン3 (Prevが存在する : targetが領域中間) */
	// 1. 再帰中に,nextControlBlock > target
	// 2. 同　ControlBlock < target
	if( memcmp( targetControlBlock->blockOptr()->addr(), targetOptr->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) < 0 && memcmp( targetControlBlock->blockOptr()->addr(), targetOptr->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH) < 0 )
	{
		std::cout << "パターン 3" << "\n";
		return std::unique_ptr<FreeBlockControlBlock>(targetControlBlock);
	}


	return targetOptrPrevControlBlock( targetControlBlock->nextControlBlock().get() , targetOptr );
}




std::unique_ptr<FreeBlockControlBlock> OverlayMemoryAllocator::placeControlBlock( optr* targetOptr  /* 新たにフリーブロックを配置する対象のOptr */, FreeBlockControlBlock* prevControlBlock, FreeBlockControlBlock *nextControlBlock, optr* freeBlockEnd )
{
	FreeBlockControlBlock targetControlBlock( targetOptr ); // 対象のコントロールブロック

	nextControlBlock->prevControlBlock( &targetControlBlock );

	FreeBlockControlBlock* _prevControlBlock = (memcmp( prevControlBlock->blockOptr()->addr() , prevControlBlock->prevControlBlock()->blockOptr()->addr() , PREV_FREE_BLOCK_OPTR_LENGTH ) == 0 ) ? &targetControlBlock : prevControlBlock;
	FreeBlockControlBlock* _nextControlBlock = (memcmp( nextControlBlock->blockOptr()->addr() , nextControlBlock->nextControlBlock()->blockOptr()->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0 ) ? &targetControlBlock : nextControlBlock;

	targetControlBlock.prevControlBlock( prevControlBlock );
	targetControlBlock.nextControlBlock( nextControlBlock );




	if( freeBlockEnd == nullptr )
	{
		std::cout << "free block is null" << "\n";
		targetControlBlock.freeBlockEnd( nullptr );
	}
	else{
		std::cout << "free block is not null" << "\n";
		targetControlBlock.freeBlockEnd( freeBlockEnd );
		std::cout << "inserted free block below"  << "\n";
	}


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

	_metaBlock = new MetaBlock( const_cast<optr*>(_primaryOptr) );


	if( !(_metaBlock->isFileFormatted() )) // ファイルのフォーマットを行う
		init();

	std::cout << "\n\n\n\n\n";
}




std::unique_ptr<FreeBlockControlBlock> OverlayMemoryAllocator::controlBlockHead()
{
	return _metaBlock->controlBlockHead();
}





// 動作テストOK
void OverlayMemoryAllocator::init()
{
	//　ファイルの検証
	//　管理領域の配置
	//　初めのフリーブロック管理領域を配置

	// =========== 感領域を作成する前に,最初のコントロールブロックを作成する =====================
	FreeBlockControlBlock firstControlBlock(  (*_primaryOptr + META_BLOCK_SIZE).release() );

	firstControlBlock.prevControlBlock( &firstControlBlock );
	firstControlBlock.nextControlBlock( &firstControlBlock );
	firstControlBlock.freeBlockEnd( nullptr );

	//placeControlBlock( (*_primaryOptr + META_BLOCK_SIZE).get(), &firstControlBlock , &firstControlBlock , nullptr );

	_metaBlock->controlBlockHead( &firstControlBlock );

	omemcpy( const_cast<optr *>(_primaryOptr) , (unsigned char*)(FORMAT_ID) , 15 ); // フォーマットIDのセット

	// フォーマット完了
}




// どの仮想ファイルかを正確にする必要がある
std::unique_ptr<optr> OverlayMemoryAllocator::allocate( unsigned long allocateSize )
{
	std::cout << __PRETTY_FUNCTION__ << "\n";

	// 一旦allocateSizeをunsigned char型に変換する
	unsigned char ucAllocateSize[5];
	ucAllocateSize[0] = 0; // 要修正
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
	std::unique_ptr<FreeBlockControlBlock> placedNewControlBlock = placeControlBlock( (*(targetControlBlock->blockOptr()) + allocateSize).release() , targetControlBlock->prevControlBlock().get(), targetControlBlock->nextControlBlock().get(), targetControlBlock->freeBlockEnd().get() );

	if( memcmp( controlBlockHead()->blockOptr()->addr() , targetControlBlock->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0 )
	{
		_metaBlock->controlBlockHead( placedNewControlBlock.get() );
	}


	return targetControlBlock->blockOptr();
}










/*
 解放時の挙動
 1. 単純に解放した領域にControlBlockを配置する
 2. 前のフリーブロックのEndPtrをずらす
*/
void OverlayMemoryAllocator::deallocate( optr* target , unsigned long size  )
{
	/* 割り当て解除されるOptrの直前のコントロールブロックを得る */
	std::unique_ptr<FreeBlockControlBlock> __controlBlockHead = controlBlockHead();
	std::unique_ptr<FreeBlockControlBlock> insertTargetPrevControlBlock = targetOptrPrevControlBlock( __controlBlockHead.get() ,target );

	if( insertTargetPrevControlBlock == nullptr )
 	{
		if( size >= FREE_BLOCK_CONTROL_BLOCK_LENGTH ) // コントロールブロックを配置する
		{
			std::unique_ptr<FreeBlockControlBlock> ret = placeControlBlock(  target , controlBlockHead()->prevControlBlock().get() , controlBlockHead().get() , (*target + size).release() );
			_metaBlock->controlBlockHead( ret.get() );
			mergeControlBlock( ret.get() );
		}
		return;
	}

	// 直前のフリーブロックと兼用できるか検証する
	if( memcmp( target->addr(), insertTargetPrevControlBlock->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH )  ==  0 )
	{ // 併用可能 -> 直前のフリーブロックのfreeBlockEndを拡張する
		insertTargetPrevControlBlock->freeBlockEnd( (*(insertTargetPrevControlBlock->freeBlockEnd()) + size).get() ); // エラー発生する可能性あり
		mergeControlBlock( insertTargetPrevControlBlock.get() );
	}

	return;
}










void OverlayMemoryAllocator::mergeControlBlock( FreeBlockControlBlock *targetControlBlock )
{

	if( targetControlBlock->freeBlockEnd() == nullptr ) return;
	//FreeBlockControlBlock freeBlockEnd( targetControlBlock->freeBlockEnd().get() );

	//std::cout << " MergeControlBlock Called" << "\n";
	//targetControlBlock->freeBlockEnd()->printAddr(); std::cout << "\n";
	//std::cout << "------------" << "\n";
	//targetControlBlock->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
	//std::cout << memcmp( targetControlBlock->freeBlockEnd()->addr(), targetControlBlock->nextControlBlock()->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH )  << "\n";

	std::cout << "Passed Here" << "\n";
	// エラーが発生する可能性あり
	if( memcmp( targetControlBlock->freeBlockEnd()->addr(), targetControlBlock->nextControlBlock()->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0 )
	{
		std::cout << "マージ処理が行われます" << "\n";

		targetControlBlock->freeBlockEnd( targetControlBlock->nextControlBlock()->freeBlockEnd().get() );
		targetControlBlock->nextControlBlock()->nextControlBlock()->prevControlBlock( targetControlBlock );
		targetControlBlock->nextControlBlock( targetControlBlock->nextControlBlock()->nextControlBlock().get() );

		targetControlBlock->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		targetControlBlock->prevControlBlock()->blockOptr()->printAddr(); std::cout << "\n";

		return mergeControlBlock( targetControlBlock->nextControlBlock().get() );
	}

	std::cout << "コントロールブロックマージは行われませんでした もしくは 完了しました" << "\n";
	return;
}




void OverlayMemoryAllocator::printControlChain( FreeBlockControlBlock* targetControlBlock )
{
	std::cout << __PRETTY_FUNCTION__ << "\n";
	int i=0;

	do
	{
		std::cout << "[ " << i << " ]" << "\n";
		i++;

		targetControlBlock = targetControlBlock->nextControlBlock().release();

	}while( ocmp( controlBlockHead()->blockOptr().get(), targetControlBlock->blockOptr().get() , NEXT_FREE_BLOCK_OPTR_LENGTH ) != 0 );


}



}; // close miya_db namespace
