#include "MMyISAM.h"

#include "./components/page_table/overlay_memory_manager.h"
#include "./components/page_table/overlay_memory_allocator.h"
#include "./components/page_table/overlay_ptr.h"
#include "./components/page_table/optr_utils.h"
#include "./components/page_table/cache_manager/cache_table.h"
#include "./components/page_table/cache_manager/mapper/mapper.h"

#include "./components/index_manager/index_manager.h"
#include "./components/value_store_manager/value_store_manager.h"

#include "../../miya_db/query_context/query_context.h"

namespace miya_db
{


MMyISAM::MMyISAM( std::string fileName )
{   
    // 初期化の順番は
    // 1. memoryManager
    // 2. indexManager

    // データストアの初期化
    std::shared_ptr<OverlayMemoryManager> dataOverlayMemoryManager = std::shared_ptr<OverlayMemoryManager>( new OverlayMemoryManager(fileName) );
		_valueStoreManager = std::shared_ptr<ValueStoreManager>( new ValueStoreManager(dataOverlayMemoryManager) );

    //std::shared_ptr<optr> temp = dataOverlayMemoryManager->allocate(12);
    //omemcpy( temp.get() , (unsigned char*)"HelloWorld1", 12 );
    printf("(1)dataCache -> %p\n", dataOverlayMemoryManager->dataCacheTable().get() );
    printf("(1)freeListCache -> %p\n", dataOverlayMemoryManager->freeListCacheTable().get() );

		//std::cout << "----------------------------------------" << "\n";
		//printf(" dataCache addr -> %p\n", dataOverlayMemoryManager->memoryAllocator()->dataCacheTable().get());
		//printf(" mapper addr -> %p\n", dataOverlayMemoryManager->memoryAllocator()->dataCacheTable()->_mapper.get() );
		//printf("%p\n", dataOverlayMemoryManager->memoryAllocator()->dataCacheTable()->_mapper );
		//printf("%d\n", dataOverlayMemoryManager->memoryAllocator()->dataCacheTable()->_mapper->_systemPageSize);
		//printf("%d\n", dataOverlayMemoryManager->memoryAllocator()->dataCacheTable()->_mapper->_fd);
	
		/*
		std::shared_ptr<optr> temp_1 = dataOverlayMemoryManager->allocate( 11 );
		printf("%p\n", temp_1->cacheTable() );
		std::cout << "------------------------" << "\n";
		omemcpy( temp_1.get() , (unsigned char*)"HelloWorld1", 11 );
		*/	
    sleep(2);

    std::cout << "ValueStoreManager setuped" << "\n";

    // インデックスの初期化
    fileName += "_index";
    std::shared_ptr<OverlayMemoryManager> indexOverlayMemoryManager = std::shared_ptr<OverlayMemoryManager>( new OverlayMemoryManager(fileName) );
    _indexManager = std::shared_ptr<IndexManager>( new IndexManager(indexOverlayMemoryManager) );

  	

		std::shared_ptr<optr> temp_2 = indexOverlayMemoryManager->allocate(11);
    omemcpy( temp_2.get() , (unsigned char*)"HelloWorld2", 11 );
    printf("(2)dataCache -> %p\n", indexOverlayMemoryManager->dataCacheTable().get() );
    printf("(2)freeListCache -> %p\n", indexOverlayMemoryManager->freeListCacheTable().get() );

    std::cout << "----------------------------------------" << "\n";
		printf("%p\n", indexOverlayMemoryManager->memoryAllocator()->dataCacheTable()->_mapper.get() );
		printf("%d\n", indexOverlayMemoryManager->memoryAllocator()->dataCacheTable()->_mapper->_fd );
		


	
    sleep(2);
}




bool MMyISAM::add( std::shared_ptr<QueryContext> qctx )
{
    // データの保存
    //std::shared_ptr<optr> storeTarget = _dataOverlayMemoryManager->allocate( qctx->valueLength() );
    //omemcpy( storeTarget.get() , qctx->value().get() , qctx->valueLength() );
    std::cout << "MMyISAM::add() called" << "\n";

		std::shared_ptr<optr> storeTarget = _valueStoreManager->add( qctx );


    // インデックスの作成
    _indexManager->add( qctx->key() , storeTarget );
	

		return true;
}



bool MMyISAM::get( std::shared_ptr<QueryContext> qctx )
{
    std::shared_ptr<optr> dataOptr = _indexManager->find( qctx->key() ); // ここで取得されるoptrにはキャッシュテーブルがセットされていない

		size_t dataLength; std::shared_ptr<unsigned char> data;
		if( dataOptr == nullptr ) return false;

		dataLength = _valueStoreManager->get( dataOptr , &data );

		qctx->value( data , dataLength );

		return true;
}





void MMyISAM::hello()
{
	std::cout << "Hello" << "\n";
}



};
