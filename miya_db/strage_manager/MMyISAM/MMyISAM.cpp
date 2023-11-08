#include "MMyISAM.h"

#include "./components/page_table/overlay_memory_manager.h"
#include "./components/page_table/overlay_memory_allocator.h"
#include "./components/page_table/overlay_ptr.h"
#include "./components/page_table/optr_utils.h"
#include "./components/page_table/cache_manager/cache_table.h"
#include "./components/page_table/cache_manager/mapper/mapper.h"

#include "./components/index_manager/index_manager.h"
#include "./components/index_manager/normal_index_manager.h"

#include "./components/value_store_manager/value_store_manager.h"

#include "../../miya_db/query_context/query_context.h"

#include "./safe_mode_manager/safe_index_manager/safe_index_manager.h"

namespace miya_db
{


MMyISAM::MMyISAM( std::string filePath )
{   
    // 初期化の順番は
    // 1. memoryManager
    // 2. indexManager


   // データストアの初期化
    std::shared_ptr<OverlayMemoryManager> dataOverlayMemoryManager = std::shared_ptr<OverlayMemoryManager>( new OverlayMemoryManager(filePath) );
	_normal._valueStoreManager = std::shared_ptr<ValueStoreManager>( new ValueStoreManager(dataOverlayMemoryManager) );

    // インデックスの初期化
    filePath += "_index";
    std::shared_ptr<OverlayMemoryManager> indexOverlayMemoryManager = std::shared_ptr<OverlayMemoryManager>( new OverlayMemoryManager(filePath) );
    _normal._indexManager = std::shared_ptr<IndexManager>( new NormalIndexManager(indexOverlayMemoryManager) );


	// 初回起動時は通常モードで起動する
	_valueStoreManager = _normal._valueStoreManager.get(); 
	_indexManager = dynamic_cast<IndexManager*>(_normal._indexManager.get());

}




bool MMyISAM::add( std::shared_ptr<QueryContext> qctx )
{
    // データの保存
	std::shared_ptr<optr> storeTarget = _valueStoreManager->add( qctx ); // 先にデータ()を配置する

    // インデックスの作成
    _indexManager->add( qctx->key() , storeTarget );

	std::cout << "\x1b[36m" << "(MiyaDB) add with key :: ";
	for( int i=0; i<20; i++){
		printf("%02X", qctx->key().get()[i]);
	} std::cout << "\x1b[39m" << "\n";



	return true;
}



bool MMyISAM::get( std::shared_ptr<QueryContext> qctx )
{
    std::shared_ptr<optr> dataOptr = _indexManager->find( qctx->key() ); // ここで取得されるoptrにはキャッシュテーブルがセットされていない

	std::cout << "\x1b[32m" << "(MiyaDB) get with key :: ";
	for( int i=0; i<20; i++){
		printf("%02X", qctx->key().get()[i]);
	} std::cout << "\x1b[39m" << "\n";


		if( dataOptr == nullptr ) return false;
		size_t dataLength; std::shared_ptr<unsigned char> data;

		dataLength = _valueStoreManager->get( dataOptr , &data );

		qctx->value( data , dataLength );

		return true;
}





bool MMyISAM::remove( std::shared_ptr<QueryContext> qctx )
{
	std::cout << "\x1b[35m" << "(MiyaDB) remove with key :: ";
	for( int i=0; i<20; i++){
		printf("%02X", qctx->key().get()[i] );
	}  std::cout << "\x1b[39m"  << "\n";

	_indexManager->remove( qctx->key() );
 // 本来はvalueStoreからも削除する

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
