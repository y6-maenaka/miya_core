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
			omemcpy( (*_primaryOptr + CONTROL_BLOCK_HEAD_OFFSET).get() , targetControlBlock->blockOptr()->addr() , NEXT_FREE_BLOCK_OPTR_LENGTH );
			freeBlockHead()->prevControlBlock( freeBlockHead().get() );
			freeBlockHead()->nextControlBlock( freeBlockHead().get() );
			return;	
		}

		targetControlBlock->nextControlBlock( freeBlockHead().get() ); // 新たなフリーブロック
		targetControlBlock->prevControlBlock( freeBlockHead()->prevControlBlock().get() );

		freeBlockHead()->prevControlBlock()->nextControlBlock( targetControlBlock );
		freeBlockHead()->prevControlBlock( targetControlBlock );

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

	//optr *retOptr = new optr;
	std::shared_ptr<optr> retOptr = std::make_shared<optr>();
	unsigned char retAddr[5] = {0,0,0,0,0};
	omemcpy( retAddr , (*_primaryOptr + CONTROL_BLOCK_HEAD_OFFSET).get() , CONTROL_BLOCK_LENGTH );
	retOptr->cacheTable( _primaryOptr->cacheTable() ); retOptr->addr( retAddr );
	ControlBlock retControlBlock( retOptr );

	return std::make_unique<ControlBlock>( retControlBlock );
}


void MetaBlock::allocatedBlockHead( ControlBlock *targetAllodatedBlock )
{
	if( _primaryOptr == nullptr ) return;

	if( targetAllodatedBlock == nullptr )
	{
		unsigned char addrZero[5] = {0,0,0,0,0};
		omemcpy( (*_primaryOptr + ALLOCATED_BLOCK_HEAD_OFFSET).get(), addrZero , CONTROL_BLOCK_LENGTH );
	}
	else
	{
		if( allocatedBlockHead() == nullptr )
		{

			omemcpy( (*_primaryOptr + ALLOCATED_BLOCK_HEAD_OFFSET).get(), targetAllodatedBlock->blockAddr() , CONTROL_BLOCK_LENGTH );
			allocatedBlockHead()->prevControlBlock( allocatedBlockHead().get() );
			allocatedBlockHead()->nextControlBlock( allocatedBlockHead().get() );
			return;
		}

		targetAllodatedBlock->nextControlBlock( allocatedBlockHead().get() );
		targetAllodatedBlock->prevControlBlock( allocatedBlockHead()->prevControlBlock().get() );

		allocatedBlockHead()->prevControlBlock()->nextControlBlock( targetAllodatedBlock );
		allocatedBlockHead()->prevControlBlock( targetAllodatedBlock );

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

	//optr *retOptr = new optr;
	std::shared_ptr<optr> retOptr = std::make_shared<optr>();
	unsigned char retAddr[5]; memset( retAddr , 0x00 , CONTROL_BLOCK_LENGTH ); omemcpy( retAddr , (*_primaryOptr + ALLOCATED_BLOCK_HEAD_OFFSET).get() , CONTROL_BLOCK_LENGTH );
	retOptr->cacheTable( _primaryOptr->cacheTable() ); retOptr->addr( retAddr );
	ControlBlock retControlBlock( retOptr );

	return std::make_unique<ControlBlock>( retControlBlock );
}


void MetaBlock::unUsedControlBlockHead( ControlBlock* targetUnUsedControlBlock )
{
	

	if( _primaryOptr == nullptr ){
 		return;
	}

	if( targetUnUsedControlBlock == nullptr )
	{
		unsigned char addrZero[5] = {0,0,0,0,0};
		omemcpy( (*_primaryOptr + UNUSED_BLOCK_HEAD_OFFSET).get(), addrZero , CONTROL_BLOCK_LENGTH );
	}
	else
	{
		if( unUsedControlBlockHead() == nullptr )
		{
			omemcpy( ((*_primaryOptr) + UNUSED_BLOCK_HEAD_OFFSET ).get() , targetUnUsedControlBlock->blockAddr() , CONTROL_BLOCK_LENGTH ); // 先頭未使用ブロックが置き換えられる
			((*_primaryOptr) + UNUSED_BLOCK_HEAD_OFFSET	)->printValueContinuously( 5 ); std::cout << "\n";

			unUsedControlBlockHead()->prevControlBlock( unUsedControlBlockHead().get() );
			unUsedControlBlockHead()->nextControlBlock( unUsedControlBlockHead().get() );

			return;
		}

		targetUnUsedControlBlock->nextControlBlock( unUsedControlBlockHead().get() );
		targetUnUsedControlBlock->prevControlBlock( unUsedControlBlockHead()->prevControlBlock().get() );

		unUsedControlBlockHead()->prevControlBlock()->nextControlBlock( targetUnUsedControlBlock );
		unUsedControlBlockHead()->prevControlBlock( targetUnUsedControlBlock );

		omemcpy( (*_primaryOptr + UNUSED_BLOCK_HEAD_OFFSET ).get() , targetUnUsedControlBlock->blockOptr()->addr() , CONTROL_BLOCK_LENGTH );	
	}
	return;
}


std::unique_ptr<ControlBlock> MetaBlock::unUsedControlBlockHead()
{
	if( _primaryOptr == nullptr ) return nullptr;

	unsigned char addrZero[5] = {0, 0, 0, 0, 0};
	if( ocmp( (*_primaryOptr + (UNUSED_BLOCK_HEAD_OFFSET)).get() , addrZero , CONTROL_BLOCK_LENGTH ) == 0 )
		return nullptr;

	unsigned char* retAddr = new unsigned char[5];
 	omemcpy( retAddr , (*_primaryOptr + UNUSED_BLOCK_HEAD_OFFSET).get() , CONTROL_BLOCK_LENGTH );

	//optr *retOptr = new optr( retAddr ); 
	std::shared_ptr<optr> retOptr = std::shared_ptr<optr>( new optr(retAddr) ); retOptr->cacheTable( _primaryOptr->cacheTable() ); 
	ControlBlock retControlBlock( retOptr );

	return std::make_unique<ControlBlock>( retControlBlock );
}


// 使用と同時に使用済みになる
std::unique_ptr<ControlBlock> MetaBlock::useUnUsedControlBlockHead()
{
	if( _primaryOptr == nullptr ) return nullptr;

	unsigned char addrZero[5] = {0, 0, 0, 0, 0};
	if( ocmp( (*_primaryOptr + (UNUSED_BLOCK_HEAD_OFFSET)).get() , addrZero , CONTROL_BLOCK_LENGTH ) == 0 )
		return nullptr;

	unsigned char* retAddr = new unsigned char[5];
 	omemcpy( retAddr , (*_primaryOptr + UNUSED_BLOCK_HEAD_OFFSET).get() , CONTROL_BLOCK_LENGTH );

	//optr *retOptr = new optr( retAddr ); 
	std::shared_ptr<optr> retOptr = std::shared_ptr<optr>( new optr(retAddr) ); retOptr->cacheTable( _primaryOptr->cacheTable() ); 
	ControlBlock retControlBlock( retOptr );

	/* 以降は未使用ブロックを使用済みにするシーケンス*/
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
	std::shared_ptr<optr> controlBlockTailOptr = std::shared_ptr<optr>( new optr(controlBlockTailOptrAddr) );
	//optr* controlBlockTailOptr = new optr( controlBlockTailOptrAddr ); 
	controlBlockTailOptr->cacheTable( _primaryOptr->cacheTable() );

	ControlBlock controlBlockTail( controlBlockTailOptr );

	unsigned addrZero[5] = {0,0,0,0,0};
	if( memcmp( controlBlockTail.blockAddr(), &addrZero, CONTROL_BLOCK_LENGTH ) == 0 )
		return nullptr;

	return std::make_unique<ControlBlock>( controlBlockTail );
}


void MetaBlock::dbState( std::shared_ptr<unsigned char> target )
{
  if( target == nullptr ) return;

  std::cout << "\x1b[36m" << "DB State SYNCESD" << "\x1b[39m" << "\n";
  omemcpy( (*_primaryOptr + (DB_STATE_OFFSET)).get() , target.get() , DB_STATE_LENGTH );
  return;
}

size_t MetaBlock::dbState( std::shared_ptr<unsigned char> *ret )
{
  *ret = std::shared_ptr<unsigned char>( new unsigned char[DB_STATE_LENGTH] );
  omemcpy( (*ret).get(), (*_primaryOptr + DB_STATE_OFFSET) , DB_STATE_LENGTH );
  return DB_STATE_LENGTH;
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
