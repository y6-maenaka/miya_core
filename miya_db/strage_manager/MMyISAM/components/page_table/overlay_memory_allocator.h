#ifndef B6103BCF_DE57_4E00_96CB_194A3C316C52
#define B6103BCF_DE57_4E00_96CB_194A3C316C52


#include <iostream>
#include <memory>
#include <limits>
#include <unistd.h>
#include <stdexcept>

#include "./overlay_ptr.h"

namespace miya_db
{


#define FORMAT_ID "HELLOMIYACOIN!!"



/* フリーブロック管理ブロック*/
constexpr unsigned int PREV_FREE_BLOCK_OPTR_LENGTH = 5; // [bytes]
constexpr unsigned int PREV_FREE_BLOCK_OPTR_OFFSET = 0;

constexpr unsigned int NEXT_FREE_BLOCK_OPTR_LENGTH = 5; // [bytes]
constexpr unsigned int NEXT_FREE_BLOCK_OPRT_OFFSET = PREV_FREE_BLOCK_OPTR_OFFSET + PREV_FREE_BLOCK_OPTR_LENGTH;

constexpr unsigned int MAPPING_OPTR_LENGTH = 5; // [bytes]
constexpr unsigned int MAPPING_OPTR_OFFSET = NEXT_FREE_BLOCK_OPRT_OFFSET + NEXT_FREE_BLOCK_OPTR_LENGTH;

constexpr unsigned int FREE_BLOCK_END_OPTR_LENGTH = 5; // [bytes]
constexpr unsigned int FREE_BLOCK_END_OPTR_OFFSET = MAPPING_OPTR_OFFSET + MAPPING_OPTR_LENGTH;

constexpr unsigned int FREE_BLOCK_CONTROL_BLOCK_LENGTH = PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH + MAPPING_OPTR_LENGTH + FREE_BLOCK_END_OPTR_LENGTH;

constexpr unsigned int CONTROL_BLOCK_LENGTH = 5;

constexpr unsigned int DB_STATE_LENGTH = 20; // SHA1


/* メタブロック */
constexpr unsigned int META_BLOCK_SIZE = 100; // メタブロック(管理領域)のサイズ
constexpr unsigned int META_BLOCK_OFFSET = 0;
constexpr unsigned int CONTROL_BLOCK_HEAD_OFFSET = sizeof(FORMAT_ID) - 1; // 15
constexpr unsigned int ALLOCATED_BLOCK_HEAD_OFFSET = CONTROL_BLOCK_HEAD_OFFSET + 5; // 20
constexpr unsigned int UNUSED_BLOCK_HEAD_OFFSET = ALLOCATED_BLOCK_HEAD_OFFSET + 5; // 25
constexpr unsigned int CONTROL_BLOCK_TAIL_OFFSET = UNUSED_BLOCK_HEAD_OFFSET + 5; // 30
constexpr unsigned int DB_STATE_OFFSET = CONTROL_BLOCK_TAIL_OFFSET + DB_STATE_LENGTH;




/*
 [ フリーブロック管理ブロックの構造 ]
 	- Prev Control Block
	- Next Control Block
	- Mapping Optr
	- Free Block End Optr
*/


class optr;
class CacheTable;
struct ControlBlock;




/*
 -------------------------------------------------------------------------------------------
  | FORMAT_ID | CONTROL_BLOCK_HEAD | ALLOCATED_BLOCK_HEAD | UNUSED_BLOCK_HEAD | CONTROL_BLOCK_TAIL
 --------------------------------------------------------------------------------------------
*/


struct MetaBlock // 管理領域
{
private:


public:
	optr* _primaryOptr = nullptr;
	MetaBlock( optr *_primaryOptr );

	optr* primaryOptr();

	void freeBlockHead( ControlBlock *targetControlBlock ); // 先頭にセットする機能も備える
	std::unique_ptr<ControlBlock> freeBlockHead();

	void allocatedBlockHead( ControlBlock *targetAllodatedBlock ); // 先頭にセットする機能も兼ねる
	std::unique_ptr<ControlBlock> allocatedBlockHead();

	std::unique_ptr<ControlBlock> useUnUsedControlBlockHead();
	void unUsedControlBlockHead( ControlBlock* targetUnUsedControlBlock ); // 先頭にセットする機能も備える
	std::unique_ptr<ControlBlock> unUsedControlBlockHead();

	void controlBlockTail( ControlBlock* targetControlBlock );
	std::unique_ptr<ControlBlock> controlBlockTail();

	void dbState( std::shared_ptr<unsigned char> target );
	size_t dbState( std::shared_ptr<unsigned char> *ret );

	bool isFormatted();
	bool isFileFormatted();
};













/*
 -----------------------------------------------------------------------------------
  | PrevControlBlock(5) | NextControlBlock(5) | MappingOptr(5) | MappingEndOptr(5) |
 -----------------------------------------------------------------------------------
*/


// 先頭ポインタをラップして取り扱いが簡単になるようにしている
struct ControlBlock
{
private:
	//optr *_blockOptr;
	std::shared_ptr<optr> _blockOptr;

public:
	ControlBlock(){};
	ControlBlock( std::shared_ptr<optr> optr );

	std::shared_ptr<optr> blockOptr(); // getter
	unsigned char* blockAddr();

  //  ----- このメソッドでリターンされるoptrには,cacheTableがセットされていないから要注意 ---------------
	std::unique_ptr<optr> mappingOptr();
	void mappingOptr( optr *target ); // 対象のoptrを投げる
	// ----------------------------------------------------------------------------------------------------

	// PrevControlBlock
	std::unique_ptr<ControlBlock> prevControlBlock();
	void prevControlBlock( ControlBlock* target );

	// NextControlBlock
	std::unique_ptr<ControlBlock> nextControlBlock();
	void nextControlBlock( ControlBlock* target );

	// ControlBlockEnd
	std::unique_ptr<optr> freeBlockEnd();
	void freeBlockEnd( optr* target );

	unsigned long freeBlockSize();

};







class OverlayMemoryAllocator
{
private:
	MetaBlock *_metaBlock;

	//CacheTable *_dataCacheTable = nullptr;
	std::shared_ptr<CacheTable> _dataCacheTable = nullptr;
	//CacheTable *_freeListCacheTable = nullptr;
	std::shared_ptr<CacheTable> _freeListCacheTable = nullptr;

protected:
	// 管理ファイルを初期化するので迂闊に呼び出さない
	void init(); // 初めのコントロールブロックを配置する

public:
	OverlayMemoryAllocator( int dataFileFD  = -1 , int freeListFileFD = -1 );

	std::shared_ptr<optr> allocate( unsigned long allocateSize );
	void deallocate( optr *target );

	std::unique_ptr<ControlBlock> freeBlockHead();
	std::unique_ptr<ControlBlock> allocatedBlockHead();
	std::unique_ptr<ControlBlock> unUsedControlBlockHead();

	std::unique_ptr<ControlBlock> findFreeBlock( optr* targetOptr ); // 指定のアドレスのマッピング情報を持つフリーブロックを検索する
	std::unique_ptr<ControlBlock> findFreeBlock( ControlBlock *targetControlBlock, unsigned int allocateSize ); // 指定のサイズ以上のマッピング領域を持つフリーブロックを検索する
	std::unique_ptr<ControlBlock> findAllocatedBlock( optr *targetOptr );
	std::unique_ptr<ControlBlock> targetOptrPrevControlBlock( ControlBlock *targetControlBlock , optr *targetOptr );

	std::unique_ptr<ControlBlock> placeControlBlock( optr* targetOptr , ControlBlock* prevControlBlock, ControlBlock *nextControlBlock , optr* freeBlockEnd , bool isReplace = false );
	std::unique_ptr<ControlBlock> rePlaceControlBlock( optr *destOptr , ControlBlock* fromControlBlock , optr* freeBlockEnd );

	// リストの繋げ直しも行う
	void useUnUsedControlBlock();
	std::unique_ptr<ControlBlock> newControlBlock();

	void toUnUsedControlBlock( ControlBlock* targetControlBlock );

	void mergeControlBlock( ControlBlock *targetControlBlock );

	void printControlFile();
	void printFreeBlockChain();

	std::shared_ptr<optr> get( unsigned char* oAddr );

	void clear(); // 管理ファイルを実質削除&フォーマットする

	std::shared_ptr<CacheTable> dataCacheTable();
	std::shared_ptr<CacheTable> freeListCacheTable();

	MetaBlock* metaBlock(); // あまり使用しないようにする
}; // close miya_db
  



   
   

#endif // B6103BCF_DE57_4E00_96CB_194A3C316C52


};
