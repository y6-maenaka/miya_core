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







std::unique_ptr<ControlBlock> OverlayMemoryAllocator::findFreeBlock( optr* targetOptr )
{
	std::cout << "findFreeBlock Called" << "\n";

	std::unique_ptr<ControlBlock> currentFreeBlock = _metaBlock->freeBlockHead();
	if( currentFreeBlock == nullptr ) return nullptr;

	do
	{
		if( memcmp( targetOptr->addr() , currentFreeBlock->mappingOptr()->addr() , CONTROL_BLOCK_LENGTH ) == 0 ) return currentFreeBlock;
		currentFreeBlock = currentFreeBlock->nextControlBlock();
	}
	while( memcmp( currentFreeBlock->blockAddr() , _metaBlock->freeBlockHead()->blockAddr() , CONTROL_BLOCK_LENGTH  ) != 0 );

	return nullptr;
}



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



std::unique_ptr<ControlBlock> OverlayMemoryAllocator::findAllocatedBlock( optr *targetOptr )
{

	std::unique_ptr<ControlBlock> currentAllocatedBlock = allocatedBlockHead();
	if( currentAllocatedBlock.get() == nullptr ) return nullptr;

	do
	{
		if( memcmp( currentAllocatedBlock->mappingOptr()->addr() , targetOptr->addr() , CONTROL_BLOCK_LENGTH ) == 0 ) return currentAllocatedBlock;
		currentAllocatedBlock = currentAllocatedBlock->nextControlBlock();
	}
	while( memcmp( currentAllocatedBlock->blockAddr() , _metaBlock->allocatedBlockHead()->blockAddr() , CONTROL_BLOCK_LENGTH ) != 0 );

	return nullptr;

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


OverlayMemoryAllocator::OverlayMemoryAllocator( int dataFileFD , int freeListFileFD )
{
	if( dataFileFD < 0 ) return;
		//_dataCacheTable = new CacheTable( dataFileFD );
		_dataCacheTable = std::shared_ptr<CacheTable>( new CacheTable(dataFileFD) );
		//_dataCacheTable = std::shared_ptr<CacheTable>( new CacheTable(dataFileFD) );

	if( freeListFileFD < 0 ) return;
		//_freeListCacheTable = new CacheTable( freeListFileFD );
		_freeListCacheTable = std::shared_ptr<CacheTable>( new CacheTable(freeListFileFD) );
		//_freeListCacheTable = std::shared_ptr<CacheTable>( new CacheTable(freeListFileFD) );


	printf("Seted DataCache CacheTable with -> %d - %p\n" , dataFileFD , _dataCacheTable.get() );
	printf("Seted FreeList CacheTable with -> %d - %p\n" , freeListFileFD ,_freeListCacheTable.get() );

	/* MetaBlockの初期化 */
	unsigned char addrZero[5] = {0,0,0,0,0};
	optr* primaryOptr = new optr( addrZero );
	primaryOptr->cacheTable(  _freeListCacheTable );
	_metaBlock = new MetaBlock( primaryOptr );


	//_metaBlock = new MetaBlock( const_cast<optr*>(_primaryOptr) );

	if( !(_metaBlock->isFileFormatted() )) // ファイルのフォーマットを行う
		init();
}


// 動作テストOK
void OverlayMemoryAllocator::init()
{
	//　ファイルの検証
	//　管理領域の配置
	//　初めのフリーブロック管理領域を配置


	// =========== 感領域を作成する前に,最初のコントロールブロックを作成する =====================
	ControlBlock firstControlBlock(  (*_metaBlock->primaryOptr() + META_BLOCK_SIZE) );



	firstControlBlock.prevControlBlock( &firstControlBlock );
	firstControlBlock.nextControlBlock( &firstControlBlock );


	unsigned char addrZero[5]	= {0,0,0,0,0};
	optr *_mappingOptr = new optr( addrZero ); // 初期のマッピング持つoptrの作成
	firstControlBlock.mappingOptr( _mappingOptr );

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
std::shared_ptr<optr> OverlayMemoryAllocator::allocate( unsigned long allocateSize )
{
	// 一旦allocateSizeをunsigned char型に変換する
	unsigned char ucAllocateSize[5];
	ucAllocateSize[0] = 0; // 要修正
	ucAllocateSize[1] = (allocateSize >> 24 ) & 0xFF;
	ucAllocateSize[2] = (allocateSize >> 16 ) & 0xFF;
	ucAllocateSize[3] = (allocateSize >> 8 ) & 0xFF;
	ucAllocateSize[4] = (allocateSize) & 0xFF;

	std::unique_ptr<ControlBlock> targetControlBlock = findFreeBlock( _metaBlock->freeBlockHead().get() , allocateSize );

	/* 新たな割り当てブロックを作成する */
	std::unique_ptr<ControlBlock> newAllocatedBlock =  _metaBlock->useUnUsedControlBlockHead(); // 取得と同時に未使用ブロックはチェーンから外される

	if( newAllocatedBlock == nullptr )
		newAllocatedBlock = newControlBlock();


	newAllocatedBlock->mappingOptr( targetControlBlock->mappingOptr().get() );
	newAllocatedBlock->freeBlockEnd(  ( *(newAllocatedBlock->mappingOptr()) + allocateSize).get() );
	_metaBlock->allocatedBlockHead( newAllocatedBlock.get() );

	//std::cout << "Generated New Allocated Block Address with -> "; newAllocatedBlock->blockOptr()->printAddr(); std::cout << "\n";

	if( targetControlBlock->freeBlockSize() == 0 || targetControlBlock->freeBlockSize() > allocateSize )
	{ // 新たなフリーブロックを作成する

		std::unique_ptr<ControlBlock> newFreeBlock = _metaBlock->useUnUsedControlBlockHead(); // 取得と同時に未使用ブロックはチェーンから外される
		if( newFreeBlock == nullptr )
			newFreeBlock = newControlBlock();

		//std::cout << "Generated New Free Block Address with -> "; newFreeBlock->blockOptr()->printAddr(); std::cout << "\n";

		newFreeBlock->mappingOptr( ( *(newAllocatedBlock->mappingOptr()) + allocateSize ).get() );
		newFreeBlock->freeBlockEnd( targetControlBlock->freeBlockEnd().get() );

		toUnUsedControlBlock( targetControlBlock.get() ); // 対象のコントロール(フリー)ブロックをチェーンから外す

		// 問題1 :  freeBlockHeadにblockを突っ込んでもprevとnextが設定されていない
		// 問題2 : mappingOptr()のマッピング値がずれている

		_metaBlock->freeBlockHead( newFreeBlock.get() );

	}else
	{
		toUnUsedControlBlock( targetControlBlock.get() ); // 対象のコントロール(フリー)ブロックをチェーンから外す
	}

	std::shared_ptr<optr> ret = std::shared_ptr<optr>( new optr( newAllocatedBlock->mappingOptr()->addr()) );
	ret->cacheTable( _dataCacheTable );

	return ret;
	std::cout << "Retuened Optr" << "\n";

	//return std::make_shared<optr>( *ret );
}


/*
 解放時の挙動
 1. 単純に解放した領域にControlBlockを配置する
 2. 前のフリーブロックのEndPtrをずらす
*/
void OverlayMemoryAllocator::deallocate( optr* target )
{
	std::unique_ptr<ControlBlock> allocatedBlock = findAllocatedBlock( target );
	// allocatedBlock->blockOptr()->printAddr(); std::cout << "\n";

	if( allocatedBlock == nullptr )
	{
		throw std::runtime_error("Trying to deallocate an unallocated optr.");
		return;
	}

	// 解放する領域の分のフリーブロックを作成する
	std::unique_ptr<ControlBlock> newFreeBlock = _metaBlock->useUnUsedControlBlockHead();
	if( newFreeBlock == nullptr ) newFreeBlock = newControlBlock();

	newFreeBlock->mappingOptr( target );
	newFreeBlock->freeBlockEnd( allocatedBlock->freeBlockEnd().get() );

	_metaBlock->freeBlockHead( newFreeBlock.get() ); // この時点ではフローブロックは2つのはず


	toUnUsedControlBlock( allocatedBlock.get()  ); // 割り当てブロックを解放する

	mergeControlBlock( newFreeBlock.get() ); // マージ処理を行う
}


void OverlayMemoryAllocator::mergeControlBlock( ControlBlock *targetControlBlock )
{

	std::cout << "mergeControlBlock Caleld" << "\n";
	if( targetControlBlock->freeBlockEnd() == nullptr ) return;
	//FreeBlockControlBlock freeBlockEnd( targetControlBlock->freeBlockEnd().get() );

	// エラーが発生する可能性あり

	std::unique_ptr<ControlBlock>  mergedControlBlock = findFreeBlock( targetControlBlock->freeBlockEnd().get() );

	// mergedControlBlock->blockOptr()->printAddr(); std::cout << "\n";

	if( mergedControlBlock == nullptr ) return;


	targetControlBlock->freeBlockEnd( mergedControlBlock->freeBlockEnd().get() ); // マッピング領域の終端を伸ばす

	toUnUsedControlBlock( mergedControlBlock.get() ); // マージされるコントロールブロックを破棄する


	/*
	targetControlBlock->nextControlBlock( mergedControlBlock->nextControlBlock().get() );
	if(memcmp( targetControlBlock->prevControlBlock()->blockAddr() , mergedControlBlock->blockAddr() , CONTROL_BLOCK_LENGTH  ) == 0)
		targetControlBlock->prevControlBlock( targetControlBlock );
	*/

	return mergeControlBlock( targetControlBlock );
}


void OverlayMemoryAllocator::printFreeBlockChain()
{
	int i=0;
	ControlBlock *targetControlBlock = _metaBlock->freeBlockHead().release();

	do
	{
		std::cout << "[ " << i << " ]" << "\n";
		i++;

		std::cout << "[ ADDRESS ] : "; targetControlBlock->blockOptr()->printAddr(); std::cout << "\n";
		std::cout << "[ MAPPING ADDRESS  : ]"; targetControlBlock->mappingOptr()->printAddr(); std::cout << "\n";
		std::cout << "[ PREV ] : "; targetControlBlock->prevControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		std::cout << "[ NEXT ] : "; targetControlBlock->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		std::cout << "[ FREE BLOCK SIZE ] : " << targetControlBlock->freeBlockSize() << "\n";
		std::cout << "\n\n\n";

		targetControlBlock = targetControlBlock->nextControlBlock().release();

	}while( memcmp( freeBlockHead()->blockOptr()->addr() , targetControlBlock->blockOptr()->addr(), NEXT_FREE_BLOCK_OPTR_LENGTH ) != 0 );
}


void OverlayMemoryAllocator::printControlFile()
{
	unsigned char emptyControlBlock[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned char firstControlBlockOptrAddr[5] = {0,0,0,0,0x64};
	//optr* firstControlBlockOptr = new optr( firstControlBlockOptrAddr );
	//std::shared_ptr<optr> firstControlBlockOptr = new optr( firstControlBlockOptrAddr );
	std::shared_ptr<optr> firstControlBlockOptr = std::shared_ptr<optr>( new optr(firstControlBlockOptrAddr) );
	firstControlBlockOptr->cacheTable( _freeListCacheTable );

	//firstControlBlockOptr->printValueContinuously( 20 ); std::cout << "\n";

	std::cout << "\n\n\n============================" << "\n\n";
	std::cout << "[ FREE ] : ";
	if(  _metaBlock->freeBlockHead() == nullptr ) std::cout << "nullptr" << "\n";
	else{ _metaBlock->freeBlockHead()->blockOptr()->printAddr(); std::cout << "\n";}


	std::cout << "[ ALLOCATED ] : "	;
	if(  _metaBlock->allocatedBlockHead() == nullptr ) std::cout << "nullptr" << "\n";
	else{ _metaBlock->allocatedBlockHead()->blockOptr()->printAddr(); std::cout << "\n";}


	std::cout << "[ UNUSED ] : "	;
	if(  _metaBlock->unUsedControlBlockHead() == nullptr ) std::cout << "nullptr" << "\n";
	else{ _metaBlock->unUsedControlBlockHead()->blockOptr()->printAddr(); std::cout << "\n";}

	std::cout << "[ TAIL ] : "	;
	if(  _metaBlock->controlBlockTail() == nullptr ) std::cout << "nullptr" << "\n";
	else{ _metaBlock->controlBlockTail()->blockOptr()->printAddr(); std::cout << "\n";}
	std::cout << "\n============================" << "\n";

	//ControlBlock *tmp;
	std::shared_ptr<ControlBlock> tmp;
	while( ocmp( firstControlBlockOptr.get() , (unsigned char*)emptyControlBlock , 20 ) != 0 )
	{
		//tmp = new ControlBlock( firstControlBlockOptr );
		tmp = std::shared_ptr<ControlBlock>( new ControlBlock(firstControlBlockOptr) );

		std::cout << "\n\n----------------------------" << "\n";
		std::cout << "[ ADDRESS ] : ";tmp->blockOptr()->printAddr(); std::cout << "\n";
		std::cout << "[ PREV ] : "; tmp->prevControlBlock()->blockOptr()->printAddr();  std::cout << "\n";
		std::cout << "[ NEXT ] : "; tmp->nextControlBlock()->blockOptr()->printAddr();  std::cout << "\n";
		std::cout << "[ MAPPING ] : "; tmp->mappingOptr()->printAddr(); std::cout << "\n";

		std::cout << "[ END ] : ";
		if(  tmp->freeBlockEnd() == nullptr ) std::cout << "nullptr" << "\n";
		else{ tmp->freeBlockEnd()->printAddr(); std::cout << "\n";}


		std::cout << "----------------------------" << "\n\n";

		firstControlBlockOptr = ((*firstControlBlockOptr) + 20);
	}
	return;
}


std::unique_ptr<ControlBlock> OverlayMemoryAllocator::newControlBlock()
{
	ControlBlock newControlBlock( ( *(_metaBlock->controlBlockTail()->blockOptr()) + FREE_BLOCK_CONTROL_BLOCK_LENGTH ) );
	newControlBlock.blockOptr()->cacheTable( _freeListCacheTable ); // セットしないとなぜかエラ〜になる

	_metaBlock->controlBlockTail( &newControlBlock );
	return std::make_unique<ControlBlock>( newControlBlock );
}


//  指定のコントロールブロックを所属しているリストから外す
void OverlayMemoryAllocator::toUnUsedControlBlock( ControlBlock *targetControlBlock )
{
	ControlBlock* _prevControlBlock = nullptr;
	if(  memcmp( targetControlBlock->prevControlBlock()->blockAddr() , targetControlBlock->blockAddr() , PREV_FREE_BLOCK_OPTR_LENGTH ) == 0  )
		_prevControlBlock = nullptr;
	else{
		_prevControlBlock = targetControlBlock->prevControlBlock().release();
	}


	ControlBlock* _nextControlBlock = nullptr;
	if(  memcmp( targetControlBlock->nextControlBlock()->blockAddr() , targetControlBlock->blockAddr() , NEXT_FREE_BLOCK_OPTR_LENGTH ) == 0  )
		_nextControlBlock = nullptr;
	else
		_nextControlBlock = targetControlBlock->nextControlBlock().get();


	// コントロールブロックが削除後に空になる場合	 -> 単に先頭をクリアする
	if( _prevControlBlock == nullptr || _nextControlBlock == nullptr )
	{
			//std::cout << "コントロールブロック解放後に先頭要素が空になります" << "\n";
			if ( memcmp( _metaBlock->freeBlockHead()->blockAddr(), targetControlBlock->blockAddr(), CONTROL_BLOCK_LENGTH ) == 0 )
			{
				_metaBlock->freeBlockHead(nullptr);
			}
			else if ( memcmp( _metaBlock->allocatedBlockHead()->blockOptr()->addr(), targetControlBlock->blockOptr()->addr(), CONTROL_BLOCK_LENGTH ) == 0 )
			{
				_metaBlock->allocatedBlockHead( nullptr );
			}
			else if ( memcmp( _metaBlock->unUsedControlBlockHead()->blockOptr()->addr(), targetControlBlock->blockOptr()->addr(), CONTROL_BLOCK_LENGTH ) == 0 )
			{
				_metaBlock->unUsedControlBlockHead( nullptr );
			}

			_metaBlock->unUsedControlBlockHead( targetControlBlock );


			// _metaBlock->unUsedControlBlockHead()->blockOptr()->printAddr(); std::cout << "\n";
			return;
	}

	/*
	 ------------------------
	 | Prev | Target | Next |
	 ------------------------
	 */


	std::cout << "DONE"  << "\n";

	if ( _metaBlock->freeBlockHead() != nullptr &&  memcmp( _metaBlock->freeBlockHead()->blockAddr(), targetControlBlock->blockAddr(), CONTROL_BLOCK_LENGTH ) == 0 )
	{
		_metaBlock->freeBlockHead( targetControlBlock->nextControlBlock().get() );
	}
	else if ( _metaBlock->allocatedBlockHead() != nullptr && memcmp( _metaBlock->allocatedBlockHead()->blockOptr()->addr(), targetControlBlock->blockOptr()->addr(), CONTROL_BLOCK_LENGTH ) == 0 )
	{
		_metaBlock->allocatedBlockHead( targetControlBlock->nextControlBlock().get() );
	}
	else if ( _metaBlock->unUsedControlBlockHead() != nullptr  && memcmp( _metaBlock->unUsedControlBlockHead()->blockOptr()->addr(), targetControlBlock->blockOptr()->addr(), CONTROL_BLOCK_LENGTH ) == 0 )
	{
		_metaBlock->unUsedControlBlockHead( targetControlBlock->nextControlBlock().get() );
	}


	// 割り当て解除対象の前後のブロックの接続を切る必要がある

	targetControlBlock->nextControlBlock()->prevControlBlock( targetControlBlock->prevControlBlock().get() );
	targetControlBlock->prevControlBlock()->nextControlBlock( targetControlBlock->nextControlBlock().get() );


	/* この怪しい */
	//targetControlBlock->prevControlBlock()->nextControlBlock( targetControlBlock->nextControlBlock().get() );
	//_prevControlBlock->nextControlBlock( targetControlBlock->nextControlBlock().get() );

	//targetControlBlock->nextControlBlock()->prevControlBlock( targetControlBlock->prevControlBlock().get() );
	//_nextControlBlock->prevControlBlock( targetControlBlock->prevControlBlock().get() );





	_metaBlock->unUsedControlBlockHead( targetControlBlock );

	/*
	if( _metaBlock->unUsedControlBlockHead() == nullptr ) // 未使用ブロックが空であればそのまま追加する
	{
		std::cout << "未使用ブロックの先頭に配置されます" << "\n";
		printf("%p\n", targetControlBlock );
		_metaBlock->unUsedControlBlockHead( targetControlBlock );
	}
	else
	{ // 未使用ブロックに既存のブロックがある場合は,チェーンに繋げる
		std::cout << "未使用ブロックチェーンにマージされます" << "\n";
		targetControlBlock->nextControlBlock( _metaBlock->unUsedControlBlockHead()->nextControlBlock().get() );
		targetControlBlock->prevControlBlock( _metaBlock->unUsedControlBlockHead().get() );

		_metaBlock->unUsedControlBlockHead()->nextControlBlock()->prevControlBlock( targetControlBlock );
		_metaBlock->unUsedControlBlockHead()->nextControlBlock( targetControlBlock );
	}
	*/

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


std::shared_ptr<optr> OverlayMemoryAllocator::get( unsigned char* oAddr )
{
	std::shared_ptr<optr> ret = std::shared_ptr<optr>( new optr(oAddr) );
	ret->cacheTable( _dataCacheTable );

	return ret;
}


void OverlayMemoryAllocator::clear()
{
	init();
}


std::shared_ptr<CacheTable> OverlayMemoryAllocator::dataCacheTable()
{
	//return std::shared_ptr<CacheTable>(const_cast<CacheTable*>(_dataCacheTable));
	return _dataCacheTable;
}


std::shared_ptr<CacheTable> OverlayMemoryAllocator::freeListCacheTable()
{
	//return std::shared_ptr<CacheTable>(const_cast<CacheTable*>(_freeListCacheTable));
	return _freeListCacheTable;
}


MetaBlock* OverlayMemoryAllocator::metaBlock()
{
  return _metaBlock;
}

}; // close miya_db namespace
