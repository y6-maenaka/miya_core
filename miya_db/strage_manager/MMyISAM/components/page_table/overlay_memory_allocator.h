#ifndef B6103BCF_DE57_4E00_96CB_194A3C316C52
#define B6103BCF_DE57_4E00_96CB_194A3C316C52


#include <iostream>
#include <memory>
#include "./overlay_ptr.h"





namespace miya_db
{


constexpr unsigned int PREV_FREE_BLOCK_PTR_LENGTH = 5; // [bytes]
constexpr unsigned int NEXT_FREE_BLOCK_PTR_LENGTH = 5; // [bytes]
constexpr unsigned int END_FREE_BLOCK_PTR_LENGTH = 5; // [bytes]


/*
 structure of free block ptr

	- prev_free_block_ptr
	- next_free_block_ptr
	- free_block_end_ptr

*/





struct FreeBlockControlBlock
{
private:
	optr *_optr = nullptr;

public:
	FreeBlockControlBlock( optr *optr );

	std::unique_ptr<FreeBlockControlBlock> prevControlBlock();
	std::unique_ptr<FreeBlockControlBlock> nextControlBlock();
	std::unique_ptr<optr> freeBlockEnd();
	unsigned int freeBlockSize();
};

std::unique_ptr<FreeBlockControlBlock> findFreeBlock( std::unique_ptr<FreeBlockControlBlock> targetControlBlock , unsigned int allocateSize );





class OverlayMemoryAllocator
{
private:

public:
	std::unique_ptr<optr> allocate( unsigned int allocateSize );
	void unallocate( unsigned char *target , unsigned int size );
};






}; // close miya_db

#endif // B6103BCF_DE57_4E00_96CB_194A3C316C52



