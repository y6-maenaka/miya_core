#include "overlay_memory_allocator.h"


#include "./optr_utils.h"

namespace miya_db
{


/*
 DONE 1. allocatedBlockHead
 DONE 2. freeBlockHead
 3. unUsedControlBlock
*/




MetaBlock::MetaBlock( optr *primaryOptr )
{
	_primaryOptr = primaryOptr;
}



optr* MetaBlock::primaryOptr()
{
	return _primaryOptr;
}


void MetaBlock::freeBlockHead( ControlBlock *targetControlBlock )
{
	if( targetControlBlock == nullptr ) // 先頭コントロールブロックを初期化する場合
	{
		unsigned char addrZero[5] = {0,0,0,0,0};
		omemcpy( (*_primaryOptr + CONTROL_BLOCK_HEAD_OFFSET).get() , addrZero , NEXT_FREE_BLOCK_OPTR_LENGTH );
	}
	else
	{
		if( freeBlockHead() == nullptr ) // フリーブロックが一つも登録されていなければ
		{
			std::cout << "既存のフリーブロックは何も登録されていません" << "\n";
			omemcpy( (*_primaryOptr + CONTROL_BLOCK_HEAD_OFFSET).get() , targetControlBlock->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH );
			freeBlockHead()->prevControlBlock( freeBlockHead().get() );
			freeBlockHead()->nextControlBlock( freeBlockHead().get() );
			return;	
		}

		targetControlBlock->nextControlBlock( freeBlockHead().get() );
		targetControlBlock->prevControlBlock( freeBlockHead()->prevControlBlock().get() );
		freeBlockHead()->prevControlBlock( targetControlBlock );

		std::cout << "debug marker 3" << "\n";
		printf("%p\n", freeBlockHead()->nextControlBlock().get() );
		printf("%p\n", targetControlBlock );
		printf("%p\n", targetControlBlock->blockOptr()->cacheTable() );

		std::cout << "-----------------" << "\n";
		freeBlockHead()->blockOptr()->printAddr(); std::cout << "\n";
		freeBlockHead()->prevControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		freeBlockHead()->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
		std::cout << "-----------------" << "\n";
		printf("%p\n", freeBlockHead()->prevControlBlock()->blockOptr()->cacheTable() );
		printf("%p\n", freeBlockHead()->nextControlBlock()->blockOptr()->cacheTable() );

		freeBlockHead()->prevControlBlock()->nextControlBlock( targetControlBlock );
		std::cout << "debug marker 4" << "\n";

		omemcpy( (*_primaryOptr + CONTROL_BLOCK_HEAD_OFFSET).get() , targetControlBlock->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH );
	}
	return;
}





std::unique_ptr<ControlBlock> MetaBlock::freeBlockHead() // 要修正
{
	if( _primaryOptr == nullptr ) return nullptr;

	unsigned char addrZero[5] = {0,0,0,0,0};
	if( ocmp( (*_primaryOptr + (CONTROL_BLOCK_HEAD_OFFSET)).get() , addrZero , CONTROL_BLOCK_LENGTH ) == 0 )
		return nullptr;

	optr *retOptr = new optr;
	unsigned char retAddr[5] = {0,0,0,0,0};
	omemcpy( retAddr , (*_primaryOptr + CONTROL_BLOCK_HEAD_OFFSET).get() , CONTROL_BLOCK_LENGTH );
	retOptr->cacheTable( _primaryOptr->cacheTable() ); retOptr->addr( retAddr );
	ControlBlock retControlBlock( retOptr );

	return std::make_unique<ControlBlock>( retControlBlock );
}





void MetaBlock::allocatedBlockHead( ControlBlock *targetAllodatedBlock , ControlBlock *test  )
{
	if( _primaryOptr == nullptr ) return;

	if( targetAllodatedBlock == nullptr )
	{
		unsigned char addrZero[5] = {0,0,0,0,0};
		omemcpy( (*_primaryOptr + ALLOCATED_BLOCK_HEAD_OFFSET).get(), addrZero , CONTROL_BLOCK_LENGTH );
		std::cout << "target allocated block is same allocated block head" << "\n";
	}
	else
	{
		std::cout << "ここに入ります" << "\n";
		if( allocatedBlockHead() == nullptr )
		{
			std::cout << "パターン1" << "\n";
			targetAllodatedBlock->blockOptr()->printAddr(); std::cout << "\n";
			test->blockOptr()->printAddr(); std::cout << "\n";
			test->prevControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
			test->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";

			targetAllodatedBlock->prevControlBlock( targetAllodatedBlock );
			targetAllodatedBlock->nextControlBlock( targetAllodatedBlock );

			std::cout << "  ----------" << "\n";
			test->prevControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
			test->nextControlBlock()->blockOptr()->printAddr(); std::cout << "\n";
			std::cout << "  ----------" << "\n";


			omemcpy( (*_primaryOptr + ALLOCATED_BLOCK_HEAD_OFFSET).get(), targetAllodatedBlock->blockAddr() , CONTROL_BLOCK_LENGTH );
			return;
		}

		std::cout << "パターン2" << "\n";
		targetAllodatedBlock->nextControlBlock( allocatedBlockHead().get() );
		targetAllodatedBlock->prevControlBlock( allocatedBlockHead()->prevControlBlock().get() );
		allocatedBlockHead()->prevControlBlock( targetAllodatedBlock );
		allocatedBlockHead()->prevControlBlock()->nextControlBlock( targetAllodatedBlock );
		omemcpy( (*_primaryOptr + ALLOCATED_BLOCK_HEAD_OFFSET).get(), targetAllodatedBlock->blockOptr()->addr() , CONTROL_BLOCK_LENGTH );
	}
	return;
}



std::unique_ptr<ControlBlock> MetaBlock::allocatedBlockHead()
{
	if( _primaryOptr == nullptr ) return nullptr;

	unsigned char addrZero[5] = {0, 0, 0, 0, 0};
	if( ocmp( (*_primaryOptr + (ALLOCATED_BLOCK_HEAD_OFFSET)).get() , addrZero , CONTROL_BLOCK_LENGTH ) == 0 )
		return nullptr;

	optr *retOptr = new optr;
	unsigned char retAddr[5]; memset( retAddr , 0x00 , CONTROL_BLOCK_LENGTH ); omemcpy( retAddr , (*_primaryOptr + ALLOCATED_BLOCK_HEAD_OFFSET).get() , CONTROL_BLOCK_LENGTH );
	retOptr->cacheTable( _primaryOptr->cacheTable() ); retOptr->addr( retAddr );
	ControlBlock retControlBlock( retOptr );

	return std::make_unique<ControlBlock>( retControlBlock );
}




void MetaBlock::unUsedControlBlockHead( ControlBlock* targetUnUsedControlBlock )
{
	if( _primaryOptr == nullptr ) return;

	if( targetUnUsedControlBlock == nullptr )
	{
		unsigned char addrZero[5] = {0,0,0,0,0};
		omemcpy( (*_primaryOptr + UNUSED_BLOCK_HEAD_OFFSET).get(), addrZero , CONTROL_BLOCK_LENGTH );
	}
	else
	{
		if( unUsedControlBlockHead() == nullptr )
		{
			omemcpy( (*_primaryOptr + UNUSED_BLOCK_HEAD_OFFSET ).get() , targetUnUsedControlBlock->blockOptr()->addr() , CONTROL_BLOCK_LENGTH );
			return;
		}
		targetUnUsedControlBlock->nextControlBlock( unUsedControlBlockHead().get() );
		targetUnUsedControlBlock->prevControlBlock( unUsedControlBlockHead()->prevControlBlock().get() );
		unUsedControlBlockHead()->prevControlBlock( targetUnUsedControlBlock );
		unUsedControlBlockHead()->prevControlBlock()->nextControlBlock( targetUnUsedControlBlock );

		omemcpy( (*_primaryOptr + UNUSED_BLOCK_HEAD_OFFSET ).get() , targetUnUsedControlBlock->blockOptr()->addr() , CONTROL_BLOCK_LENGTH );	
	}
	return;
}




// 使用と同時に使用済みになる
std::unique_ptr<ControlBlock> MetaBlock::unUsedControlBlockHead()
{

	std::cout << "unUsedControlBlockHead() called" << "\n";

	unsigned char unUsedControlBlockOptrAddr[5] = {0,0,0,0,0};
	omemcpy( unUsedControlBlockOptrAddr , (*_primaryOptr + UNUSED_BLOCK_HEAD_OFFSET).get() , CONTROL_BLOCK_LENGTH  );


	optr* unUsedControlBlockOptr = new optr( unUsedControlBlockOptrAddr );
	ControlBlock unUsedControlBlock( unUsedControlBlockOptr );


	std::cout << "unUsedCOntrolBlockOptrAddr -> " << "\n";  (*_primaryOptr + UNUSED_BLOCK_HEAD_OFFSET)->printValueContinuously(5); std::cout << "\n";
	unUsedControlBlock.blockOptr()->printAddr(); std::cout << "\n";

	unsigned char addrZero[5] = {0,0,0,0,0};
	if( memcmp( unUsedControlBlock.blockAddr() , addrZero , 5 ) == 0 ){
		std::cout << "unUsedControlBlockOptr retuend nullptr" << "\n";
		return nullptr;
	}



	unsigned char* unUsedfreeBlockHeadAddr = new unsigned char[5];
	omemcpy( unUsedfreeBlockHeadAddr , (*_primaryOptr + UNUSED_BLOCK_HEAD_OFFSET).get() , CONTROL_BLOCK_LENGTH );

	optr *unUsedfreeBlockHeadOptr = new optr( unUsedfreeBlockHeadAddr );
	unUsedfreeBlockHeadOptr->cacheTable( _primaryOptr->cacheTable() );

	ControlBlock retControlBlock( unUsedfreeBlockHeadOptr );

	if( memcmp( retControlBlock.blockAddr() , retControlBlock.nextControlBlock()->blockAddr() , CONTROL_BLOCK_LENGTH ) == 0 ) // 未使用ブロックが今回の使用でになる場合
		unUsedControlBlockHead( nullptr );
	else {
		retControlBlock.nextControlBlock()->prevControlBlock( retControlBlock.prevControlBlock().get() );
		retControlBlock.prevControlBlock()->nextControlBlock( retControlBlock.nextControlBlock().get() );
		unUsedControlBlockHead( retControlBlock.nextControlBlock().get() ); // 対象をチェーンから外す
	}

	/* 本当は確実にチェーンを切ったほうが良い
	retControlBlock.nextControlBlock( nullptr );
	retControlBlock.prevControlBlock( nullptr );
	*/

	return std::make_unique<ControlBlock>( retControlBlock );
}



void MetaBlock::controlBlockTail( ControlBlock* targetControlBlock )
{
	if( _primaryOptr == nullptr ) return;

	if( targetControlBlock == nullptr )
	{
		unsigned char addrZero[5] = {0,0,0,0,0};
		omemcpy( (*_primaryOptr + CONTROL_BLOCK_TAIL_OFFSET).get() , &addrZero , CONTROL_BLOCK_LENGTH );
	}
	else
	{
		omemcpy( (*_primaryOptr + CONTROL_BLOCK_TAIL_OFFSET).get() , targetControlBlock->blockOptr()->addr() , CONTROL_BLOCK_LENGTH );
	}
}



std::unique_ptr<ControlBlock> MetaBlock::controlBlockTail()
{
	unsigned char controlBlockTailOptrAddr[5] = {0,0,0,0,0};
	omemcpy( controlBlockTailOptrAddr , (*_primaryOptr + CONTROL_BLOCK_TAIL_OFFSET).get() , CONTROL_BLOCK_LENGTH  );
	optr* controlBlockTailOptr = new optr( controlBlockTailOptrAddr ); 
	controlBlockTailOptr->cacheTable( _primaryOptr->cacheTable() );

	ControlBlock controlBlockTail( controlBlockTailOptr );

	unsigned addrZero[5] = {0,0,0,0,0};
	if( memcmp( controlBlockTail.blockAddr(), &addrZero, CONTROL_BLOCK_LENGTH ) == 0 )
		return nullptr;

	return std::make_unique<ControlBlock>( controlBlockTail );
}







bool MetaBlock::isFileFormatted() // ファイル先頭のフォーマットIDが一致したら初期化されている
{
	if( ocmp( _primaryOptr , (unsigned char *)(FORMAT_ID) , 15 ) == 0 )  {
		std::cout  << "[ ATTENTION ] is Formated" << "\n";
		return true;
	}

	std::cout  << "[ ATTENTION ] is Not Formated" << "\n";
	return false;
}



}
