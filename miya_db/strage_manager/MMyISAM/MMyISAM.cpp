#include "MMyISAM.h"





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
    _indexManager = std::shared_ptr<OverlayMemoryManager>( new OverlayMemoryManager(_indexOverlayMemoryManager) );

}




void MMyISAM::regist( std::shared_ptr<unsigned char> data , size_t dataLength , std::shared_ptr<unsigned char> key )
{
    // データの保存
    std::shared_ptr<optr> storeTarget = _dataOverlayMemoryManager->allocate(dataLength);
    omemcpy( storeTarget.get() , data.get() , dataLength );

    // インデックスの作成
    _indexManager->add( key , storeTarget );
}


void MMyISAM::get( std::shared_ptr<unsigned char> key )
{
    std::shared_ptr<optr> ret = _indexManager->get( key ); // ここで取得されるoptrにはキャッシュテーブルがセットされていない
    ret->cacheTable( _dataOverlayMemoryManager->dataCacheTable() );

    return ret;
}
