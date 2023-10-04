#include "MMyISAM.h"

#include "./components/page_table/overlay_memory_manager.h"
#include "./components/page_table/overlay_memory_allocator.h"
#include "./components/page_table/overlay_ptr.h"
#include "./components/page_table/optr_utils.h"

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
    std::shared_ptr<OverlayMemoryManager> dataOverlayMemoryManager=  std::shared_ptr<OverlayMemoryManager>( new OverlayMemoryManager(fileName) );
		_valueStoreManager = std::shared_ptr<ValueStoreManager>( new ValueStoreManager(dataOverlayMemoryManager) );

    std::cout << "ValueStoreManager setuped" << "\n";

    // インデックスの初期化
    fileName += "_index";
    std::shared_ptr<OverlayMemoryManager> indexOverlayMemoryManager = std::shared_ptr<OverlayMemoryManager>( new OverlayMemoryManager(fileName) );
    _indexManager = std::shared_ptr<IndexManager>( new IndexManager(indexOverlayMemoryManager) );

}




bool MMyISAM::add( std::shared_ptr<QueryContext> qctx )
{
    // データの保存
    //std::shared_ptr<optr> storeTarget = _dataOverlayMemoryManager->allocate( qctx->valueLength() );
    //omemcpy( storeTarget.get() , qctx->value().get() , qctx->valueLength() );
	

		std::shared_ptr<optr> storeTarget = _valueStoreManager->add( qctx );


    // インデックスの作成
    _indexManager->add( qctx->key() , storeTarget );
	

		return true;
}



bool MMyISAM::get( std::shared_ptr<QueryContext> qctx )
{
		puts("called()");

		std::cout << "---- 1 ----" << "\n";


    std::shared_ptr<optr> dataOptr = _indexManager->find( qctx->key() ); // ここで取得されるoptrにはキャッシュテーブルがセットされていない

		std::cout << "---- 2 ----" << "\n";

		size_t dataLength; std::shared_ptr<unsigned char> data;
		if( dataOptr == nullptr ) return false;

		dataLength = _valueStoreManager->get( dataOptr , &data );

		std::cout << "---- 3 ----" << "\n";

		qctx->value( data , dataLength );

		std::cout << "---- 4 ----" << "\n";

		return true;
}





void MMyISAM::hello()
{
	std::cout << "Hello" << "\n";
}



};
