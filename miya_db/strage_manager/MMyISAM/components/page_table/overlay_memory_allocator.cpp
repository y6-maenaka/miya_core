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
	omemcpy( _blockOptr, target , PREV_FREE_BLOCK_OPTR_LENGTH ); // コントロールブロックの戦闘位置がPREV
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
	omemcpy( (*_blockOptr + PREV_FREE_BLOCK_OPTR_LENGTH).get() , target->blockOptr().get() , NEXT_FREE_BLOCK_OPTR_LENGTH );
}

void FreeBlockControlBlock::nextControlBlock( optr* target )
{
	omemcpy( (*_blockOptr + PREV_FREE_BLOCK_OPTR_LENGTH).get() , target , NEXT_FREE_BLOCK_OPTR_LENGTH ); // コントロールブロックの戦闘位置がPREV
}





std::unique_ptr<optr> FreeBlockControlBlock::freeBlockEnd()
{
	if( _blockOptr == nullptr )
	{
		std::cout << "freeBlockEnd retuend nullptr" << "\n";
		return nullptr;
	}

	unsigned char addrZero[5] = {0, 0, 0, 0, 0};
	if( ocmp( (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH )).get() , addrZero , FREE_BLOCK_END_OPTR_LENGTH ) == 0 )
	{
		std::cout << "retuend nullptr" << "\n";
		return nullptr;
	}


	std::unique_ptr<optr> ret = std::make_unique<optr>( *((*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH)).get()) );
	
	return ret;
}



void FreeBlockControlBlock::freeBlockEnd( optr* target )
{
	if( target == nullptr )
	{
		std::cout << "--------" << "\n";
		std::cout << "CALLED" << "\n";
		unsigned char addrZero[5] = {0,0,0,0,0};
		(*_blockOptr + 0)->printAddr(); std::cout << "\n";
		(*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH))->printAddr(); std::cout << "\n";
		std::cout << "--------" << "\n";
		omemcpy( (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH)).get() , addrZero , FREE_BLOCK_END_OPTR_LENGTH );
	}

	else{
		//omemcpy( (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH)).get() , target , FREE_BLOCK_END_OPTR_LENGTH );
		unsigned char addrZero[5] = {0,0,0,0,0};
		std::cout << "IMPORTANT MARK" << "\n";
		_blockOptr->printAddr(); std::cout << "\n";
		(*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH))->printAddr();	 std::cout << "\n";
		std::cout << "フリーブロックの終端 -> "; target->printAddr(); std::cout << "\n";
		omemcpy( (*_blockOptr + (PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH)).get() , target  , FREE_BLOCK_END_OPTR_LENGTH );
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
		std::cout << "♾️♾️♾️♾️♾️♾️♾️♾️♾️" << "\n";
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




std::unique_ptr<FreeBlockControlBlock> OverlayMemoryAllocator::placeControlBlock( optr* targetOptr  /* 新たにフリーブロックを配置する対象のOptr */, FreeBlockControlBlock* prevControlBlock, FreeBlockControlBlock *nextControlBlock, optr* freeBlockEnd )
{
	FreeBlockControlBlock targetControlBlock( targetOptr ); // 対象のコントロールブロック
	std::cout << "------" << "\n";
	targetControlBlock.blockOptr()->printAddr(); std::cout << "\n";
	std::cout << "------" << "\n";


	FreeBlockControlBlock* _prevControlBlock = (memcmp( prevControlBlock->blockOptr()->addr() , prevControlBlock->prevControlBlock()->blockOptr()->addr() , PREV_FREE_BLOCK_OPTR_LENGTH ) == 0 ) ? &targetControlBlock : prevControlBlock;
	FreeBlockControlBlock* _nextControlBlock = (memcmp( nextControlBlock->blockOptr()->addr() , nextControlBlock->nextControlBlock()->blockOptr()->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0 ) ? &targetControlBlock : nextControlBlock;


	targetControlBlock.prevControlBlock( _prevControlBlock );
	targetControlBlock.nextControlBlock( _nextControlBlock );


	if( freeBlockEnd == nullptr )
	{
		std::cout << "HERE 1" << "\n";
		targetControlBlock.freeBlockEnd( nullptr );
	}
	else{
		std::cout << "HERE 2" << "\n";
		targetControlBlock.freeBlockEnd( freeBlockEnd );
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

	std::cout << "コントロールブロックの先頭 :: "; controlBlockHead()->blockOptr()->printAddr(); std::cout << "\n";
	std::cout << "対象のコントロールブロック位置 :: " ; targetControlBlock->blockOptr()->printAddr(); std::cout << "\n";

	// 新しいフリーメモリ管理ブロックの作成
	unsigned int targetFreeBlockSize = targetControlBlock->freeBlockSize();
	std::cout << "確保されたフローブロックのサイズは -> " << targetFreeBlockSize << "\n";

	// これから割り当てるフリーブロックにフリーブロック管理ブロックを配置するスペースがない場合は設置しない
	if( FREE_BLOCK_CONTROL_BLOCK_LENGTH + allocateSize <= targetFreeBlockSize )
	{
		std::cout << "フリーブロック管理ブロックは設置しません" << "\n";
		targetControlBlock->prevControlBlock()->nextControlBlock( targetControlBlock->nextControlBlock().get() );
		targetControlBlock->nextControlBlock()->prevControlBlock( targetControlBlock->prevControlBlock().get() );

		return targetControlBlock->blockOptr();
	}

	// 新規にフリーブロックコントロールブロックを作成し配置する

	std::unique_ptr<FreeBlockControlBlock> placedNewControlBlock = placeControlBlock( (*(targetControlBlock->blockOptr()) + allocateSize).release() , targetControlBlock->prevControlBlock().get(), targetControlBlock->nextControlBlock().get(), targetControlBlock->freeBlockEnd().get() );

	if( memcmp( controlBlockHead()->blockOptr()->addr() , targetControlBlock->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0 )
	{
		std::cout << "MetaBlockの参照先が変更されます :: "; placedNewControlBlock->blockOptr()->printAddr(); std::cout << "\n";
		_metaBlock->controlBlockHead( placedNewControlBlock.get() );
		std::cout << "変更されました :: "; _metaBlock->controlBlockHead()->blockOptr()->printAddr(); std::cout << "\n";
	}


	return targetControlBlock->blockOptr();
}










/*
 解放時の挙動
 1. 単純に解放した領域にControlBlockを配置する
 2. 前のフリーブロックのEndPtrをずらす
*/
void OverlayMemoryAllocator::unallocate( optr* target , unsigned long size  )
{

	/* 割り当て解除されるOptrの直前のコントロールブロックを得る */
	std::unique_ptr<FreeBlockControlBlock> __controlBlockHead = controlBlockHead();
	std::unique_ptr<FreeBlockControlBlock> insertTargetPrevControlBlock = targetOptrPrevControlBlock( __controlBlockHead.get() ,target );

	if( insertTargetPrevControlBlock == nullptr )
 	{
		if( size >= FREE_BLOCK_CONTROL_BLOCK_LENGTH ) // コントロールブロックを配置する
		{
			std::unique_ptr<FreeBlockControlBlock> ret = placeControlBlock(  target , controlBlockHead()->prevControlBlock().get() , controlBlockHead().get() , (*target + size).get() );
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
	// エラーが発生する可能性あり
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




void OverlayMemoryAllocator::printControlChain( FreeBlockControlBlock* targetControlBlock )
{
	std::cout << __PRETTY_FUNCTION__ << "\n";
	int i=0;

	do
	{
		std::cout << "[ " << i << " ]" << "\n";
		i++;

	}while( ocmp( controlBlockHead()->blockOptr().get(), targetControlBlock->blockOptr().get() , NEXT_FREE_BLOCK_OPTR_LENGTH ) != 0 );

}



}; // close miya_db namespace
