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

    // インデックスの初期化
    fileName += "_index";
    std::shared_ptr<OverlayMemoryManager> indexOverlayMemoryManager = std::shared_ptr<OverlayMemoryManager>( new OverlayMemoryManager(fileName) );
    _indexManager = std::shared_ptr<IndexManager>( new IndexManager(indexOverlayMemoryManager) );

}




bool MMyISAM::add( std::shared_ptr<QueryContext> qctx )
{
    // データの保存
		std::shared_ptr<optr> storeTarget = _valueStoreManager->add( qctx ); // 先にデータ()を配置する

    // インデックスの作成
    _indexManager->add( qctx->key() , storeTarget );

		return true;
}



bool MMyISAM::get( std::shared_ptr<QueryContext> qctx )
{
    std::shared_ptr<optr> dataOptr = _indexManager->find( qctx->key() ); // ここで取得されるoptrにはキャッシュテーブルがセットされていない

		if( dataOptr == nullptr ) return false;
		size_t dataLength; std::shared_ptr<unsigned char> data;

		dataLength = _valueStoreManager->get( dataOptr , &data );

		qctx->value( data , dataLength );

		return true;
}




bool MMyISAM::exists( std::shared_ptr<QueryContext> qctx )
{
	std::shared_ptr<optr> dataOptr = nullptr;
	dataOptr = _indexManager->find( qctx->key() );

	// 簡易的にキーが存在するか否かでデータの存在有無を判別することとする
	if( dataOptr == nullptr ) return false; // キーが存在しない : false
	return true; // キーが存在しない : true
}




void MMyISAM::hello()
{
	std::cout << "Hello" << "\n";
}



};
