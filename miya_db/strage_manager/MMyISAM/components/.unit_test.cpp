#include ".unit_test.h"


#include "./page_table/overlay_memory_allocator.h"
#include "./page_table/overlay_memory_manager.h"
#include "./page_table/overlay_ptr.h"
#include "./page_table/optr_utils.h"
#include "./page_table/cache_manager/cache_table.h"


#include "./value_store_manager/value_store_manager.h"
#include "./index_manager/index_manager.h"


void optr_cache_unit_test()
{

	std::string fileName = "test";
  	std::shared_ptr<miya_db::OverlayMemoryManager> OverlayMemoryManager_1 =  std::shared_ptr<miya_db::OverlayMemoryManager>( new miya_db::OverlayMemoryManager(fileName) );
	// std::shared_ptr<miya_db::ValueStoreManager> valueStoreManager = std::make_shared<miya_db::ValueStoreManager>( OverlayMemoryManager_1 );
	std::shared_ptr<miya_db::optr> optr_1 = OverlayMemoryManager_1->allocate( 100 );

	miya_db::omemcpy( optr_1.get() , (unsigned char*)"HelloWorld1", 11  );
	std::cout << "------------------------------------------" << "\n";
	printf("(1) cache addr -> %p\n", OverlayMemoryManager_1->memoryAllocator()->dataCacheTable().get() );
	printf("(1) mapper Addr -> %p\n", OverlayMemoryManager_1->memoryAllocator()->dataCacheTable()->_mapper.get() );



	fileName += "_index";
  	std::shared_ptr<miya_db::OverlayMemoryManager> OverlayMemoryManager_2 =  std::shared_ptr<miya_db::OverlayMemoryManager>( new miya_db::OverlayMemoryManager(fileName) );
	// std::shared_ptr<miya_db::IndexManager> indexManager = std::make_shared<miya_db::IndexManager>( OverlayMemoryManager_2 );
	std::shared_ptr<miya_db::optr> optr_2 = OverlayMemoryManager_2->allocate( 100 );

	miya_db::omemcpy( optr_2.get() , (unsigned char*)"HelloWorld2", 11 );
	std::cout << "------------------------------------------" << "\n";
	printf("(2) cache addr -> %p\n", OverlayMemoryManager_2->memoryAllocator()->dataCacheTable().get() );
	printf("(2)mapper Addr -> %p\n", OverlayMemoryManager_2->memoryAllocator()->dataCacheTable()->_mapper.get() );



}
