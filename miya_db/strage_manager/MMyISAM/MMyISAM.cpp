#include "MMyISAM.h"

#include "./components/page_table/overlay_memory_manager.h"
#include "./components/page_table/overlay_memory_allocator.h"
#include "./components/page_table/overlay_ptr.h"
#include "./components/page_table/optr_utils.h"

#include "./components/index_manager/index_manager.h"

namespace miya_db
{


MMyISAM::MMyISAM( std::string fileName )
{   
    // 初期化の順番は
    // 1. memoryManager
    // 2. indexManager

    // データストアの初期化
    _dataOverlayMemoryManager  = std::shared_ptr<OverlayMemoryManager>( new OverlayMemoryManager(fileName) );
    
    // インデックスの初期化
    fileName += "_index";
    std::shared_ptr<OverlayMemoryManager> _indexOverlayMemoryManager = std::shared_ptr<OverlayMemoryManager>( new OverlayMemoryManager(fileName) );
    _indexManager = std::shared_ptr<IndexManager>( new IndexManager(_indexOverlayMemoryManager) );

}




void MMyISAM::regist( std::shared_ptr<unsigned char> data , size_t dataLength , std::shared_ptr<unsigned char> key )
{
    // データの保存
    std::shared_ptr<optr> storeTarget = _dataOverlayMemoryManager->allocate(dataLength);
    omemcpy( storeTarget.get() , data.get() , dataLength );

    // インデックスの作成
    _indexManager->add( key , storeTarget );
}


std::shared_ptr<optr> MMyISAM::get( std::shared_ptr<unsigned char> key )
{
    std::shared_ptr<optr> ret = _indexManager->find( key ); // ここで取得されるoptrにはキャッシュテーブルがセットされていない
    ret->cacheTable( _dataOverlayMemoryManager->memoryAllocator()->dataCacheTable().get() );

    return ret;
}


};
