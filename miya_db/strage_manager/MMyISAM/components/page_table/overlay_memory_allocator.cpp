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
先頭のフリーブロックへのポインタ(controlBlockZero) | 先頭フリーブロック(freeBlockHead) |
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










std::unique_ptr<ControlBlock> OverlayMemoryAllocator::findFreeBlock( ControlBlock *targetControlBlock, unsigned int allocateSize )
{
	if( targetControlBlock->freeBlockSize() == 0 ){ // フリーブロックが特に指定されていない場合は対象のコントロールブロックを返却する
		return std::make_unique<ControlBlock>(*targetControlBlock);
	}

	if( targetControlBlock->freeBlockSize() < allocateSize ){ // フリーブロックのサイズが指定の割り当てサイズより小さい場合
		std::cout << "再帰find"	 << "\n";
		return findFreeBlock( (targetControlBlock->nextControlBlock()).get() , allocateSize );
	}

	// このコントローブルロックを返却する
	return std::make_unique<ControlBlock>(*targetControlBlock);
}







std::unique_ptr<ControlBlock> OverlayMemoryAllocator::targetOptrPrevControlBlock( ControlBlock *targetControlBlock , optr *targetOptr )
{
	/* パターン1 (Prevが存在しない : targetが先頭 ) */
	// 1. Head > target
	if( memcmp( targetOptr->addr(), freeBlockHead()->blockOptr()->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) < 0 )
	{
		std::cout << "パターン 1" << "\n";
		return nullptr;
	}

	/* パターン2 (Prevが存在する : targetが領域最後尾) */
	// 1. 再帰中に,nextControlBlockがHead
	if( memcmp( targetControlBlock->blockOptr()->addr() , targetOptr->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) < 0 && memcmp(  targetControlBlock->blockOptr()->addr() ,targetControlBlock->nextControlBlock()->blockOptr()->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0 )
	{
		std::cout << "パターン 2" << "\n";
		return std::unique_ptr<ControlBlock>(targetControlBlock );
	}

	/* パターン3 (Prevが存在する : targetが領域中間) */
	// 1. 再帰中に,nextControlBlock > target
	// 2. 同　ControlBlock < target
	if( memcmp( targetControlBlock->blockOptr()->addr(), targetOptr->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) < 0 && memcmp( targetControlBlock->blockOptr()->addr(), targetOptr->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH) < 0 )
	{
		std::cout << "パターン 3" << "\n";
		return std::unique_ptr<ControlBlock>(targetControlBlock);
	}


	return targetOptrPrevControlBlock( targetControlBlock->nextControlBlock().get() , targetOptr );
}








/*
std::unique_ptr<ControlBlock> OverlayMemoryAllocator::placeControlBlock( optr* targetOptr , ControlBlock* prevControlBlock, ControlBlock *nextControlBlock, optr* freeBlockEnd , bool isReplace )
{
	ControlBlock targetControlBlock( targetOptr ); // 対象のコントロールブロック

	prevControlBlock->nextControlBlock( &targetControlBlock );
	nextControlBlock->prevControlBlock( &targetControlBlock );

	ControlBlock* _prevControlBlock = (memcmp( prevControlBlock->blockOptr()->addr() , prevControlBlock->prevControlBlock()->blockOptr()->addr() , PREV_FREE_BLOCK_OPTR_LENGTH ) == 0 ) ? &targetControlBlock : prevControlBlock;
	ControlBlock* _nextControlBlock = (memcmp( nextControlBlock->blockOptr()->addr() , nextControlBlock->nextControlBlock()->blockOptr()->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0 ) ? &targetControlBlock : nextControlBlock;

	targetControlBlock.prevControlBlock( _prevControlBlock );
	targetControlBlock.nextControlBlock( _nextControlBlock );


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


	return std::make_unique<ControlBlock>( targetControlBlock );
}








std::unique_ptr<ControlBlock> OverlayMemoryAllocator::rePlaceControlBlock( optr *destOptr , ControlBlock* fromControlBlock , optr* freeBlockEnd )
{
	ControlBlock targetControlBlock( destOptr );

	ControlBlock* _prevControlBlock = (memcmp( fromControlBlock->blockOptr()->addr() , fromControlBlock->prevControlBlock()->blockOptr()->addr() , PREV_FREE_BLOCK_OPTR_LENGTH ) == 0) ? &targetControlBlock : fromControlBlock->prevControlBlock().release();
	ControlBlock* _nextControlBlock = (memcmp( fromControlBlock->blockOptr()->addr() , fromControlBlock->nextControlBlock()->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0) ? &targetControlBlock : fromControlBlock->nextControlBlock().release();


	targetControlBlock.prevControlBlock( _prevControlBlock );
	targetControlBlock.nextControlBlock( _nextControlBlock );


	if( fromControlBlock->freeBlockEnd() == nullptr )
	{
		std::cout << "free block is null" << "\n";
		targetControlBlock.freeBlockEnd( nullptr );
	}
	else{
		std::cout << "free block is not null" << "\n";
		targetControlBlock.freeBlockEnd( freeBlockEnd );
		std::cout << "inserted free block below"  << "\n";
	}


	std::cout << "freeBlockHead()->blockOptr()->addr() -> "; freeBlockHead()->blockOptr()->printAddr(); std::cout << "\n";
	std::cout << "targetControlBlock.blockOptr()->addr() -> "; targetControlBlock.blockOptr()->printAddr(); std::cout << "\n";
	if( memcmp( freeBlockHead()->blockOptr()->addr() , targetControlBlock.blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) <= 0 )
	{
		std::cout << "freeBlockHead が変更されます" << "\n";
		_metaBlock->freeBlockHead( &targetControlBlock );
	}

	return std::make_unique<ControlBlock>( targetControlBlock );
}
*/




/*
std::unique_ptr<reeBlockControlBlock> getPrevControlBlock( FreeBlockControlBlock* targetControlBlock ,optr* target )
{
	if( ocmp( targetControlBlock->nextControlBlock()->blockOptr().get(), target , 5  ) < 0 && ocmp( targetControlBlock->prevControlBlock()->blockOptr().get(), target , 5 ) < 0 )
	{
		return std::unique_ptr<FreeBlockControlBlock>(targetControlBlock);
	}


	if( ocmp( targetControlBlock->nextControlBlock()->blockOptr().get() ,target , 5  ) > 0 && ocmp( targetControlBlock->blockOptr().get() , target , 5 ) < 0 )
	{
		return std::unique_ptr<FreeBlockControlBlock>(targetControlBlock);
	}

	return getPrevControlBlock( targetControlBlock->nextControlBlock().get() , target );
}
*/









OverlayMemoryAllocator::OverlayMemoryAllocator( int dataFileFD , int freeListFileFD )
{
	std::cout << __PRETTY_FUNCTION__ << "\n";

	if( dataFileFD < 0 ) return;
	_dataCacheTable = new CacheTable( dataFileFD );

	if( freeListFileFD < 0 ) return;
	_freeListCacheTable = new CacheTable( freeListFileFD );

	printf("Free List Cache Table ptr -> %p\n", const_cast<CacheTable*>(_freeListCacheTable));
	std::cout << "Process Passed Here" << "\n";

	/* MetaBlockの初期化 */
	unsigned char addrZero[5] = {0,0,0,0,0};
	optr* primaryOptr = new optr( addrZero );
	primaryOptr->cacheTable(  const_cast<CacheTable*>(_freeListCacheTable) );
	_metaBlock = new MetaBlock( primaryOptr );

	//_metaBlock = new MetaBlock( const_cast<optr*>(_primaryOptr) );

	if( !(_metaBlock->isFileFormatted() )) // ファイルのフォーマットを行う
		init();

	std::cout << "OverlayMemoryAllocator Initialize DONE" << "\n";
}










// 動作テストOK
void OverlayMemoryAllocator::init()
{
	//　ファイルの検証
	//　管理領域の配置
	//　初めのフリーブロック管理領域を配置

	// =========== 感領域を作成する前に,最初のコントロールブロックを作成する =====================
	ControlBlock firstControlBlock(  (*_metaBlock->primaryOptr() + META_BLOCK_SIZE).release() );


	firstControlBlock.prevControlBlock( &firstControlBlock );
	firstControlBlock.nextControlBlock( &firstControlBlock );
	

	unsigned char addrZero[5]	= {0,0,0,0,0};
	optr *_mappingOptr = new optr( addrZero ); // 初期のマッピング持つoptrの作成
	firstControlBlock.mappingOptr( _mappingOptr );

	std::cout << "firstControlBlock's mappingOptr address -> "; firstControlBlock.mappingOptr()->printAddr(); std::cout << "\n";
	firstControlBlock.freeBlockEnd( nullptr );


// ==========================================================================================
	_metaBlock->freeBlockHead( &firstControlBlock );
	_metaBlock->controlBlockTail( &firstControlBlock );
	_metaBlock->unUsedControlBlockHead( nullptr );

	//_metaBlock->controlBlockTail()->blockOptr()->printValueContinuously( 5 ); std::cout << "\n";

	omemcpy( (_metaBlock->primaryOptr()) , (unsigned char*)(FORMAT_ID) , 15 ); // フォーマットIDのセット
	// フォーマット完了

}







// どの仮想ファイルかを正確にする必要がある
std::unique_ptr<optr> OverlayMemoryAllocator::allocate( unsigned long allocateSize )
{

	// 一旦allocateSizeをunsigned char型に変換する
	unsigned char ucAllocateSize[5];
	ucAllocateSize[0] = 0; // 要修正
	ucAllocateSize[1] = (allocateSize >> 24 ) & 0xFF;
	ucAllocateSize[2] = (allocateSize >> 16 ) & 0xFF;
	ucAllocateSize[3]	= (allocateSize >> 8 ) & 0xFF;
	ucAllocateSize[4] = (allocateSize) & 0xFF;

	std::unique_ptr<ControlBlock> targetControlBlock = findFreeBlock( _metaBlock->freeBlockHead().get() , allocateSize );

	std::cout << "process here 0" << "\n";
	_metaBlock->freeBlockHead()->mappingOptr()->printAddr(); std::cout << "\n";


	/* 新たな割り当てブロックを作成する */
	std::unique_ptr<ControlBlock> newAllocatedBlock = unUsedControlBlockHead(); // 取得と同時に未使用ブロックはチェーンから外される
	std::cout << "process here 1"	 << "\n";

	if( newAllocatedBlock == nullptr )
		newAllocatedBlock = newControlBlock();

	std::cout << "new allocated block ptr -> "; newAllocatedBlock->blockOptr()->printAddr(); std::cout << "\n";


	std::cout << "newAllocateBlock's mapping optr address -> "; newAllocatedBlock->blockOptr()->printAddr(); std::cout << "\n";

	newAllocatedBlock->mappingOptr( targetControlBlock->mappingOptr().get() );
	newAllocatedBlock->freeBlockEnd( targetControlBlock->freeBlockEnd().get() );	
	_metaBlock->allocatedBlockHead( newAllocatedBlock.get() , targetControlBlock.get() ); // テスト



	std::cout << "---------------" << "\n";
	std::cout << "newAllocateBlock generated with newControlBlock -> "; newAllocatedBlock->blockOptr()->printAddr(); std::cout << "\n";
	std::cout << "---------------" << "\n";
	
	if( targetControlBlock->freeBlockSize() == 0 || targetControlBlock->freeBlockSize() > allocateSize )
	{ // 新たなフリーブロックを作成する
		std::cout << "新たなフリーブロックを作成します"	<< "\n";

	
		std::unique_ptr<ControlBlock> newFreeBlock = unUsedControlBlockHead(); // 取得と同時に未使用ブロックはチェーンから外される
		if( newFreeBlock == nullptr )
		{
			std::cout << "unUsedControlBlockHead retuend nullptr and generate new control block from newControlBlock" << "\n";
			newFreeBlock = newControlBlock();
		}

		std::cout << "処理はここまで来ました" << "\n";

		std::cout << "---------------" << "\n";
		std::cout << "newFreeBlock generated with newControlBlock -> "; newFreeBlock->blockOptr()->printAddr(); std::cout << "\n";
		std::cout << "---------------" << "\n";
	

	
		newFreeBlock->mappingOptr( ( *(newAllocatedBlock->mappingOptr()) + allocateSize ).get() );
		

		newFreeBlock->freeBlockEnd( targetControlBlock->freeBlockEnd().get() );

		
		std::cout << "#################" << "\n";
		targetControlBlock->blockOptr()->printAddr(); std::cout << "\n";
		targetControlBlock->prevControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		targetControlBlock->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		std::cout << "#################" << "\n";
		newAllocatedBlock->mappingOptr()->printAddr(); std::cout << "\n";


		toUnUsedControlBlock( targetControlBlock.get() ); // 対象のコントロール(フリー)ブロックをチェーンから外す
		std::cout << "toUnUsedControlBlock Func Done"	 << "\n";


		std::cout << "~~~~~~~~~~~~~~~~~~~~~~" << "\n";
		newFreeBlock->blockOptr()->printAddr(); std::cout << "\n";
		newFreeBlock->prevControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		newFreeBlock->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~" << "\n";
		newAllocatedBlock->mappingOptr()->printAddr(); std::cout << "\n";
		printf("%p\n", _metaBlock->freeBlockHead().get() );



		// 問題1 :  freeBlockHeadにblockを突っ込んでもprevとnextが設定されていない
		// 問題2 : mappingOptr()のマッピング値がずれている

		_metaBlock->freeBlockHead( newFreeBlock.get() );


	
		
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~" << "\n";
		_metaBlock->freeBlockHead()->blockOptr()->printAddr(); std::cout << "\n";
		_metaBlock->freeBlockHead()->prevControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		_metaBlock->freeBlockHead()->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~" << "\n";
		newAllocatedBlock->mappingOptr()->printAddr();  std::cout << "\n";

	}else
	{
		toUnUsedControlBlock( targetControlBlock.get() ); // 対象のコントロール(フリー)ブロックをチェーンから外す
	}



	std::cout << "Error Done" << "\n";

	
	std::cout << "!!!!!!!!!!!!!" << "\n";
	newAllocatedBlock->mappingOptr()->printAddr(); std::cout << "\n";
	optr *ret = new optr( newAllocatedBlock->mappingOptr()->addr() );
	ret->cacheTable( const_cast<CacheTable*>(_dataCacheTable) );


	return std::make_unique<optr>( *ret );
}










/*
 解放時の挙動
 1. 単純に解放した領域にControlBlockを配置する
 2. 前のフリーブロックのEndPtrをずらす
*/
void OverlayMemoryAllocator::deallocate( optr* target )
{
	std::cout << "Deallocate Called" << "\n";

	/* 割り当て解除されるOptrの直前のコントロールブロックを得る */
	//std::unique_ptr<FreeBlockControlBlock> __freeBlockHead = freeBlockHead();

	/*
	std::unique_ptr<FreeBlockControlBlock> insertTargetPrevControlBlock = targetOptrPrevControlBlock( freeBlockHead().release() ,target );

	if( insertTargetPrevControlBlock == nullptr )
 	{
		if( size >= FREE_BLOCK_CONTROL_BLOCK_LENGTH ) // コントロールブロックを配置する
		{
			std::unique_ptr<FreeBlockControlBlock> ret = placeControlBlock(  target , freeBlockHead()->prevControlBlock().get() , freeBlockHead().get() , (*target + size).release() );
			_metaBlock->freeBlockHead( ret.get() );
			mergeControlBlock( ret.get() );
		}
		return;
	}



	// prevが存在する場合は,一旦controlBlockを作成して，必要であればmergeで削除する
	std::cout << "新たなコントロールブロックが配置される前のコントロールブロック ->->-> ";  insertTargetPrevControlBlock->blockOptr()->printAddr(); std::cout << "\n";
	placeControlBlock( target , insertTargetPrevControlBlock.get() , insertTargetPrevControlBlock->nextControlBlock().get() , (*target + size).release() );
	mergeControlBlock( insertTargetPrevControlBlock.get() );
	*/

	return;
}








void OverlayMemoryAllocator::mergeControlBlock( ControlBlock *targetControlBlock )
{

	if( targetControlBlock->freeBlockEnd() == nullptr ) return;
	//FreeBlockControlBlock freeBlockEnd( targetControlBlock->freeBlockEnd().get() );

	// エラーが発生する可能性あり
	if( memcmp( targetControlBlock->freeBlockEnd()->addr(), targetControlBlock->nextControlBlock()->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0 )
	{
		std::cout << "マージされます" << "\n";
		targetControlBlock->freeBlockEnd( targetControlBlock->nextControlBlock()->freeBlockEnd().get() );
		targetControlBlock->nextControlBlock()->nextControlBlock()->prevControlBlock( targetControlBlock );
		targetControlBlock->nextControlBlock( targetControlBlock->nextControlBlock()->nextControlBlock().get() );

		targetControlBlock->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		targetControlBlock->prevControlBlock()->blockOptr()->printAddr(); std::cout << "\n";

		return mergeControlBlock( targetControlBlock->nextControlBlock().get() );
	}
	else
	{
		std::cout << "マージされませんでした" << "\n";
	}

	return;
}



void OverlayMemoryAllocator::printControlChain( ControlBlock* targetControlBlock )
{
	int i=0;

	do
	{
		sleep(1);
		std::cout << "[ " << i << " ]" << "\n";
		i++;

		std::cout << "[ ADDRESS ] : "; targetControlBlock->blockOptr()->printAddr(); std::cout << "\n";
		std::cout << "[ PREV ] : "; targetControlBlock->prevControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		std::cout << "[ NEXT ] : "; targetControlBlock->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		std::cout << "[ FREE BLOCK SIZE ] : " << targetControlBlock->freeBlockSize() << "\n";
		std::cout << "\n\n\n";

		targetControlBlock = targetControlBlock->nextControlBlock().release();

	}while( memcmp( freeBlockHead()->blockOptr()->addr() , targetControlBlock->blockOptr()->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) != 0 );


}


/*
std::unique_ptr<ControlBlock> OverlayMemoryAllocator::getUnUsedControlBlock()
{

	std::unique_ptr<ControlBlock> ret = unUsedControlBlockHead();
	unsigned char* controlBlockTailOptrAddr = nullptr;

	if( ret == nullptr ) // 使用されていない最後尾のコントロールブロックを得る
	{
		unsigned char *freeBlockTailAddr = ( freeBlockHead() == nullptr ) ? nullptr : freeBlockHead()->prevControlBlock()->blockOptr()->addr();
		unsigned char *allocateBlockTailAddr = ( allocatedBlockHead() == nullptr ) ? nullptr : allocatedBlockHead()->prevControlBlock()->blockOptr()->addr();

		if( freeBlockTailAddr == nullptr && allocateBlockTailAddr == nullptr )
			return nullptr;  // エラー発生
		else if( freeBlockTailAddr == nullptr || allocateBlockTailAddr == nullptr )
			controlBlockTailOptrAddr = ( freeBlockTailAddr == nullptr ) ? allocateBlockTailAddr : freeBlockTailAddr;
		else
			controlBlockTailOptrAddr = ( memcmp(freeBlockTailAddr , allocateBlockTailAddr , CONTROL_BLOCK_LENGTH ) > 0 ) ? freeBlockTailAddr : allocateBlockTailAddr;

		optr controlBlockTailOptr( controlBlockTailOptrAddr );
		ControlBlock newUnUsedControlBlock( &controlBlockTailOptr );

		return std::make_unique<ControlBlock>( newUnUsedControlBlock );
	}

	// 既存の未使用コントロールブロックが存在する場合
	if(memcmp( ret->nextControlBlock()->blockOptr()->addr(), unUsedControlBlockHead()->blockOptr()->addr(), CONTROL_BLOCK_LENGTH ) == 0) // 未使用ブロックが１つの場合
		_metaBlock->unUsedControlBlockHead( nullptr );
	else
		_metaBlock->unUsedControlBlockHead( unUsedControlBlockHead()->nextControlBlock().get() );

	return ret;
}
*/






std::unique_ptr<ControlBlock> OverlayMemoryAllocator::newControlBlock()
{
	ControlBlock newControlBlock( ( *(_metaBlock->controlBlockTail()->blockOptr()) + FREE_BLOCK_CONTROL_BLOCK_LENGTH ).release() );
	newControlBlock.blockOptr()->cacheTable( const_cast<CacheTable*>(_freeListCacheTable) ); // セットしないとなぜかエラ〜になる

	std::cout << "new control block generated with -> "; newControlBlock.blockOptr()->printAddr(); std::cout << "\n";
	_metaBlock->controlBlockTail( &newControlBlock );
	return std::make_unique<ControlBlock>( newControlBlock );	
}







//  指定のコントロールブロックを所属しているリストから外す
void OverlayMemoryAllocator::toUnUsedControlBlock( ControlBlock *targetControlBlock )
{
	

	std::cout << "=============================" << "\n";
	std::cout << "=============================" << "\n";
	std::cout << "=============================" << "\n";
	std::cout << "=============================" << "\n";
	targetControlBlock->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
	targetControlBlock->prevControlBlock()->blockOptr()->printAddr(); std::cout << "\n";

	ControlBlock* _prevControlBlock = nullptr;
	if(  memcmp( targetControlBlock->prevControlBlock()->blockAddr() , targetControlBlock->blockAddr() , PREV_FREE_BLOCK_OPTR_LENGTH ) == 0  )
		_prevControlBlock = nullptr;
	else
		_prevControlBlock = targetControlBlock->prevControlBlock().get();

	std::cout << "check point 1 " << "\n";

	ControlBlock* _nextControlBlock = nullptr;
	if(  memcmp( targetControlBlock->nextControlBlock()->blockAddr() , targetControlBlock->blockAddr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0  )
		_nextControlBlock = nullptr;
	else
		_nextControlBlock = targetControlBlock->nextControlBlock().get();


	std::cout << "check point 2 " << "\n";

	// コントロールブロックが削除後に空になる場合	
	if( _prevControlBlock == nullptr || _nextControlBlock == nullptr )
	{
		std::cout << "対象のコントロールブロックは空になります" << "\n";
		targetControlBlock->blockOptr()->printAddr(); std::cout << "\n";
		targetControlBlock->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		targetControlBlock->prevControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		
		printf("%p\n" ,_prevControlBlock );
		printf("%p\n", _nextControlBlock ); // 次のコントロールブロックが空になっている


			if ( memcmp( _metaBlock->freeBlockHead()->blockAddr(), targetControlBlock->blockAddr(), CONTROL_BLOCK_LENGTH ) == 0 )
			{
				std::cout << "branch 1" << "\n";
				_metaBlock->freeBlockHead(nullptr);
			}
			else if ( memcmp( _metaBlock->allocatedBlockHead()->blockOptr()->addr(), targetControlBlock->blockOptr()->addr(), CONTROL_BLOCK_LENGTH ) == 0 )
			{
				std::cout << "branch 2" << "\n";
				_metaBlock->allocatedBlockHead( nullptr , nullptr );
			}
			else if ( memcmp( _metaBlock->unUsedControlBlockHead()->blockOptr()->addr(), targetControlBlock->blockOptr()->addr(), CONTROL_BLOCK_LENGTH ) == 0 )
			{
				std::cout << "branch 3" << "\n";
				_metaBlock->unUsedControlBlockHead( nullptr );
			}

			std::cout << "toUnUsedControlBlock retuend point 1" << "\n";
			return;
	}

	std::cout << "check point 3" << "\n";
	printf("%p\n", _prevControlBlock );

	_prevControlBlock->nextControlBlock( targetControlBlock->nextControlBlock().get() );
	_nextControlBlock->prevControlBlock( targetControlBlock->prevControlBlock().get() );		

	std::cout << "check point 4" << "\n";

	if( _metaBlock->unUsedControlBlockHead() == nullptr ) // 未使用ブロックが空であればそのまま追加する
	{
		_metaBlock->unUsedControlBlockHead( targetControlBlock );
	}
	else{ // 未使用ブロックに既存のブロックがある場合は,チェーンに繋げる
		targetControlBlock->nextControlBlock( _metaBlock->unUsedControlBlockHead()->nextControlBlock().get() );
		targetControlBlock->prevControlBlock( _metaBlock->unUsedControlBlockHead().get() );
		
		_metaBlock->unUsedControlBlockHead()->nextControlBlock()->prevControlBlock( targetControlBlock );
		_metaBlock->unUsedControlBlockHead()->nextControlBlock( targetControlBlock );
	}

	std::cout << "toUnUsedControlBlock retuend point 2" << "\n";
	
}











std::unique_ptr<ControlBlock> OverlayMemoryAllocator::freeBlockHead()
{
	return _metaBlock->freeBlockHead();
}



std::unique_ptr<ControlBlock> OverlayMemoryAllocator::allocatedBlockHead()
{
	return _metaBlock->allocatedBlockHead();
}




std::unique_ptr<ControlBlock> OverlayMemoryAllocator::unUsedControlBlockHead()
{
	return _metaBlock->unUsedControlBlockHead();
}








}; // close miya_db namespace
