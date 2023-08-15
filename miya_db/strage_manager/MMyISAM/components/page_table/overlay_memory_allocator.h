#ifndef B6103BCF_DE57_4E00_96CB_194A3C316C52
#define B6103BCF_DE57_4E00_96CB_194A3C316C52


#include <iostream>
#include <memory>
#include <limits>

#include "./overlay_ptr.h"

namespace miya_db
{


#define FORMAT_ID "HELLOMIYACOIN!!"


/* フリーブロック管理ブロック*/
constexpr unsigned int PREV_FREE_BLOCK_OPTR_LENGTH = 5; // [bytes]
constexpr unsigned int NEXT_FREE_BLOCK_OPTR_LENGTH = 5; // [bytes]	
constexpr unsigned int FREE_BLOCK_END_OPTR_LENGTH = 5; // [bytes]


/* メタブロック */
constexpr unsigned int META_BLOCK_SIZE = 100; // メタブロック(管理領域)のサイズ
constexpr unsigned int META_BLOCK_OFFSET = 0;
constexpr unsigned int CONTROL_BLOCK_HEAD_OFFSET = sizeof(FORMAT_ID) - 1;



constexpr unsigned int FREE_BLOCK_CONTROL_BLOCK_LENGTH = PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH + FREE_BLOCK_END_OPTR_LENGTH;
/*
 structure of free block ptr

	- prev_free_block_ptr
	- next_free_block_ptr
	- free_block_end_ptr
*/

class optr;
class CacheTable;
struct FreeBlockControlBlock;






struct MetaBlock // 管理領域
{
private:
	optr *_blockOptr = nullptr;


public:
	MetaBlock( optr *primaryOptr ) : _blockOptr( primaryOptr ) {};
	optr* blockOptr(){ return _blockOptr; };

	void controlBlockHead( FreeBlockControlBlock *targetControlBlock );
	std::unique_ptr<FreeBlockControlBlock> controlBlockHead();
	bool isFormatted();
	
	bool isFileFormatted();
};




// 先頭ポインタをラップして取り扱いが簡単になるようにしている
struct FreeBlockControlBlock
{
private:
	optr *_blockOptr = nullptr;

public:
	FreeBlockControlBlock(){};
	FreeBlockControlBlock( optr *optr );

	std::unique_ptr<optr> blockOptr(); // getter

	// PrevControlBlock
	std::unique_ptr<FreeBlockControlBlock> prevControlBlock();
	void prevControlBlock( FreeBlockControlBlock* target );
	void prevControlBlock( optr* target );

	// NextControlBlock
	std::unique_ptr<FreeBlockControlBlock> nextControlBlock();
	void nextControlBlock( FreeBlockControlBlock* target );
	void nextControlBlock( optr* target );

	// ControlBlockEnd
	std::unique_ptr<optr> freeBlockEnd();
	void freeBlockEnd( optr* target );

	unsigned long freeBlockSize();

};







class OverlayMemoryAllocator
{
private:	
	const optr* _primaryOptr = nullptr;
	MetaBlock *_metaBlock;

protected:

	void init(); // 初めのコントロールブロックを配置する


public:
	OverlayMemoryAllocator( optr *primaryOptr );
	~OverlayMemoryAllocator(){ delete _primaryOptr; };

	std::unique_ptr<optr> allocate( unsigned long allocateSize );
	void unallocate( optr *target , unsigned long size );

	std::unique_ptr<FreeBlockControlBlock> controlBlockHead();

	std::unique_ptr<FreeBlockControlBlock> findFreeBlock( FreeBlockControlBlock *targetControlBlock, unsigned int allocateSize ); 
	std::unique_ptr<FreeBlockControlBlock> targetOptrPrevControlBlock( FreeBlockControlBlock *targetControlBlock , optr *targetOptr );
	std::unique_ptr<FreeBlockControlBlock> placeControlBlock( optr* targetOptr , FreeBlockControlBlock* prevControlBlock, FreeBlockControlBlock *nextControlBlock , optr* freeBlockEnd );

	void mergeControlBlock( FreeBlockControlBlock *targetControlBlock );

	void printControlChain( FreeBlockControlBlock* targetControlBlock );
};





void showFreeBlockControlBlockList();


}; // close miya_db

#endif // B6103BCF_DE57_4E00_96CB_194A3C316C52


;
