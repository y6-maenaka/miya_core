#include "unit_test.h"




#include "./overlay_ptr.h"
#include "./optr_utils.h"
#include "./overlay_memory_manager.h"
#include "./overlay_memory_allocator.h"
#include "./cache_manager/cache_table.h"
#include "./cache_manager/mapper/mapper.h"





int deallocate_test_pattern_1()
{

	/*
	int dataFd = open("../miya_db/table_files/test_table/test.oswap", O_RDWR , (mode_t)0600 );
	int freeListFd = open("../miya_db/table_files/test_table/test.ofl", O_RDWR , (mode_t)0600 );

	miya_db::OverlayMemoryManager *manager = new miya_db::OverlayMemoryManager(dataFd , freeListFd );



	
	unsigned char* ret = new unsigned char[100];
	memset( ret, 0x2D , 100 );
	//omemcpy( allocatedOptr.get() ,  ret , 100 );

	std::cout << "\n !!== MemoryAllocator Successfully Done ==! \n" << "\n";

	
	
	std::unique_ptr<miya_db::optr> allocatedOptr_1 = manager->allocate( 200 );
	std::cout << "------------------------" << "\n";
	std::cout << "[ NEW ALLOCATE ] : "; allocatedOptr_1->printAddr(); std::cout << "\n";
	std::cout << "------------------------" << "\n";
	//miya_db::omemcpy( allocatedOptr_1.get() , (unsigned char*)"HelloWorld",  10 );

	// 適切に0x64のコントロールブロックが解放されてない



	std::cout << "\n\n ------------------- \n\n" << "\n";


	manager->deallocate( allocatedOptr_1.get() );
	std::cout << "\n\n ------------------- \n\n" << "\n";


	
	std::unique_ptr<miya_db::optr> allocatedOptr_2 = manager->allocate( 100 );
	std::cout << "------------------------" << "\n";
	std::cout << "[ NEW ALLOCATE ] : "; allocatedOptr_2->printAddr(); std::cout << "\n";
	std::cout << "------------------------" << "\n";



	// ここまでテストOK

	std::cout << "\n\n ------------------- \n\n" << "\n";

	std::unique_ptr<miya_db::optr> allocatedOptr_3 = manager->allocate( 100 );
	std::cout << "------------------------" << "\n";
	std::cout << "[ NEW ALLOCATE ] : "; allocatedOptr_3->printAddr(); std::cout << "\n";
	std::cout << "------------------------" << "\n";


	// ここまでテストOK



	std::cout << "\n\n ------------------- \n\n" << "\n";

	manager->deallocate( allocatedOptr_3.get() );






	std::cout << "\n\n ------------------- \n\n" << "\n";


	manager->deallocate( allocatedOptr_2.get() );
	


	manager->memoryAllocator()->printControlFile();
	return 0;
	*/

	/*
	manager->deallocate( allocatedOptr_2.get() , 100 );
	manager->deallocate( allocatedOptr_3.get() , 50 );
	std::cout << "解放処理が正常に終了しました" << "\n";


	std::cout << "Control Block Head :: "	;
	manager->memoryAllocator()->controlBlockHead()->blockOptr()->printAddr(); std::cout << "\n";

	manager->memoryAllocator()->printControlChain( manager->memoryAllocator()->controlBlockHead().get() );
	*/




	return 0;
}


int common_test()
{
	return 0;
}





int optr_test()
{

	int dataFd = open("../miya_db/table_files/test_table/test.oswap", O_RDWR , (mode_t)0600 );
	int freeListFd = open("../miya_db/table_files/test_table/test.ofl", O_RDWR , (mode_t)0600 );


	miya_db::OverlayMemoryManager *manager = new miya_db::OverlayMemoryManager( dataFd , freeListFd );

	/*
	std::cout << "-------------"	 << "\n";
	std::cout << "( 2 )" << "\n";
	miya_db::optr tmp;
	tmp.addr(2 );
	tmp.cacheTable( manager->cacheTable() );
	tmp.printAddr(); std::cout << "\n";
	std::cout << tmp.frame() << "\n";
	std::cout << tmp.offset() << "\n";
	tmp.value();
	std::cout << "-------------"	 << "\n";


	std::cout << "-------------"	 << "\n";
	std::cout << "( 507 )" << "\n";
	miya_db::optr _tmp;
	_tmp.addr(507 );
	_tmp.cacheTable( manager->cacheTable() );
	_tmp.printAddr(); std::cout << "\n";
	std::cout << _tmp.frame() << "\n";
	std::cout << _tmp.offset() << "\n";
	_tmp.value();
	std::cout << "-------------"	 << "\n";



	std::cout << "-------------"	 << "\n";
	std::cout << "( 508 )" << "\n";
	miya_db::optr __tmp;
	__tmp.addr(508 );
	__tmp.cacheTable( manager->cacheTable() );
	__tmp.printAddr(); std::cout << "\n";
	std::cout << __tmp.frame() << "\n";
	std::cout << __tmp.offset() << "\n";
	__tmp.value();
	std::cout << "-------------"	 << "\n";



	std::cout << "-------------"	 << "\n";
	std::cout << "( 509 )" << "\n";
	miya_db::optr ___tmp;
	___tmp.addr(509 );
	___tmp.cacheTable( manager->cacheTable() );
	___tmp.printAddr(); std::cout << "\n";
	std::cout << ___tmp.frame() << "\n";
	std::cout << ___tmp.offset() << "\n";
	___tmp.value();
	std::cout << "-------------"	 << "\n";

	std::cout << "-------------"	 << "\n";
	std::cout << "( 512 )" << "\n";
	miya_db::optr ____tmp;
	____tmp.addr(512 );
	____tmp.cacheTable( manager->cacheTable() );
	____tmp.printAddr(); std::cout << "\n";
	std::cout << ____tmp.frame() << "\n";
	std::cout << ____tmp.offset() << "\n";
	____tmp.value('@');
	std::cout << "-------------"	 << "\n";
	*/


	/*
	std::cout << "-------------"	 << "\n";
	std::cout << "( 110 . 6E  )" << "\n";
	miya_db::optr tmp;
	tmp.addr( 110 );
	tmp.cacheTable( manager->cacheTable() );
	tmp.printAddr(); std::cout << "\n";
	std::cout << tmp.frame() << "\n";
	std::cout << tmp.offset() << "\n";
	tmp.value();
	std::cout << "-------------"	 << "\n";
	*/
	


	/*
	std::cout << "\n\n\n\n";
	unsigned char addrZero[5] = {0x00,0x00,0x00,0x00,0x96};
	omemcpy( &tmp , addrZero , 5);
	tmp.printValueContinuously( 5 );
	*/

	//manager->cacheTable()->syncForce( 0);
	//(__tmp + 0)->value('@');
	//(__tmp + 1)->value('@');
	//(__tmp + 2)->value('@');
	//(__tmp + 3)->value('@');
	//(__tmp + 4)->value('@');
	//std::cout << (__tmp + 4)->offset() << "\n";
	//(__tmp + 4)->printAddr(); std::cout << "\n";
	



	return 0;

}




