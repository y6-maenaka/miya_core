#ifndef B6103BCF_DE57_4E00_96CB_194A3C316C52
#define B6103BCF_DE57_4E00_96CB_194A3C316C52


#include <iostream>
#include <memory>
#include <limits>



namespace miya_db
{


constexpr unsigned int PREV_FREE_BLOCK_OPTR_LENGTH = 5; // [bytes]
constexpr unsigned int NEXT_FREE_BLOCK_OPTR_LENGTH = 5; // [bytes]
constexpr unsigned int END_FREE_BLOCK_OPTR_LENGTH = 5; // [bytes]


constexpr unsigned int FREE_BLOCK_CONTROL_BLOCK_LENGTH = PREV_FREE_BLOCK_OPTR_LENGTH + NEXT_FREE_BLOCK_OPTR_LENGTH + END_FREE_BLOCK_OPTR_LENGTH;
/*
 structure of free block ptr

	- prev_free_block_ptr
	- next_free_block_ptr
	- free_block_end_ptr

*/

class optr;
class CacheTable;


struct FreeBlockControlBlock
{
private:
	optr *_optr = nullptr;

public:
	FreeBlockControlBlock(){};
	FreeBlockControlBlock( optr *optr );
	std::unique_ptr<optr> Optr(); // getter

	std::unique_ptr<FreeBlockControlBlock> prevControlBlock();
	void prevControlBlock( optr* target );

	std::unique_ptr<FreeBlockControlBlock> nextControlBlock();
	void nextControlBlock( optr* target );

	std::unique_ptr<optr> freeBlockEnd();
	void freeBlockEnd( optr* target );

	unsigned long freeBlockSize();



	static void placeControlBlock( optr* targetPlaceOptr , optr* prevControlBlockOptr, optr* nextControlBlockOptr, optr* endFreeBlockOptr );
};



std::unique_ptr<FreeBlockControlBlock> findFreeBlock( FreeBlockControlBlock *targetControlBlock, unsigned int allocateSize );
std::unique_ptr<FreeBlockControlBlock> getHeadControlBlock( FreeBlockControlBlock* targetControlBlock ,optr* target );



class OverlayMemoryAllocator
{
private:	
	CacheTable *_cacheTable = nullptr;

protected:
	std::unique_ptr<FreeBlockControlBlock> getHeadControlBlock();

public:
	std::unique_ptr<optr> allocate( unsigned int allocateSize );
	void unallocate( optr *target , unsigned int size );
	
};






}; // close miya_db

#endif // B6103BCF_DE57_4E00_96CB_194A3C316C52


;
